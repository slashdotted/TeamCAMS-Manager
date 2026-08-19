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
#include "valve.h"
#include <QDebug>

Valve::Valve(const QString &name) : m_name{name} {}

void Valve::push(Registry &s, double amount, const QString &flow) {
  double available{amount};
  if (s.get(QString{} + "components." + m_name + ".open").toBool()) {
    if (s.get(QString{} + "components." + m_name + ".leak").toBool()) {
      if (flow == "n2_feed") {
        available *= 0.2; // 80% leaked
      } else if (flow == "o2_feed") {
        available *= 0.4; // 60% leaked
      } else {
        Q_UNREACHABLE();
      }
    }
    Q_ASSERT(available >= 0); // Can only push gas to sinks
    pushToSinks(s, available, flow);
  } else {
    pushToSinks(s, 0, flow);
  }
}

void Valve::pull(Registry &s, double amount, const QString &flow) {
  double requested{amount};
  if (s.get(QString{} + "components." + m_name + ".open").toBool()) {

    if (flow == "o2_feed") {
      // if (s.get(QString{}+"components."+m_name+".block").toBool()) {
      //     requested = 0.5 * amount; // 50% blocked
      // }
      if (s.get(QString{} + "components." + m_name + ".block").toBool()) {
        double factor{
            s.get("components." + m_name + ".block.factor").toDouble()};
        requested = (1.0 - factor) * amount; // 50% blocked
        factor = factor + 0.05;
        if (factor > 0.86) {
          factor = 0.85;
        }
        s.set("components." + m_name + ".block.factor", factor);
      }
      // The following code is disable since we now want
      // the leak to happen after the valve (gas is lost when pushing)
      /*if (s.get(QString{}+"components."+m_name+".leak").toBool()) {
      requested *= 1.5; // 50% leaked
      }*/
      QString strength{s.get("components." + m_name + ".strength").toString()};
      if (strength == "high") {
        requested *= 64.0;
      } else if (strength == "medium") {
        requested *= 32.0;
      } else if (strength == "standard") {
        requested *= 20.0;
      } else {
        Q_UNREACHABLE();
      }
    } else if (flow == "n2_feed") {
      if (s.get(QString{} + "components." + m_name + ".block").toBool()) {
        double factor{
            s.get("components." + m_name + ".block.factor").toDouble()};
        requested = (1.0 - factor) * amount; // 50% blocked
        factor = factor + 0.05;
        if (factor > 0.86) {
          factor = 0.85;
        }
        s.set("components." + m_name + ".block.factor", factor);
      }
      // The following code is disable since we now want
      // the leak to happen after the valve (gas is lost when pushing)
      /*if (s.get(QString{}+"components."+m_name+".leak").toBool()) {
      requested *= 1.5; // 50% leaked
      }*/
      QString strength{s.get("components." + m_name + ".strength").toString()};
      if (strength == "high") {
        requested *= 160.0;
      } else if (strength == "medium") {
        requested *= 80.0;
      } else if (strength == "standard") {
        requested *= 40.0;
      } else {
        Q_UNREACHABLE();
      }
    } else {
      Q_UNREACHABLE();
    }
    pullFromSources(s, requested, flow);
  } else {
    pullFromSources(s, 0, flow);
  }
}
