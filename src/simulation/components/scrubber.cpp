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
#include "scrubber.h"
#include <QDebug>

void Scrubber::push(Registry &s, double amount, const QString &flow) {
  Q_UNUSED(s);
  Q_UNUSED(amount);
  Q_UNUSED(flow);
  Q_UNREACHABLE();
}

void Scrubber::pull(Registry &s, double amount, const QString &flow) {
  if (s.get("components.scrubber.running").toBool()) {
    double efficiency{s.get("components.scrubber.efficiency").toDouble()};
    QString strength{s.get("components.scrubber.strength").toString()};
    double factor{0.0};
    if (flow == "co2_scrub") {
      if (strength == "high") {
        if (amount > 0.45) {
          factor = 7.0;
        } else if (amount > 0.35) {
          factor = 5.0;
        } else {
          factor = 3.6;
        }
      } else if (strength == "medium") {
        if (amount > 0.45) {
          factor = 4.5;
        } else {
          factor = 3.6;
        }
      } else if (strength == "standard") {
        if (amount > 0.45) {
          factor = 4;
        } else {
          factor = 3.4;
        }
      } else {
        Q_UNREACHABLE();
      }
      Q_ASSERT(-1.0 * factor * efficiency <= 0); // Can only scrub CO2
      pushToSinks(s, -1.0 * factor * efficiency, flow);
    } else if (flow == "o2_scrub" || flow == "n2_scrub") {
      if (strength == "high") {
        factor = 2.0;
      } else if (strength == "medium") {
        factor = 1.0;
      } else if (strength == "standard") {
        factor = 0.8;
      } else {
        Q_UNREACHABLE();
      }
      Q_ASSERT(-1.0 * 0.0015 * factor * efficiency * amount <=
               0); // Can only scrub N2 / O2
      pushToSinks(s, -1.0 * 0.0015 * factor * efficiency * amount, flow);
    } else {
      Q_UNREACHABLE();
    }
  } else {
    pushToSinks(s, 0, flow);
  }
}
