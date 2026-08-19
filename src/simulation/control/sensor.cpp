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
#include "sensor.h"
#include <QDebug>

struct Sensor::pimpl {
  QString m_property;
  QString m_observed;
  QString m_upper;
  QString m_lower;
};

Sensor::Sensor(const QString &property, const QString &observed,
               const QString &upper, const QString &lower)
    : m_pimpl{std::make_unique<pimpl>()} {
  m_pimpl->m_property = property;
  m_pimpl->m_observed = observed;
  m_pimpl->m_upper = upper;
  m_pimpl->m_lower = lower;
}

Sensor::~Sensor() = default;

void Sensor::update(Registry &s) {
  double upper_threshold = s.get(m_pimpl->m_upper).toDouble();
  double lower_threshold = s.get(m_pimpl->m_lower).toDouble();
  double value{s.get(m_pimpl->m_observed).toDouble()};
  if (s.get(m_pimpl->m_property + ".failure").toBool()) {
    s.set(m_pimpl->m_property + ".value", "normal");
  } else if (value >= upper_threshold) {
    if (s.get(m_pimpl->m_property + ".upper.failure").toBool()) {
      s.set(m_pimpl->m_property + ".value", "normal");
    } else {
      s.set(m_pimpl->m_property + ".value", "high");
    }
  } else if (value <= lower_threshold) {
    if (s.get(m_pimpl->m_property + ".lower.failure").toBool()) {
      s.set(m_pimpl->m_property + ".value", "normal");
    } else {
      s.set(m_pimpl->m_property + ".value", "low");
    }
  } else {
    s.set(m_pimpl->m_property + ".value", "normal");
  }
}
