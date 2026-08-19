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
#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <QDateTime>
#include <QString>

struct LogRecord {
  QString key;
  QDateTime datetime;

  bool operator==(const LogRecord &other) const {
    return key == other.key && datetime == other.datetime;
  }
};

inline size_t qHash(const LogRecord &record, size_t seed = 0) noexcept {
  return qHash(record.key, seed) ^ qHash(record.datetime, seed);
}

class DataLogger {
public:
  virtual ~DataLogger() = default;
};

class WritableDataLogger : public virtual DataLogger {
public:
  virtual ~WritableDataLogger() = default;
  virtual void endSession() = 0;
  virtual void startSession(const QString &experimentName) = 0;
  virtual void log(double timestamp, const QString &tag, const QString &sender,
                   const QString &text) = 0;
};

class ReadableDataLogger : public virtual DataLogger {
public:
  virtual ~ReadableDataLogger() = default;
  virtual QSet<LogRecord> list() const = 0;
  virtual QString fetch(const LogRecord &logRecordKey) const = 0;
  virtual void discard(const LogRecord &logRecordKey) const = 0;
};

#endif // DATALOGGER_H
