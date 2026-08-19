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
#include "registryadapter.h"
#include <QDebug>

RegistryAdapter::RegistryAdapter(Registry *reg) : QObject{reg} {}

void RegistryAdapter::set(const QString &name, const QJsonValue &value,
                          bool initialize) {
  auto reg{dynamic_cast<Registry *>(parent())};
  reg->set(name, value, initialize);
}

QJsonValue RegistryAdapter::get(const QString &name, bool ignoreUndefined) {
  auto reg{dynamic_cast<Registry *>(parent())};
  return reg->get(name, ignoreUndefined);
}

void RegistryAdapter::stick(const QString &name, const QJsonValue &value) {
  auto reg{dynamic_cast<Registry *>(parent())};
  reg->stick(name, value);
}

void RegistryAdapter::stick(const QString &name) {
  auto reg{dynamic_cast<Registry *>(parent())};
  reg->stick(name);
}

void RegistryAdapter::setAndStick(const QString &name,
                                  const QJsonValue &value) {
  auto reg{dynamic_cast<Registry *>(parent())};
  reg->setAndStick(name, value);
}

void RegistryAdapter::unstick(const QString &name) {
  auto reg{dynamic_cast<Registry *>(parent())};
  reg->unstick(name);
}

bool RegistryAdapter::isStuck(const QString &name) {
  auto reg{dynamic_cast<Registry *>(parent())};
  return reg->isStuck(name);
}

void RegistryAdapter::dump() {
  auto reg{dynamic_cast<Registry *>(parent())};
  reg->dump();
}

bool RegistryAdapter::exists(const QString &name) {
  auto reg{dynamic_cast<Registry *>(parent())};
  return reg->exists(name);
}

void RegistryAdapter::touch(const QString &name) {
  auto reg{dynamic_cast<Registry *>(parent())};
  reg->touch(name);
}

void RegistryAdapter::reset() {
  auto reg{dynamic_cast<Registry *>(parent())};
  reg->reset();
}

QStringList RegistryAdapter::stuckProperties() {
  auto reg{dynamic_cast<Registry *>(parent())};
  return reg->stuckProperties();
}
