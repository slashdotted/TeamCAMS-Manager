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
#include "triggers.h"
#include <QDebug>

// Sets triggers for user events/requests (used for repair triggers)
struct TriggerData {
  QUuid eid;
  QString type;
  QString pattern;
  QString code;
};

// TODO: dump / restore all triggers
struct Triggers::pimpl {
  QMultiMap<QString, TriggerData> m_triggers;
};

Triggers::Triggers(QObject *parent)
    : QObject{parent}, m_pimpl{std::make_unique<pimpl>()} {}

Triggers::~Triggers() = default;

QString Triggers::activate(const QString &msgPattern, const QString &code,
                           const QString &reference, const QString &type) {
  TriggerData td;
  td.pattern = msgPattern;
  td.code = code;
  td.type = type;
  if (reference == "") {
    td.eid = QUuid::createUuid();
  } else {
    td.eid = QUuid::fromString(reference);
  }
  m_pimpl->m_triggers.insert(msgPattern, td);
  return td.eid.toString();
}

void Triggers::deactivateByUuid(const QString &event) {
  QUuid eventUuid{event};
  for (auto it = m_pimpl->m_triggers.begin();
       it != m_pimpl->m_triggers.end();) {
    if (it.value().eid == eventUuid) {
      it = m_pimpl->m_triggers.erase(it);
    } else {
      ++it;
    }
  }
}

void Triggers::deactivateByType(const QString &type) {
  QList<QString> staleUuids;
  for (auto it = m_pimpl->m_triggers.begin();
       it != m_pimpl->m_triggers.end();) {
    if (it.value().type == type) {
      staleUuids.append(it->eid.toString());
      it = m_pimpl->m_triggers.erase(it);
    } else {
      ++it;
    }
  }
  for (const auto &u : staleUuids) {
    deactivateByUuid(u);
  }
}

bool Triggers::exists(const QString &event) const {
  QUuid eventUuid{event};
  for (auto it = m_pimpl->m_triggers.begin(); it != m_pimpl->m_triggers.end();
       ++it) {
    if (it.value().eid == eventUuid) {
      return true;
    }
  }
  return false;
}

void Triggers::processTriggers(const QString &trigger, QVariantMap &bindings) {
  if (m_pimpl->m_triggers.contains(trigger)) {
    QList<TriggerData> values = m_pimpl->m_triggers.values(trigger);
    for (const auto &td : values) {
      bindings["_eventid"] = td.eid.toString();
      bindings["_trigger"] = trigger;
      emit evaluateRequest(td.code, bindings);
    }
  }
}

void Triggers::processTriggers(const QString &trigger) {
  QVariantMap map;
  processTriggers(trigger, map);
}

void Triggers::reset() { m_pimpl->m_triggers.clear(); }
