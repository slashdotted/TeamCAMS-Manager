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
#ifndef TESTREGISTRY_H
#define TESTREGISTRY_H

#include "simulation/registry.h"
#include <QObject>

class TestRegistry : public QObject {
  Q_OBJECT
public:
  explicit TestRegistry(QObject *parent = nullptr);

private slots:
  void undefinedProperty();
  void get();
  void set();
  void stick();
  void setAndStick();
  void unstick();
  void isStuck();
  void stuckProperties();
};

#endif // TESTREGISTRY_H
