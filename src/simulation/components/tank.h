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
#ifndef TANK_H
#define TANK_H

#include "streamingcomponent.h"

class Tank : public StreamingComponent {
public:
  Tank(const QString &name);

  /**
   * @brief pulls the requested amount of gas from the tank
   * @param s the state of the system
   * @param amount of requested gas
   * @param flow identifier of the gas
   */
  void pull(Registry &s, double amount, const QString &flow) override;

  void push(Registry &s, double amount, const QString &flow) override;

private:
  QString m_name;
};

#endif // TANK_H
