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
#include "xmlconfigparser.h"
#include "xmlconfighandler.h"
#include <QJsonDocument>

struct XMLConfigParser::pimpl {
  Experiment *m_experiment{nullptr};
  Scheduler *m_scheduler{nullptr};
  Registry *m_registry{nullptr};
};

XMLConfigParser::XMLConfigParser(Experiment *e, Scheduler *s, Registry *r)
    : m_pimpl{std::make_unique<pimpl>()} {
  m_pimpl->m_experiment = e;
  m_pimpl->m_scheduler = s;
  m_pimpl->m_registry = r;
}

XMLConfigParser::~XMLConfigParser() = default;

void XMLConfigParser::parse(const QString &filename, bool validationMode) {
  XMLConfigHandler handler;
  if (!validationMode) {
    connect(&handler, &XMLConfigHandler::scriptElementRead, this,
            &XMLConfigParser::handleScriptElement);
    connect(&handler, &XMLConfigHandler::questionElementRead, this,
            &XMLConfigParser::handleQuestionElement);
    connect(&handler, &XMLConfigHandler::durationElementRead, this,
            &XMLConfigParser::handleDurationElement);
    connect(&handler, &XMLConfigHandler::assistanceElementRead, this,
            &XMLConfigParser::handleAssistanceElement);
    connect(&handler, &XMLConfigHandler::interfaceElementRead, this,
            &XMLConfigParser::handleInterfaceElement);
    connect(&handler, &XMLConfigHandler::controlElementRead, this,
            &XMLConfigParser::handleControlElement);
    connect(&handler, &XMLConfigHandler::adaptiveElementRead, this,
            &XMLConfigParser::handleAdaptiveElement);
    connect(&handler, &XMLConfigHandler::errorElementRead, this,
            &XMLConfigParser::handleErrorElement);
    connect(&handler, &XMLConfigHandler::codeElementRead, this,
            &XMLConfigParser::handleCodeElement);
    connect(&handler, &XMLConfigHandler::filterElementRead, this,
            &XMLConfigParser::handleFilterElement);
    connect(&handler, &XMLConfigHandler::messageElementRead, this,
            &XMLConfigParser::handleMessageElement);
    connect(&handler, &XMLConfigHandler::accessElementRead, this,
            &XMLConfigParser::handleAccessElement);
    connect(&handler, &XMLConfigHandler::popupElementRead, this,
            &XMLConfigParser::handlePopupElement);
  }
  QFile file{filename};
  if (!QFile::exists(filename)) {
    throw XMLConfigParserException{
        tr("Configuration parser: file %1 does not exist").arg(filename)};
  }
  if (!validationMode) {
    m_pimpl->m_experiment->scriptingManager()->include("cams.js");
  }
  if (file.open(QIODeviceBase::ReadOnly)) {
    try {
      QXmlStreamReader reader{&file};
      while (!reader.atEnd()) {
        auto el = reader.readNext();
        switch (el) {
        case QXmlStreamReader::StartElement: {
          auto name = reader.name();
          auto attr = reader.attributes();
          handler.startElement(name, attr);
        } break;
        case QXmlStreamReader::EndElement: {
          auto name = reader.name();
          handler.endElement(name);
        } break;
        case QXmlStreamReader::Characters: {
          auto text = reader.text();
          handler.characters(text);
        } break;
        default:
          break;
        }
      }
      if (reader.hasError()) {
        throw XMLConfigParserException{reader.errorString()};
      }
    } catch (const XMLConfigHandlerException &e) {
      throw XMLConfigParserException{e.what()};
    }
  }
}

