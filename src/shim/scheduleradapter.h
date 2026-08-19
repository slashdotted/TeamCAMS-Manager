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
#ifndef SCHEDULERADAPTER_H
#define SCHEDULERADAPTER_H

#include "events/scheduler.h"
#include <QObject>
#include <QString>

class SchedulerAdapter : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(SchedulerAdapter)
public:
  explicit SchedulerAdapter(Scheduler *s);

public slots:
  /* Schedules a new event */
  QString schedule(unsigned int timestamp, const QString &code,
                   const QString &reference = "", const QString &type = "");
  /* Schedules a new recurring event */
  QString schedule(unsigned int timestamp, unsigned int interval,
                   const QString &code, const QString &reference = "",
                   const QString &type = "");
  /* Unschedules the event and all other events which refer to it */
  void unscheduleByUuid(const QString &event);
  void unscheduleByType(const QString &type);
  /* Check if the event exists */
  bool exists(const QString &event) const;
  bool postpone(unsigned int deltatimestamp, const QString &event);
  void processEvents(unsigned int timestamp);
  void reset();
};

#endif // SCHEDULERADAPTER_H
