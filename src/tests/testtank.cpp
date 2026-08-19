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
#include "testtank.h"
#include "simulation/components/tank.h"
#include "testcomponent.h"
#include <QtTest/QtTest>

TestTank::TestTank(QObject *parent) : QObject(parent) {}

void TestTank::testPull() {
  Tank t{"gas"};
  Registry r;
  r.set("components.gas.volume", 100, true);
  r.set("components.gas.open", false, true);
  r.set("components.gas.infinite", false, true);
  r.set("components.gas.capacity", 100, true);
  TestComponent tc;
  t.connectSink(&tc, "gas");
  t.pull(r, 1.0, "gas");
  QVERIFY(tc.totalPushed("gas") == 0.0);
  tc.reset();
  r.set("components.gas.open", true);
  t.pull(r, 1.0, "gas");
  QVERIFY(tc.totalPushed("gas") == 1.0);
  QVERIFY(r.get("components.gas.volume") == 99);
  QVERIFY(r.get("components.gas.capacity") == 100);
  tc.reset();
  t.pull(r, 100, "gas");
  QVERIFY(tc.totalPushed("gas") == 99.0);
  QVERIFY(r.get("components.gas.volume") == 0);
  QVERIFY(r.get("components.gas.capacity") == 100);
  tc.reset();
  r.set("components.gas.infinite", true);
  t.pull(r, 500, "gas");
  QVERIFY(tc.totalPushed("gas") == 500.0);
  QVERIFY(r.get("components.gas.volume").toInt() > 0);
  QVERIFY(r.get("components.gas.capacity") == 100);
}
