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
#include "heater.h"

void Heater::push(Registry &s, double amount, const QString &flow) {
  Q_UNUSED(s);
  Q_UNUSED(amount);
  Q_UNUSED(flow);
  Q_UNREACHABLE();
}

void Heater::pull(Registry &s, double amount, const QString &flow) {
  Q_UNUSED(amount);
  Q_UNUSED(flow);
  if (s.get("components.heater.running").toBool() ||
      s.get("components.heater.factor").toDouble() > 0) {
    double factor{0.05 * s.get("components.heater.factor").toDouble()};
    double efficiency{s.get("components.heater.efficiency").toDouble()};
    QString strength{s.get("components.heater.strength").toString()};
    if (strength == "high") {
      factor *= 4.0;
    } else if (strength == "medium") {
      factor *= 2.0;
    } else if (strength == "standard") {
      factor *= 1.0;
    } else {
      Q_UNREACHABLE();
    }
    factor *= efficiency;
    Q_ASSERT(factor >= 0); // Can only increase temperature
    pushToSinks(s, factor, "heater");
  } else {
    pushToSinks(s, 0, "heater");
  }
}
