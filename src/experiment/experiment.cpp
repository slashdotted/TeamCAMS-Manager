// TeamCAMS - reborn Cabin Air Management System
// Copyright (C) 2015-2026  Amos Brocco,
//                          Cognitive Ergonomics and Work Psychology Team,
//                          Psychology Department of Fribourg University,
//                          Switzerland / Department of Innovative Technologies
//                          University of Applied Sciences and Arts of Southern
//                          Switzerland, Contact: amos.brocco@supsi.ch
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
#include "experiment.h"
#include "config/xmlconfigparser.h"
#include "gui/mainwindow.h"
#include "operators/operatoradapter.h"
#include "simulation/registry.h"
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

// Private data
struct Experiment::pimpl {
  bool m_enabled;
  QString m_name;
  QString m_description;
  QString m_script;
  Simulator *m_simulator{nullptr};
  Scheduler *m_scheduler{nullptr};
  Triggers *m_triggers{nullptr};
  AccessManager *m_credential_manager{nullptr};
  OperatorsManager *m_connection_manager{nullptr};
  ExecutionManager *m_execution_manager{nullptr};
  LoggingManager *m_logging_manager{nullptr};
  ScriptingManager *m_scripting_manager{nullptr};
  OperatorAdapter *m_client_adapter{nullptr};
  ContentLibrary *m_content_library{nullptr};
  std::shared_ptr<ExperimentServer> m_communication_server{nullptr};
  std::shared_ptr<MessageTransport> m_transport{nullptr};
};

