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
#include "experimentadapter.h"

ExperimentAdapter::ExperimentAdapter(Experiment *exp) : QObject{exp} {}

QString ExperimentAdapter::name() const {
  auto exp{dynamic_cast<Experiment *>(parent())};
  return exp->name();
}

void ExperimentAdapter::name(const QString &name) {
  auto exp{dynamic_cast<Experiment *>(parent())};
  exp->name(name);
}

QString ExperimentAdapter::description() const {
  auto exp{dynamic_cast<Experiment *>(parent())};
  return exp->description();
}

void ExperimentAdapter::description(const QString &desc) {
  auto exp{dynamic_cast<Experiment *>(parent())};
  exp->description(desc);
}

void ExperimentAdapter::script(const QString &scriptname) {
  auto exp{dynamic_cast<Experiment *>(parent())};
  exp->script(scriptname);
}

QString ExperimentAdapter::script() const {
  auto exp{dynamic_cast<Experiment *>(parent())};
  return exp->script();
}

void ExperimentAdapter::initialize() {
  auto exp{dynamic_cast<Experiment *>(parent())};
  exp->initialize();
}
