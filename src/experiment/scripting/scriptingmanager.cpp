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
#include "scriptingmanager.h"
#include "experiment/experiment.h"
#include <QDir>
#include <QFileInfo>
#include <QJSEngine>
#include <QQmlEngine>
#ifdef MANAGERGUI
#include "gui/asyncmessageboxfactory.h"
#include <QApplication>
#include <QMessageBox>
#endif
#include "shim/connectionadapter.h"
#include "shim/executionadapter.h"
#include "shim/loggingadapter.h"
#include "shim/messagefilteradapter.h"
#include "shim/registryadapter.h"
#include "shim/scheduleradapter.h"
#include "shim/scriptingadapter.h"
#include "shim/simulatoradapter.h"
#include "shim/triggersadapter.h"

struct ScriptingManager::pimpl {
  bool m_errorcondition{false};
  Experiment *m_experiment{nullptr};
  QJSEngine m_engine;
  ConnectionAdapter *m_connection{nullptr};
  ExecutionAdapter *m_execution{nullptr};
  LoggingAdapter *m_logging{nullptr};
  RegistryAdapter *m_registry{nullptr};
  SchedulerAdapter *m_scheduler{nullptr};
  TriggersAdapter *m_triggers{nullptr};
  ScriptingAdapter *m_scripting{nullptr};
  MessageFilterAdapter *m_filter{nullptr};
  SimulatorAdapter *m_simulator{nullptr};
};

ScriptingManager::ScriptingManager(Experiment *experiment)
    : QObject{experiment}, m_pimpl{std::make_unique<pimpl>()} {
  m_pimpl->m_experiment = experiment;
  m_pimpl->m_connection = new ConnectionAdapter{
      experiment->operatorsManager(), experiment->credentialManager()};
  m_pimpl->m_execution = new ExecutionAdapter{experiment->executionManager()};
  m_pimpl->m_logging = new LoggingAdapter{experiment->loggingManager()};
  m_pimpl->m_registry =
      new RegistryAdapter{experiment->simulator()->registry()};
  m_pimpl->m_scheduler = new SchedulerAdapter{experiment->scheduler()};
  m_pimpl->m_triggers = new TriggersAdapter{experiment->triggers()};
  m_pimpl->m_scripting = new ScriptingAdapter{this};
  m_pimpl->m_filter =
      new MessageFilterAdapter{&experiment->operatorsManager()->filter()};
  m_pimpl->m_simulator = new SimulatorAdapter{experiment->simulator()};

  m_pimpl->m_engine.installExtensions(QJSEngine::AllExtensions);

  m_pimpl->m_engine.globalObject().setProperty(
      "connection", m_pimpl->m_engine.newQObject(m_pimpl->m_connection));
  m_pimpl->m_engine.globalObject().setProperty(
      "execution", m_pimpl->m_engine.newQObject(m_pimpl->m_execution));
  m_pimpl->m_engine.globalObject().setProperty(
      "logging", m_pimpl->m_engine.newQObject(m_pimpl->m_logging));
  m_pimpl->m_engine.globalObject().setProperty(
      "registry", m_pimpl->m_engine.newQObject(m_pimpl->m_registry));
  m_pimpl->m_engine.globalObject().setProperty(
      "scheduler", m_pimpl->m_engine.newQObject(m_pimpl->m_scheduler));
  m_pimpl->m_engine.globalObject().setProperty(
      "triggers", m_pimpl->m_engine.newQObject(m_pimpl->m_triggers));
  m_pimpl->m_engine.globalObject().setProperty(
      "scripting", m_pimpl->m_engine.newQObject(m_pimpl->m_scripting));
  m_pimpl->m_engine.globalObject().setProperty(
      "filter", m_pimpl->m_engine.newQObject(m_pimpl->m_filter));
  m_pimpl->m_engine.globalObject().setProperty(
      "simulator", m_pimpl->m_engine.newQObject(m_pimpl->m_simulator));

  QQmlEngine::setObjectOwnership(m_pimpl->m_connection,
                                 QQmlEngine::CppOwnership);
  QQmlEngine::setObjectOwnership(m_pimpl->m_execution,
                                 QQmlEngine::CppOwnership);
  QQmlEngine::setObjectOwnership(m_pimpl->m_logging, QQmlEngine::CppOwnership);
  QQmlEngine::setObjectOwnership(m_pimpl->m_registry, QQmlEngine::CppOwnership);
  QQmlEngine::setObjectOwnership(m_pimpl->m_scheduler,
                                 QQmlEngine::CppOwnership);
  QQmlEngine::setObjectOwnership(m_pimpl->m_triggers, QQmlEngine::CppOwnership);
  QQmlEngine::setObjectOwnership(m_pimpl->m_scripting,
                                 QQmlEngine::CppOwnership);
  QQmlEngine::setObjectOwnership(m_pimpl->m_filter, QQmlEngine::CppOwnership);
  QQmlEngine::setObjectOwnership(m_pimpl->m_simulator,
                                 QQmlEngine::CppOwnership);
}