// Implementation
Experiment::Experiment(
    const QString &name,
    std::shared_ptr<CommunicationManager> communicationManager, QObject *parent)
    : QObject{parent}, m_pimpl{std::make_unique<pimpl>()} {
  m_pimpl->m_enabled = false;
  m_pimpl->m_name = name;
  m_pimpl->m_simulator = new Simulator{this};
  m_pimpl->m_scheduler = new Scheduler{this};
  m_pimpl->m_triggers = new Triggers{this};
  m_pimpl->m_credential_manager = new AccessManager{this};
  m_pimpl->m_connection_manager = new OperatorsManager{this};
  m_pimpl->m_execution_manager = new ExecutionManager{this};
  m_pimpl->m_logging_manager = new LoggingManager{this};
  m_pimpl->m_scripting_manager = new ScriptingManager{this};
  m_pimpl->m_client_adapter = new OperatorAdapter{this};
  m_pimpl->m_content_library = new ContentLibrary{this};
  if (communicationManager) {
    m_pimpl->m_communication_server =
        communicationManager->newExperimentServer(this);
    m_pimpl->m_transport = communicationManager->transport();
    connect(
        m_pimpl->m_communication_server.get(), &ExperimentServer::peerConnected,
        this, [this](std::shared_ptr<Peer> peer) {
          qDebug() << "[Experiment] Peer connected" << peer->card()->username();
          m_pimpl->m_connection_manager->add(peer->card(), peer);
        });
    connect(m_pimpl->m_communication_server.get(),
            &ExperimentServer::peerDisconnected, this,
            [this](std::shared_ptr<Peer> peer) {
              qDebug() << "[Experiment] Peer disconnected"
                       << peer->card()->username();
              m_pimpl->m_connection_manager->disconnect(peer->card());
            });
  }

  connect(m_pimpl->m_credential_manager, &AccessManager::contentChanged, this,
          &Experiment::onContentChanged);
  connect(m_pimpl->m_execution_manager, &ExecutionManager::tick,
          m_pimpl->m_simulator, &Simulator::update);
  connect(m_pimpl->m_execution_manager, &ExecutionManager::tick,
          m_pimpl->m_scheduler, &Scheduler::processEvents);
  connect(m_pimpl->m_execution_manager, &ExecutionManager::tick,
          m_pimpl->m_logging_manager, &LoggingManager::timerInterval);
  connect(m_pimpl->m_execution_manager, &ExecutionManager::stateChanged, this,
          [this](const ExecutionManager::ExecutionState &state,
                 const ExecutionManager::ExecutionState &previous) {
            auto initstate = m_pimpl->m_simulator->registry()
                                 ->get("system.state", true)
                                 .isNull();
            switch (state) {
            case ExecutionManager::ExecutionState::UNINITIALIZED:
              m_pimpl->m_simulator->registry()->set("system.state",
                                                    "UNINITIALIZED", initstate);
              break;
            case ExecutionManager::ExecutionState::RUNNING:
              m_pimpl->m_simulator->registry()->set("system.state", "RUNNING",
                                                    initstate);
              break;
            case ExecutionManager::ExecutionState::PAUSED:
              m_pimpl->m_simulator->registry()->set("system.state", "PAUSED",
                                                    initstate);
              break;
            }
            if (previous != ExecutionManager::ExecutionState::UNINITIALIZED &&
                state == ExecutionManager::ExecutionState::UNINITIALIZED) {
              m_pimpl->m_connection_manager->disconnectAll();
            }
            if (state == ExecutionManager::ExecutionState::UNINITIALIZED) {
              m_pimpl->m_simulator->reset();
              m_pimpl->m_triggers->reset();
              m_pimpl->m_scheduler->reset();
              m_pimpl->m_scripting_manager->reset();
              m_pimpl->m_logging_manager->reset();
              m_pimpl->m_connection_manager->reset();
              reset();
            } else if (previous ==
                       ExecutionManager::ExecutionState::UNINITIALIZED) {
              m_pimpl->m_logging_manager->initialize();
              m_pimpl->m_simulator->initialize();
              m_pimpl->m_scripting_manager->initialize();
              initialize();
            }
          });

  connect(&m_pimpl->m_connection_manager->filter(), &MessageFilter::log, this,
          [this](const QString &tag, const QString &text) {
            m_pimpl->m_logging_manager->log(tag, "<system>", text);
          });
  connect(m_pimpl->m_client_adapter, &OperatorAdapter::log,
          m_pimpl->m_logging_manager, &LoggingManager::log);
  connect(m_pimpl->m_simulator->registry(), &Registry::changed, this,
          [this](const QString &key, const QJsonValue &value,
                 const QString &who, bool skipLogging,
                 const QString &notifyTarget) {
            Q_UNUSED(notifyTarget)
            // Log registry updates
            auto jsondoc{QJsonDocument::fromVariant(value.toVariant())};
            if (!skipLogging) {
              QJsonObject data;
              data["key"] = key;
              data["value"] = value;
              m_pimpl->m_logging_manager->log(
                  "update", who,
                  QJsonDocument{data}.toJson(QJsonDocument::Compact));
            }
            // m_pimpl->m_connection_manager->broadcastUpdate(key, value);
            //  Loopback trigger
            QVariantMap data;
            data["key"] = key;
            data["value"] = value;
            data["sender"] = who;
            m_pimpl->m_triggers->processTriggers("<update>", data);
          });
  connect(
      m_pimpl->m_logging_manager, &LoggingManager::logged, this,
      [this](const QString &tag, const QString &sender, const QString &text) {
        QVariantMap logdata;
        logdata["tag"] = tag;
        logdata["sender"] = sender;
        logdata["text"] = text;
        m_pimpl->m_triggers->processTriggers("<log>", logdata);
      });
  connect(m_pimpl->m_scheduler, &Scheduler::evaluateRequest,
          m_pimpl->m_scripting_manager, &ScriptingManager::evaluate);
  connect(m_pimpl->m_triggers, &Triggers::evaluateRequest,
          m_pimpl->m_scripting_manager, &ScriptingManager::evaluate);
  connect(
      m_pimpl->m_connection_manager, &OperatorsManager::clientConnected, this,
      [this](UserCardPtr ptr) {
        QVariantMap cdata;
        cdata["user"] = ptr->username();
        cdata["alias"] = ptr->alias();
        cdata["type"] = static_cast<int>(ptr->access());
        if ((ptr->access() == AccessCard::AccessType::FULL_VIEW_AUTOSTART ||
             ptr->access() == AccessCard::AccessType::FULL_VIEW_AUTORESTART) &&
            m_pimpl->m_execution_manager->state() ==
                ExecutionManager::ExecutionState::UNINITIALIZED) {
          m_pimpl->m_execution_manager->resume();
        } else if (ptr->access() ==
                   AccessCard::AccessType::FULL_VIEW_AUTORESTART) {
          m_pimpl->m_execution_manager->pause();
          m_pimpl->m_simulator->reset();
          m_pimpl->m_triggers->reset();
          m_pimpl->m_scheduler->reset();
          m_pimpl->m_scripting_manager->reset();
          m_pimpl->m_logging_manager->reset();
          initialize();
          m_pimpl->m_simulator->initialize();
          m_pimpl->m_scripting_manager->initialize();
          m_pimpl->m_logging_manager->initialize();
          m_pimpl->m_execution_manager->initialize();
          m_pimpl->m_execution_manager->resume();
        }
        m_pimpl->m_triggers->processTriggers("<connection>", cdata);
      });
  connect(m_pimpl->m_connection_manager, &OperatorsManager::clientDisconnected,
          this, [this](UserCardPtr ptr) {
            QVariantMap cdata;
            cdata["user"] = ptr->username();
            cdata["alias"] = ptr->alias();
            cdata["type"] = static_cast<int>(ptr->access());
            m_pimpl->m_triggers->processTriggers("<disconnection>", cdata);
          });
  connect(m_pimpl->m_connection_manager, &OperatorsManager::aliasChanged, this,
          [this](UserCardPtr ptr) {
            QVariantMap cdata;
            cdata["user"] = ptr->username();
            cdata["alias"] = ptr->alias();
            cdata["type"] = static_cast<int>(ptr->access());
            m_pimpl->m_triggers->processTriggers("<aliaschanged>", cdata);
          });
}

