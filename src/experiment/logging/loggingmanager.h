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
#ifndef LOGGINGMANAGER_H
#define LOGGINGMANAGER_H
#include "experiment/logging/datalogger.h"
#include <QObject>
#include <memory>

class Experiment;

class LoggingManager : public QObject, public ReadableDataLogger {
  Q_OBJECT
  Q_DISABLE_COPY(LoggingManager)
public:
  explicit LoggingManager(Experiment *exp);
  ~LoggingManager() override;

public slots:
  /* Logging and graphical notifications */
  void log(const QString &tag, const QString &sender,
           const QString &text = "") const;
  void timerInterval(int ms);
  void critical(const QString &message) const;
  void information(const QString &message) const;
  void warning(const QString &message) const;

  QSet<LogRecord> list() const override;
  QString fetch(const LogRecord &logRecordKey) const override;
  void discard(const LogRecord &logRecordKey) const override;

  void initialize();
  void reset();

signals:
  void logged(const QString &tag, const QString &sender,
              const QString &text = "") const;

private:
  struct pimpl;
  std::unique_ptr<pimpl> m_pimpl;
};

#endif // LOGGINGMANAGER_H