void XMLConfigParser::parseString(const QString &config) {
  XMLConfigHandler handler;
  connect(&handler, &XMLConfigHandler::assistanceElementRead,
          [this](unsigned int startTime, const QJsonObject &parameters) {
            if (startTime == 0) {
              this->handleAssistanceElement(
                  static_cast<unsigned int>(
                      m_pimpl->m_experiment->executionManager()->timestamp() /
                      1000) +
                      1,
                  parameters);
            } else {
              this->handleAssistanceElement(startTime, parameters);
            }
          });
  connect(&handler, &XMLConfigHandler::interfaceElementRead,
          [this](unsigned int startTime, const QString &display,
                 const QString &modifier, const QString &value,
                 const QString &users) {
            if (startTime == 0) {
              this->handleInterfaceElement(
                  static_cast<unsigned int>(
                      m_pimpl->m_experiment->executionManager()->timestamp() /
                      1000) +
                      1,
                  display, modifier, value, users);
            } else {
              this->handleInterfaceElement(startTime, display, modifier, value,
                                           users);
            }
          });
  connect(&handler, &XMLConfigHandler::controlElementRead,
          [this](unsigned int startTime, const QString &cmd) {
            if (startTime == 0) {
              this->handleControlElement(
                  static_cast<unsigned int>(
                      m_pimpl->m_experiment->executionManager()->timestamp() /
                      1000) +
                      1,
                  cmd);
            } else {
              this->handleControlElement(startTime, cmd);
            }
          });
  connect(&handler, &XMLConfigHandler::adaptiveElementRead,
          [this](unsigned int startTime, const QJsonObject &parameters) {
            if (startTime == 0) {
              this->handleAdaptiveElement(
                  static_cast<unsigned int>(
                      m_pimpl->m_experiment->executionManager()->timestamp() /
                      1000) +
                      1,
                  parameters);
            } else {
              this->handleAdaptiveElement(startTime, parameters);
            }
          });
  connect(&handler, &XMLConfigHandler::errorElementRead,
          [this](unsigned int startTime, const QString &errorType,
                 const QJsonObject &parameters) {
            if (startTime == 0) {
              this->handleErrorElement(
                  static_cast<unsigned int>(
                      m_pimpl->m_experiment->executionManager()->timestamp() /
                      1000) +
                      1,
                  errorType, parameters);
            } else {
              this->handleErrorElement(startTime, errorType, parameters);
            }
          });
  connect(&handler, &XMLConfigHandler::codeElementRead,
          [this](unsigned int startTime, const QString &code) {
            if (startTime == 0) {
              this->handleCodeElement(
                  static_cast<unsigned int>(
                      m_pimpl->m_experiment->executionManager()->timestamp() /
                      1000) +
                      1,
                  code);
            } else {
              this->handleCodeElement(startTime, code);
            }
          });
  connect(&handler, &XMLConfigHandler::filterElementRead,
          [this](unsigned int startTime, bool filterEnabled,
                 const QString &mtype, const QString &key, const QString &user,
                 bool feedback) {
            if (startTime == 0) {
              this->handleFilterElement(
                  static_cast<unsigned int>(
                      m_pimpl->m_experiment->executionManager()->timestamp() /
                      1000) +
                      1,
                  filterEnabled, mtype, key, user, feedback);
            } else {
              this->handleFilterElement(startTime, filterEnabled, mtype, key,
                                        user, feedback);
            }
          });
  connect(&handler, &XMLConfigHandler::messageElementRead,
          [this](unsigned int startTime, const QString &sender,
                 const QString &recipient, const QString &text) {
            if (startTime == 0) {
              this->handleMessageElement(
                  static_cast<unsigned int>(
                      m_pimpl->m_experiment->executionManager()->timestamp() /
                      1000) +
                      1,
                  sender, recipient, text);
            } else {
              this->handleMessageElement(startTime, sender, recipient, text);
            }
          });
  connect(
      &handler, &XMLConfigHandler::accessElementRead,
      [this](unsigned int startTime, const QString &sender, int accessType) {
        if (startTime == 0) {
          this->handleAccessElement(
              static_cast<unsigned int>(
                  m_pimpl->m_experiment->executionManager()->timestamp() /
                  1000) +
                  1,
              sender, accessType);
        } else {
          this->handleAccessElement(startTime, sender, accessType);
        }
      });
  connect(&handler, &XMLConfigHandler::questionElementRead,
          [this](const QString &id, const QString &text) {
            this->handleQuestionElement(id, text);
          });
  try {
    QXmlStreamReader reader{config};
    while (!reader.atEnd()) {
      auto el = reader.readNext();
      switch (el) {
      case QXmlStreamReader::StartElement: {
        auto name = reader.name();
        auto attr = reader.attributes();
        handler.startElement(name, attr);
      } break;
      case QXmlStreamReader::EndElement: {
        auto name = reader.name();
        handler.endElement(name);
      } break;
      case QXmlStreamReader::Characters: {
        auto text = reader.text();
        handler.characters(text);
      } break;
      default:
        break;
      }
    }
    if (reader.hasError()) {
      throw XMLConfigParserException{reader.errorString()};
    }
  } catch (const XMLConfigHandlerException &e) {
    throw XMLConfigParserException{e.what()};
  }
}

void XMLConfigParser::validate(QIODevice &file) {
  file.open(QIODeviceBase::ReadOnly);
  XMLConfigHandler handler;
  try {
    QXmlStreamReader reader{&file};
    while (!reader.atEnd()) {
      auto el = reader.readNext();
      switch (el) {
      case QXmlStreamReader::StartElement: {
        auto name = reader.name();
        auto attr = reader.attributes();
        handler.startElement(name, attr);
      } break;
      case QXmlStreamReader::EndElement: {
        auto name = reader.name();
        handler.endElement(name);
      } break;
      case QXmlStreamReader::Characters: {
        auto text = reader.text();
        handler.characters(text);
      } break;
      default:
        break;
      }
    }
    if (reader.hasError()) {
      throw XMLConfigParserException{reader.errorString()};
    }
  } catch (const XMLConfigHandlerException &e) {
    throw XMLConfigParserException{e.what()};
  }
}

