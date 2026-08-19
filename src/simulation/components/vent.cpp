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
#include "vent.h"
#include <QDebug>

void Vent::push(Registry &s, double amount, const QString &flow) {
  Q_UNUSED(s)
  Q_UNUSED(amount)
  Q_UNUSED(flow)
  Q_UNREACHABLE();
}

void Vent::pull(Registry &s, double amount, const QString &flow) {
  if (s.get("components.vent.running").toBool()) {
    double efficiency{s.get("components.vent.efficiency").toDouble()};
    double percent{s.get("components.vent.percent").toDouble()};
    QString strength{s.get("components.vent.strength").toString()};
    double factor{0.0};
    if (flow == "co2_vent") {
      if (strength == "high") {
        factor = percent * 4.0;
      } else if (strength == "medium") {
        factor = percent * 2.0;
      } else if (strength == "standard") {
        factor = percent * 1.0;
      } else {
        Q_UNREACHABLE();
      }
      Q_ASSERT(-1.0 * amount * factor * efficiency <=
               0); // Can only vent out gas
      pushToSinks(s, -1.0 * amount * factor * efficiency, flow);
    } else if (flow == "o2_vent") {
      if (strength == "high") {
        factor = percent * 4.0;
      } else if (strength == "medium") {
        factor = percent * 2.0;
      } else if (strength == "standard") {
        factor = percent * 1.0;
      } else {
        Q_UNREACHABLE();
      }
      Q_ASSERT(-1.0 * amount * factor * efficiency <=
               0); // Can only vent out gas
      pushToSinks(s, -1.0 * amount * factor * efficiency, flow);
    } else if (flow == "n2_vent") {
      if (strength == "high") {
        factor = percent * 4.0;
      } else if (strength == "medium") {
        factor = percent * 2.0;
      } else if (strength == "standard") {
        factor = percent * 1.0;
      } else {
        Q_UNREACHABLE();
      }
      Q_ASSERT(-1.0 * amount * factor * efficiency <=
               0); // Can only vent out gas
      pushToSinks(s, -1.0 * amount * factor * efficiency, flow);
    } else {
      Q_UNREACHABLE();
    }
  } else {
    pushToSinks(s, 0, flow);
  }
}
