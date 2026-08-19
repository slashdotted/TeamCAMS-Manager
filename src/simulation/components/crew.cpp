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
#include "crew.h"
#include <QDebug>

void Crew::push(Registry &s, double amount, const QString &flow) {
  Q_UNUSED(s);
  Q_UNUSED(amount);
  Q_UNUSED(flow);
  Q_UNREACHABLE();
}

void Crew::pull(Registry &s, double amount, const QString &flow) {
  Q_UNUSED(s);
  Q_UNUSED(amount);
  Q_UNUSED(flow);
  Q_UNREACHABLE();
}

void Crew::update(Registry &s) {
  double o2consumption = 2 + (double(rand()) / RAND_MAX) * 2.0;
  s.set("components.crew.o2consumption", o2consumption);
  pushToSinks(s, -1.0 * o2consumption, "o2_crew");
  pushToSinks(s, o2consumption, "co2_crew");
  pushToSinks(s, 0.04, "heater_crew");
  pushToSinks(s, 0.05, "humidity_crew");
}