void XMLConfigParser::handleScriptElement(const QJsonObject &parameters) {
  m_pimpl->m_registry->set("adaptive.enabled", parameters["adaptive"].toBool());
  m_pimpl->m_registry->set("adaptive.type",
                           parameters["adaptiveControl"].toString());
  m_pimpl->m_registry->set("adaptive.adaptivecontrolmaxchange",
                           parameters["adaptiveControlMaxChange"].toInt());
  m_pimpl->m_registry->set("adaptive.reactionbaselinetransmission",
                           parameters["reactionbaselinetransmission"].toInt());
  m_pimpl->m_registry->set(
      "adaptive.reactionbaselinetransmissionduringfault",
      parameters["reactionbaselinetransmissionDuringFault"].toInt());
  m_pimpl->m_registry->set("adaptive.reactionbaselinelogging",
                           parameters["reactionbaselinelogging"].toInt());
  m_pimpl->m_registry->set(
      "adaptive.reactionbaselineloggingduringfault",
      parameters["reactionbaselineloggingduringfault"].toInt());
  m_pimpl->m_registry->set(
      "adaptive.reactiontimeloggingpredefinedintervals_a",
      parameters["reactiontimeloggingpredefinedintervals_a"].toInt());
  m_pimpl->m_registry->set(
      "adaptive.reactiontimeloggingpredefinedintervals_b",
      parameters["reactiontimeloggingpredefinedintervals_b"].toInt());
  m_pimpl->m_registry->set(
      "adaptive.reactiontimeloggingpredefinedintervals_c",
      parameters["reactiontimeloggingpredefinedintervals_c"].toInt());
  m_pimpl->m_registry->set(
      "adaptive.reactiontimeloggingpredefinedintervals_d",
      parameters["reactiontimeloggingpredefinedintervals_d"].toInt());
  m_pimpl->m_registry->set(
      "adaptive.reactiontimeloggingpredefinedintervals_e",
      parameters["reactiontimeloggingpredefinedintervals_e"].toInt());
  m_pimpl->m_registry->set(
      "adaptive.reactiontimetransmissionpredefinedintervals_a",
      parameters["reactiontimetransmissionpredefinedintervals_a"].toInt());
  m_pimpl->m_registry->set(
      "adaptive.reactiontimetransmissionpredefinedintervals_b",
      parameters["reactiontimetransmissionpredefinedintervals_b"].toInt());
  m_pimpl->m_registry->set(
      "adaptive.reactiontimetransmissionpredefinedintervals_c",
      parameters["reactiontimetransmissionpredefinedintervals_c"].toInt());
  m_pimpl->m_registry->set(
      "adaptive.reactiontimetransmissionpredefinedintervals_d",
      parameters["reactiontimetransmissionpredefinedintervals_d"].toInt());
  m_pimpl->m_registry->set(
      "adaptive.reactiontimetransmissionpredefinedintervals_e",
      parameters["reactiontimetransmissionpredefinedintervals_e"].toInt());
}

void XMLConfigParser::handleQuestionElement(const QString &id,
                                            const QString &text) {
    if (id.toLower() != "heading" && id.toLower() != "headingok") {
        m_pimpl->m_registry->set("automation.survey.question." + id, text, true);
    } else {
        m_pimpl->m_registry->set("automation.survey.question." + id, text);
    }
}

void XMLConfigParser::handleDurationElement(unsigned int endTime) {
    m_pimpl->m_scheduler->schedule(endTime, "execution.stop()");
}

void XMLConfigParser::handleAssistanceElement(unsigned int startTime,
                                              const QJsonObject &parameters) {
  QJsonDocument doc{parameters};
  QString jsonData{doc.toJson(QJsonDocument::Compact)};
  m_pimpl->m_scheduler->schedule(startTime,
                                 "assistanceChange(_ctx, " + jsonData + ")");
}

void XMLConfigParser::handleInterfaceElement(unsigned int startTime,
                                             const QString &display,
                                             const QString &modifier,
                                             const QString &value,
                                             const QString &users) {
  // TODO: Add support for "per user" changes (using explicit update send and
  // patterns)
  // TODO: Add support function in cams to avoid clutter
  if (users.isEmpty()) {
    m_pimpl->m_scheduler->schedule(
        startTime, "registry.set(\"interface." + display.toLower() + "." +
                       modifier.toLower() + "\", " + value.toLower() + ")");
  } else {
    m_pimpl->m_scheduler->schedule(
        startTime, "sendUserUpdate(\"interface." + display.toLower() + "." +
                       modifier.toLower() + "\", " + value.toLower() + ", \"" +
                       users + "\")");
  }
}

