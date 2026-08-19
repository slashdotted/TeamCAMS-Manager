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
#include "loggingmanager.h"
#include "consolelogger.h"
#include "experiment/experiment.h"
#include "filelogger.h"
#ifdef MANAGERGUI
#include "gui/asyncmessageboxfactory.h"
#include <QApplication>
#include <QMessageBox>
#endif

// TODO: Implement pluggable logging output
// TODO: Maybe we need access to registry to output status of the system (to
// implement a "special" logging system)

struct LoggingManager::pimpl {
  QList<WritableDataLogger *> m_loggers;
  Experiment *m_experiment;
};

LoggingManager::LoggingManager(Experiment *exp)
    : QObject{exp}, m_pimpl{std::make_unique<pimpl>()} {
  m_pimpl->m_experiment = exp;
#ifdef MANAGERGUI
  if (qgetenv("NO_FILE_LOGGER").isEmpty()) {
    m_pimpl->m_loggers.push_back(
        new FileLogger{exp, exp->simulator()->registry(), this});
  }
#else
  if (qgetenv("NO_CONSOLE_LOGGER").isEmpty()) {
    m_pimpl->m_loggers.push_back(
        new ConsoleLogger{exp, exp->simulator()->registry(), this});
  }
#endif
}

LoggingManager::~LoggingManager() {
  for (const auto &l : m_pimpl->m_loggers) {
    l->endSession();
  }
}

void LoggingManager::log(const QString &tag, const QString &sender,
                         const QString &text) const {
  for (const auto &l : m_pimpl->m_loggers) {
    l->log(m_pimpl->m_experiment->executionManager()->timestamp(), tag, sender,
           text);
  }
  emit logged(tag, sender, text);
}

void LoggingManager::timerInterval(int ms) {
  Q_UNUSED(ms)
  // TODO: Periodic parameter log
}

void LoggingManager::critical(const QString &message) const {
#ifdef MANAGERGUI
  AsyncMessageBoxFactory::critical(QApplication::activeWindow(), tr("Critical"),
                                   message, QMessageBox::Ok);
#else
  qError() << message;
#endif
}

void LoggingManager::information(const QString &message) const {
#ifdef MANAGERGUI
  AsyncMessageBoxFactory::information(QApplication::activeWindow(),
                                      tr("Information"), message,
                                      QMessageBox::Ok);
#else
  qInfo() << message;
#endif
}

void LoggingManager::warning(const QString &message) const {
#ifdef MANAGERGUI
  AsyncMessageBoxFactory::warning(QApplication::activeWindow(), tr("Alert"),
                                  message, QMessageBox::Ok);
#else
  qWarning() << message;
#endif
}

QSet<LogRecord> LoggingManager::list() const {
  QSet<LogRecord> s;
  for (const auto &l : m_pimpl->m_loggers) {
    auto rdl = dynamic_cast<ReadableDataLogger *>(l);
    if (rdl) {
      s.unite(rdl->list());
    }
  }
  return s;
}

QString LoggingManager::fetch(const LogRecord &logRecordKey) const {
  for (const auto &l : m_pimpl->m_loggers) {
    auto rdl = dynamic_cast<ReadableDataLogger *>(l);
    if (rdl) {
      auto data = rdl->fetch(logRecordKey);
      if (!data.isEmpty())
        return data;
    }
  }
  return QString{};
}

void LoggingManager::discard(const LogRecord &logRecordKey) const {
  for (const auto &l : m_pimpl->m_loggers) {
    auto rdl = dynamic_cast<ReadableDataLogger *>(l);
    if (rdl) {
      rdl->discard(logRecordKey);
    }
  }
}

void LoggingManager::initialize() {
  for (const auto &l : m_pimpl->m_loggers) {
    l->startSession(m_pimpl->m_experiment->name());
  }
}

void LoggingManager::reset() {
  for (const auto &l : m_pimpl->m_loggers) {
    l->endSession();
  }
}
