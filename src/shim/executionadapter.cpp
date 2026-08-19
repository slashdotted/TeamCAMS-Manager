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
#include "executionadapter.h"
#include <QDebug>
#include "experiment/experiment.h"

ExecutionAdapter::ExecutionAdapter(ExecutionManager *em) : QObject{em} {}

void ExecutionAdapter::toggle() {
  auto em{dynamic_cast<ExecutionManager *>(parent())};
  em->toggle();
}

void ExecutionAdapter::resume() {
  auto em{dynamic_cast<ExecutionManager *>(parent())};
  em->resume();
}

void ExecutionAdapter::pause() {
  auto em{dynamic_cast<ExecutionManager *>(parent())};
  em->pause();
}

void ExecutionAdapter::stop()
{
    auto em{dynamic_cast<ExecutionManager *>(parent())};
    auto exp{dynamic_cast<Experiment *>(em->parent())};
    if (em && exp) {
        exp->operatorsManager()->disconnectAll();
        em->reset();
    }
}

void ExecutionAdapter::step() {
  auto em{dynamic_cast<ExecutionManager *>(parent())};
  em->step();
}

double ExecutionAdapter::timestamp() const {
  auto em{dynamic_cast<ExecutionManager *>(parent())};
  return em->timestamp();
}

double ExecutionAdapter::speed() const {
  auto em{dynamic_cast<ExecutionManager *>(parent())};
  return em->speed();
}

void ExecutionAdapter::speed(double factor) const {
  auto em{dynamic_cast<ExecutionManager *>(parent())};
  return em->speed(factor);
}