ScriptingManager::~ScriptingManager() = default;

void ScriptingManager::evaluate(const QString &code, QVariantMap bindings) {
  if (m_pimpl->m_errorcondition)
    return;
  QJSValue sandbox{m_pimpl->m_engine.evaluate(
      QString{"(function(_ctx) { %1 });"}.arg(code))};
  bindings["_timestamp"] = m_pimpl->m_execution->timestamp() / 1000.0;
  bindings["_experiment"] = m_pimpl->m_experiment->name();
  QJSValue result{
      sandbox.call(QJSValueList{m_pimpl->m_engine.toScriptValue(bindings)})};
  if (result.isError()) {
    int line = result.property("lineNumber").toInt();
    m_pimpl->m_logging->log("script:exception", "<system>",
                            result.toString() + " (line " +
                                QString::number(line) + ")");
    m_pimpl->m_execution->pause();
    m_pimpl->m_errorcondition = true;
#ifdef MANAGERGUI
    AsyncMessageBoxFactory::critical(
        QApplication::activeWindow(), tr("Runtime Exception"),
        tr("<b>Runtime error in script</b><br><br>%1 -> %2")
            .arg(code,
                 result.toString() + " (line " + QString::number(line) + ")"),
        QMessageBox::Ok);
#else
    qError() << QString{"Runtime error in script: %1 -> %2"}.arg(
        code, result.toString() + " (line " + QString::number(line) + ")");
#endif
    return;
  }
}

void ScriptingManager::include(const QString &file) {
  if (m_pimpl->m_errorcondition)
    return;
  QFileInfo finfo{file};
  QString fname;
  if (finfo.isRelative()) {
    QDir dinfo{m_pimpl->m_experiment->script()};
    fname = dinfo.absoluteFilePath(file);
    QFile fcheck{fname};
    if (!fcheck.exists()) {
      fname = ":/scripts/" + file;
    }
  } else {
    fname = file;
  }
  QFile script{fname};
  if (!script.open(QIODevice::ReadOnly)) {
    m_pimpl->m_execution->pause();
    m_pimpl->m_errorcondition = true;
#ifdef MANAGERGUI
    AsyncMessageBoxFactory::critical(
        QApplication::activeWindow(), tr("Runtime Exception"),
        tr("<b>Cannot open script error in script</b><br><br>%1").arg(fname),
        QMessageBox::Ok);
#endif
    return;
  }
  QTextStream tstream(&script);
  QString code = tstream.readAll();
  script.close();
  QJSValue result{m_pimpl->m_engine.evaluate(code)};
  if (result.isError()) {
    int line = result.property("lineNumber").toInt();
    m_pimpl->m_logging->log("script:exception", "<system>",
                            result.toString() + " (line " +
                                QString::number(line) + ")");
    m_pimpl->m_execution->pause();
    m_pimpl->m_errorcondition = true;
#ifdef MANAGERGUI
    AsyncMessageBoxFactory::critical(
        QApplication::activeWindow(), tr("Runtime Exception"),
        tr("<b>Runtime error in script</b><br><br>%1 -> %2")
            .arg(code,
                 result.toString() + " (line " + QString::number(line) + ")"),
        QMessageBox::Ok);
#else
    qError() << result.property("stack").toString();
#endif
    return;
  }
}

bool ScriptingManager::hasErrorCondition() const {
  return m_pimpl->m_errorcondition;
}

void ScriptingManager::initialize() {}

void ScriptingManager::reset() { m_pimpl->m_errorcondition = false; }
