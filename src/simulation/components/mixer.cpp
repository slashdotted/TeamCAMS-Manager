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
#include "mixer.h"

void Mixer::push(Registry &s, double amount, const QString &flow) {
  pushToSinks(s, amount, flow);
}

void Mixer::pull(Registry &s, double amount, const QString &flow) {
  if (flow == "o2_feed") {
    if (s.get("components.mixer.block").toBool()) {
      amount *= 0.4;
    }
    pullFromSources(s, amount, flow);
  } else if (flow == "n2_feed") {
    if (s.get("components.mixer.block").toBool()) {
      amount *= 0.5;
    }
    pullFromSources(s, amount, flow);
  } else {
    Q_UNREACHABLE();
  }
}