Experiment::~Experiment() = default;

const QString &Experiment::name() const { return m_pimpl->m_name; }

void Experiment::name(const QString &name) {
  if (name != m_pimpl->m_name) {
    m_pimpl->m_name = name;
    emit contentChanged();
  }
}

const QString &Experiment::description() const {
  return m_pimpl->m_description;
}

void Experiment::description(const QString &desc) {
  if (desc != m_pimpl->m_description) {
    m_pimpl->m_description = desc;
    emit contentChanged();
  }
}

void Experiment::script(const QString &scriptname) {
  if (scriptname != m_pimpl->m_script) {
    m_pimpl->m_script = scriptname;
    emit contentChanged();
  }
}

const QString &Experiment::script() const { return m_pimpl->m_script; }

void Experiment::onContentChanged() { emit contentChanged(); }

void Experiment::cleanup() {
  for (const auto &l : m_pimpl->m_logging_manager->list()) {
    m_pimpl->m_logging_manager->discard(l);
  }
}

void Experiment::initialize() {
  if (!m_pimpl->m_script.isEmpty()) {
    QFileInfo fi(m_pimpl->m_script);
    if (fi.suffix() == "xml") {
      XMLConfigParser cparser{this, m_pimpl->m_scheduler,
                              m_pimpl->m_simulator->registry()};
      try {
        cparser.parse(m_pimpl->m_script);
      } catch (const XMLConfigParserException &e) {
        m_pimpl->m_logging_manager->critical(e.what());
        m_pimpl->m_execution_manager->reset();
        return;
      }
    } else {
      m_pimpl->m_scripting_manager->include("cams.js");
      m_pimpl->m_scripting_manager->include(m_pimpl->m_script);
    }
  } else {
    m_pimpl->m_scripting_manager->include("cams.js");
  }
}

void Experiment::reset() {}

void Experiment::load(const QJsonObject &json, bool ignore_name) {
  if (!ignore_name) {
    m_pimpl->m_name = json["name"].toString();
  }
  m_pimpl->m_enabled = json["enabled"].toBool();
  m_pimpl->m_description = json["description"].toString();
  m_pimpl->m_script = json["script"].toString();
  m_pimpl->m_connection_manager->load(json["network"].toObject());
  m_pimpl->m_credential_manager->load(json["users"].toObject());
  m_pimpl->m_execution_manager->load(json["execution"].toObject());
}

void Experiment::save(QJsonObject &json) const {
  json["name"] = m_pimpl->m_name;
  json["description"] = m_pimpl->m_description;
  QJsonObject ulist;
  m_pimpl->m_credential_manager->save(ulist);
  json["users"] = ulist;
  json["script"] = m_pimpl->m_script;
  QJsonObject unet;
  m_pimpl->m_connection_manager->save(unet);
  json["network"] = unet;
  QJsonObject execution;
  m_pimpl->m_execution_manager->save(execution);
  json["execution"] = execution;
  json["enabled"] = m_pimpl->m_enabled;
}

void Experiment::enabled(bool value) {
  if (value != m_pimpl->m_enabled) {
    if (m_pimpl->m_enabled && !value) {
      m_pimpl->m_execution_manager->reset();
      m_pimpl->m_connection_manager->disconnectAll();
    }
    m_pimpl->m_enabled = value;
    emit contentChanged();
    emit enabledStateChanged(value);
  }
}

bool Experiment::enabled() { return m_pimpl->m_enabled; }

Simulator *Experiment::simulator() const { return m_pimpl->m_simulator; }

Scheduler *Experiment::scheduler() const { return m_pimpl->m_scheduler; }

Triggers *Experiment::triggers() const { return m_pimpl->m_triggers; }

AccessManager *Experiment::credentialManager() const {
  return m_pimpl->m_credential_manager;
}

OperatorsManager *Experiment::operatorsManager() const {
  return m_pimpl->m_connection_manager;
}

ExecutionManager *Experiment::executionManager() const {
  return m_pimpl->m_execution_manager;
}

ScriptingManager *Experiment::scriptingManager() const {
  return m_pimpl->m_scripting_manager;
}

LoggingManager *Experiment::loggingManager() const {
  return m_pimpl->m_logging_manager;
}

ContentLibrary *Experiment::contentLibrary() const {
  return m_pimpl->m_content_library;
}

ExperimentServer *Experiment::experimentServer() const {
  return m_pimpl->m_communication_server.get();
}