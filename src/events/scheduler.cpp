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
#include "scheduler.h"
#include <QDebug>
#include <QList>
#include <QUuid>
#include <QVariantMap>
#include <algorithm>

// TODO: maybe have methods to dump / restore all scheduled events?
struct EventData {
  static long long sequence;
  long long index{0};
  QUuid eid;
  QString type;
  unsigned int timestamp{0};
  unsigned int interval{0};
  QString code;
};

long long EventData::sequence{0};

struct Scheduler::pimpl {
  void sort();
  QList<EventData *> m_events;
  EventData *m_current_event{nullptr};
};

Scheduler::Scheduler(QObject *parent)
    : QObject{parent}, m_pimpl{std::make_unique<pimpl>()} {}

Scheduler::~Scheduler() {
  for (auto e : m_pimpl->m_events) {
    delete e;
  }
}

QString Scheduler::schedule(unsigned int timestamp, const QString &code,
                            const QString &reference, const QString &type) {
  EventData *e{new EventData};
  e->index = ++EventData::sequence;
  e->timestamp = timestamp;
  e->type = type;
  e->code = code;
  e->interval = 0;
  if (reference == "") {
    e->eid = QUuid::createUuid();
  } else {
    e->eid = QUuid::fromString(reference);
  }
  m_pimpl->m_events.push_back(e);
  m_pimpl->sort();
  return e->eid.toString();
}

QString Scheduler::schedule(unsigned int timestamp, unsigned int interval,
                            const QString &code, const QString &reference,
                            const QString &type) {
  EventData *e{new EventData};
  e->index = ++EventData::sequence;
  e->timestamp = timestamp;
  e->type = type;
  e->code = code;
  e->interval = interval;
  if (reference == "") {
    e->eid = QUuid::createUuid();
  } else {
    e->eid = QUuid::fromString(reference);
  }
  m_pimpl->m_events.push_back(e);
  m_pimpl->sort();
  return e->eid.toString();
}

void Scheduler::unscheduleByUuid(const QString &event) {
  QUuid eventUuid{event};
  if (m_pimpl->m_current_event != nullptr &&
      m_pimpl->m_current_event->eid == eventUuid) {
    delete m_pimpl->m_current_event;
    m_pimpl->m_current_event = nullptr;
  }
  auto nend = std::remove_if(m_pimpl->m_events.begin(), m_pimpl->m_events.end(),
                             [&](const EventData *d) {
                               if (d->eid == eventUuid) {
                                 delete d;
                                 return true;
                               }
                               return false;
                             });
  m_pimpl->m_events.erase(nend, m_pimpl->m_events.end());
}

void Scheduler::unscheduleByType(const QString &type) {
  QStringList toBeUnscheduled;
  if (m_pimpl->m_current_event != nullptr &&
      m_pimpl->m_current_event->type == type) {
    delete m_pimpl->m_current_event;
    m_pimpl->m_current_event = nullptr;
  }
  std::for_each(m_pimpl->m_events.begin(), m_pimpl->m_events.end(),
                [&](const EventData *d) {
                  if (d->type == type) {
                    toBeUnscheduled.push_back(d->eid.toString());
                  }
                });
  for (const auto &eid : toBeUnscheduled) {
    unscheduleByUuid(eid);
  }
}

bool Scheduler::exists(const QString &event) const {
  QUuid eventUuid{event};
  if (m_pimpl->m_current_event != nullptr &&
      m_pimpl->m_current_event->eid == eventUuid) {
    return true;
  }
  return std::find_if(m_pimpl->m_events.begin(), m_pimpl->m_events.end(),
                      [&](const EventData *d) {
                        return d->eid == eventUuid;
                      }) != m_pimpl->m_events.end();
}

bool Scheduler::postpone(unsigned int deltatimestamp, const QString &event) {
  QUuid eventUuid{event};
  if (m_pimpl->m_current_event != nullptr &&
      m_pimpl->m_current_event->eid == eventUuid) {
    m_pimpl->m_current_event->timestamp += deltatimestamp;
    return true;
  }
  auto v =
      std::find_if(m_pimpl->m_events.begin(), m_pimpl->m_events.end(),
                   [&](const EventData *d) { return d->eid == eventUuid; });
  if (v != m_pimpl->m_events.end()) {
    Q_ASSERT(event == (*v)->eid.toString());
    (*v)->timestamp += deltatimestamp;
    return true;
  }
  return false;
}

void Scheduler::processEvents(unsigned int timestamp) {
  for (;;) {
    if (m_pimpl->m_events.empty())
      break;
    if (m_pimpl->m_events.first()->timestamp <= timestamp) {
      m_pimpl->m_current_event = m_pimpl->m_events.first();
      m_pimpl->m_events.pop_front();
      QVariantMap bindings;
      bindings["_eventid"] = m_pimpl->m_current_event->eid.toString();
      bindings["_sender"] = "<system>";
      bindings["_alias"] = "<system>";
      emit evaluateRequest(m_pimpl->m_current_event->code, bindings);
      if (m_pimpl->m_current_event != nullptr) {
        if (m_pimpl->m_current_event->interval > 0) {
          m_pimpl->m_current_event->timestamp =
              timestamp + m_pimpl->m_current_event->interval;
          m_pimpl->m_events.push_back(m_pimpl->m_current_event);
          m_pimpl->sort();
        } else if (m_pimpl->m_current_event->timestamp > timestamp) {
          m_pimpl->m_events.push_back(m_pimpl->m_current_event);
          m_pimpl->sort();
        }
        m_pimpl->m_current_event = nullptr;
      }
    } else {
      break;
    }
  }
}

void Scheduler::reset() {
  m_pimpl->m_events.clear();
  m_pimpl->m_current_event = nullptr;
}

void Scheduler::pimpl::sort() {
  std::sort(m_events.begin(), m_events.end(),
            [&](const EventData *lhs, const EventData *rhs) {
              if (lhs->timestamp < rhs->timestamp) {
                return true;
              }
              if (lhs->timestamp == rhs->timestamp) {
                return lhs->index < rhs->index;
              }
              return false;
            });
}
