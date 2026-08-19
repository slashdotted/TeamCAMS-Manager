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
#include "testcooler.h"
#include "simulation/components/cooler.h"
#include "testcomponent.h"
#include <QtTest/QtTest>

TestCooler::TestCooler(QObject *parent) : QObject(parent) {}

void TestCooler::testPull() {
  Cooler c;
  Registry r;
  r.set("components.cooler.running", true, true);
  r.set("components.cooler.factor", 0.0, true);
  r.set("components.cooler.efficiency", 1.0, true);
  r.set("components.cooler.strength", "medium", true);
  TestComponent tc;
  c.connectSink(&tc, "cooler");
  r.set("components.cooler.running", false);
  c.pull(r, 1.0, "");
  QVERIFY(tc.totalPushed("cooler") == 0.0);
  tc.reset();
  r.set("components.cooler.running", true);
  r.set("components.cooler.factor", 1.0);
  c.pull(r, 1.0, "");
  QVERIFY(tc.totalPushed("cooler") == -0.08);
  tc.reset();
  r.set("components.cooler.strength", "high");
  c.pull(r, 1.0, "");
  QVERIFY(tc.totalPushed("cooler") == -0.16);
  tc.reset();
  r.set("components.cooler.strength", "standard");
  c.pull(r, 1.0, "");
  QVERIFY(tc.totalPushed("cooler") == -0.04);
  tc.reset();
  r.set("components.cooler.efficiency", 0.5);
  c.pull(r, 1.0, "");
  QVERIFY(tc.totalPushed("cooler") == -0.02);
  tc.reset();
}