void XMLConfigParser::handleControlElement(unsigned int startTime,
                                           const QString &cmd) {
  m_pimpl->m_scheduler->schedule(startTime, cmd);
}

void XMLConfigParser::handleAdaptiveElement(unsigned int startTime,
                                            const QJsonObject &parameters) {
  QJsonDocument doc{parameters};
  QString jsonData{doc.toJson(QJsonDocument::Compact)};
  m_pimpl->m_scheduler->schedule(startTime,
                                 "adaptiveControl(_ctx, " + jsonData + ")");
}

void XMLConfigParser::handleErrorElement(unsigned int startTime,
                                         const QString &errorType,
                                         const QJsonObject &parameters) {
  QJsonDocument doc{parameters};
  QString jsonData{doc.toJson(QJsonDocument::Compact)};
  m_pimpl->m_scheduler->schedule(
      startTime, "new " + errorType + "().activate(_ctx, " + jsonData + ")");
}

void XMLConfigParser::handleCodeElement(unsigned int startTime,
                                        const QString &code) {
  m_pimpl->m_scheduler->schedule(startTime, code);
}

void XMLConfigParser::handleFilterElement(unsigned int startTime,
                                          bool filterEnabled,
                                          const QString &mtype,
                                          const QString &key,
                                          const QString &user, bool feedback) {
  if (mtype == "server_update") {
    if (filterEnabled) {
      m_pimpl->m_scheduler->schedule(startTime,
                                     "filter.blockOutgoingUpdate(\"" + user +
                                         "\", \"" + key + "\")");
    } else {
      m_pimpl->m_scheduler->schedule(startTime,
                                     "filter.unblockOutgoingUpdate(\"" + user +
                                         "\", \"" + key + "\")");
    }
  } else if (mtype == "client_update") {
    if (filterEnabled) {
      m_pimpl->m_scheduler->schedule(
          startTime, "filter.blockIncomingUpdate(\"" + user + "\", \"" + key +
                         "\", " + (feedback ? "true" : "false") + ")");
    } else {
      m_pimpl->m_scheduler->schedule(startTime,
                                     "filter.unblockIncomingUpdate(\"" + user +
                                         "\", \"" + key + "\")");
    }
  } else if (mtype == "server_notify") {
    if (filterEnabled) {
      m_pimpl->m_scheduler->schedule(startTime,
                                     "filter.blockOutgoingNotify(\"" + user +
                                         "\", \"" + key + "\")");
    } else {
      m_pimpl->m_scheduler->schedule(startTime,
                                     "filter.unblockOutgoingNotify(\"" + user +
                                         "\", \"" + key + "\")");
    }
  } else if (mtype == "client_trigger") {
    if (filterEnabled) {
      m_pimpl->m_scheduler->schedule(startTime,
                                     "filter.blockIncomingTrigger(\"" + user +
                                         "\", \"" + key + "\")");
    } else {
      m_pimpl->m_scheduler->schedule(startTime,
                                     "filter.unblockIncomingTrigger(\"" + user +
                                         "\", \"" + key + "\")");
    }
  } else {
    throw XMLConfigParserException{"Invalid message filter type"};
  }
}

void XMLConfigParser::handleAccessElement(unsigned int startTime,
                                          const QString &username,
                                          int accessType) {
  m_pimpl->m_scheduler->schedule(startTime,
                                 "connection.access(\"" + username + "\", " +
                                     QString::number(accessType) + ")");
}

void XMLConfigParser::handleMessageElement(unsigned int startTime,
                                           const QString &sender,
                                           const QString &recipient,
                                           const QString &message) {
  m_pimpl->m_scheduler->schedule(startTime, "sendChatMessage(\"" + sender +
                                                "\", \"" + message + "\", \"" +
                                                recipient + "\")");
}

void XMLConfigParser::handlePopupElement(unsigned int startTime,
                                         const QJsonObject &parameters) {
  bool showClose{false};
  int width{-1};
  int height{-1};

  if (parameters.contains("url") && parameters.contains("title")) {
      auto popuptitle{parameters["title"].toString()};
      auto popupurl{parameters["url"].toString()};
      m_pimpl->m_scheduler->schedule(startTime,
                                     "displayPopup(\"" + popupurl.toHtmlEscaped() + "\",\""
                                         + popuptitle.toHtmlEscaped() + "\")");
  }
}
