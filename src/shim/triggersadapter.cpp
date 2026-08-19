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
#include "triggersadapter.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

TriggersAdapter::TriggersAdapter(Triggers *triggers) : QObject{triggers} {}

QString TriggersAdapter::activate(const QString &msgPattern,
                                  const QString &code, const QString &reference,
                                  const QString &type) {
  auto t{dynamic_cast<Triggers *>(parent())};
  return t->activate(msgPattern, code, reference, type);
}

void TriggersAdapter::deactivateByUuid(const QString &event) {
  auto t{dynamic_cast<Triggers *>(parent())};
  t->deactivateByUuid(event);
}

void TriggersAdapter::deactivateByType(const QString &type) {
  auto t{dynamic_cast<Triggers *>(parent())};
  t->deactivateByType(type);
}

bool TriggersAdapter::exists(const QString &event) const {
  auto t{dynamic_cast<Triggers *>(parent())};
  return t->exists(event);
}

void TriggersAdapter::processTriggersWithBindings(const QString &trigger,
                                                  const QString &bindings) {
  QJsonDocument jsonData{QJsonDocument::fromJson(bindings.toUtf8())};
  if (!jsonData.isObject()) {
    qWarning("Bindings string does not contain a valid object (use "
             "processTriggers if bindings aren't needed)");
  }
  QJsonObject bindingsObject{jsonData.object()};
  QVariantMap bindingsMap{bindingsObject.toVariantMap()};
  auto t{dynamic_cast<Triggers *>(parent())};
  t->processTriggers(trigger, bindingsMap);
}

void TriggersAdapter::processTriggers(const QString &trigger) {
  auto t{dynamic_cast<Triggers *>(parent())};
  t->processTriggers(trigger);
}

void TriggersAdapter::reset() {
  auto t{dynamic_cast<Triggers *>(parent())};
  t->reset();
}
