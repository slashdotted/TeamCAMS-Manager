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
#include "scheduleradapter.h"

SchedulerAdapter::SchedulerAdapter(Scheduler *s) : QObject{s} {}

QString SchedulerAdapter::schedule(unsigned int timestamp, const QString &code,
                                   const QString &reference,
                                   const QString &type) {
  auto s{static_cast<Scheduler *>(parent())};
  return s->schedule(timestamp, code, reference, type);
}

QString SchedulerAdapter::schedule(unsigned int timestamp,
                                   unsigned int interval, const QString &code,
                                   const QString &reference,
                                   const QString &type) {
  auto s{static_cast<Scheduler *>(parent())};
  return s->schedule(timestamp, interval, code, reference, type);
}

void SchedulerAdapter::unscheduleByUuid(const QString &event) {
  auto s{static_cast<Scheduler *>(parent())};
  s->unscheduleByUuid(event);
}

void SchedulerAdapter::unscheduleByType(const QString &type) {
  auto s{static_cast<Scheduler *>(parent())};
  s->unscheduleByType(type);
}

bool SchedulerAdapter::exists(const QString &event) const {
  auto s{static_cast<Scheduler *>(parent())};
  return s->exists(event);
}

bool SchedulerAdapter::postpone(unsigned int deltatimestamp,
                                const QString &event) {
  auto s{static_cast<Scheduler *>(parent())};
  return s->postpone(deltatimestamp, event);
}

void SchedulerAdapter::processEvents(unsigned int timestamp) {
  auto s{static_cast<Scheduler *>(parent())};
  return s->processEvents(timestamp);
}

void SchedulerAdapter::reset() {
  auto s{static_cast<Scheduler *>(parent())};
  return s->reset();
}
