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
#include "testheater.h"
#include "simulation/components/heater.h"
#include "testcomponent.h"
#include <QtTest/QtTest>

TestHeater::TestHeater(QObject *parent) : QObject(parent) {}

void TestHeater::testPull() {
  Heater c;
  Registry r;
  r.set("components.heater.running", true, true);
  r.set("components.heater.factor", 0.0, true);
  r.set("components.heater.efficiency", 1.0, true);
  r.set("components.heater.strength", "medium", true);
  TestComponent tc;
  c.connectSink(&tc, "heater");
  r.set("components.heater.running", false);
  c.pull(r, 1.0, "");
  QVERIFY(tc.totalPushed("heater") == 0.0);
  tc.reset();
  r.set("components.heater.running", true);
  r.set("components.heater.factor", 1.0);
  c.pull(r, 1.0, "");
  QVERIFY(tc.totalPushed("heater") == 0.10);
  tc.reset();
  r.set("components.heater.strength", "high");
  c.pull(r, 1.0, "");
  QVERIFY(tc.totalPushed("heater") == 0.20);
  tc.reset();
  r.set("components.heater.strength", "standard");
  c.pull(r, 1.0, "");
  QVERIFY(tc.totalPushed("heater") == 0.05);
  tc.reset();
  r.set("components.heater.efficiency", 0.5);
  c.pull(r, 1.0, "");
  QVERIFY(tc.totalPushed("heater") == 0.025);
  tc.reset();
}
