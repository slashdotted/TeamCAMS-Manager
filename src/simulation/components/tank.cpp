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
#include "tank.h"
#include <QDebug>

Tank::Tank(const QString &name) : m_name{name} {}

void Tank::pull(Registry &s, double amount, const QString &flow) {
  double consumed{amount};
  // Q_ASSERT(amount < 1000);
  double available{s.get("components." + m_name + ".volume").toDouble()};
  if (s.get("components." + m_name + ".open").toBool()) {
    // Infinite amount of gas
    if (available < amount &&
        s.get("components." + m_name + ".infinite").toBool()) {
      available = s.get("components." + m_name + ".capacity").toDouble();
    } else {
      consumed = qMin(available, amount);
      available -= consumed;
    }
    // Update amount
    s.set("components." + m_name + ".volume", available);
    // Push requested gas to sinks
    Q_ASSERT(consumed >= 0); // Can only push gas from tank
    pushToSinks(s, consumed, flow);
  } else {
    pushToSinks(s, 0, flow);
  }
}

void Tank::push(Registry &s, double amount, const QString &flow) {
  Q_UNUSED(s);
  Q_UNUSED(amount);
  Q_UNUSED(flow);
  Q_UNREACHABLE();
}
