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
#include "testvent.h"
#include "simulation/components/vent.h"
#include "testcomponent.h"
#include <QtTest/QtTest>

TestVent::TestVent(QObject *parent) : QObject(parent) {}

void TestVent::testPull() {
  Registry r;
  Vent v;
  r.set("components.vent.running", true, true);
  r.set("components.vent.efficiency", 1.0, true);
  r.set("components.vent.percent", 0.004, true);
  r.set("components.vent.strength", "high", true);
  TestComponent tc_co2;
  v.connectSink(&tc_co2, "co2_vent");
  TestComponent tc_o2;
  v.connectSink(&tc_o2, "o2_vent");
  TestComponent tc_n2;
  v.connectSink(&tc_n2, "n2_vent");
  v.pull(r, 1.0, "co2_vent");
  QVERIFY(tc_co2.totalPushed("co2_vent") == -4.0 * 0.004);
  tc_co2.reset();
  v.pull(r, 1.0, "o2_vent");
  QVERIFY(tc_o2.totalPushed("o2_vent") == -4.0 * 0.004);
  tc_o2.reset();
  v.pull(r, 1.0, "n2_vent");
  QVERIFY(tc_n2.totalPushed("n2_vent") == -4.0 * 0.004);
  tc_n2.reset();
  r.set("components.vent.strength", "medium");
  v.pull(r, 1.0, "co2_vent");
  QVERIFY(tc_co2.totalPushed("co2_vent") == -2.0 * 0.004);
  tc_co2.reset();
  v.pull(r, 1.0, "o2_vent");
  QVERIFY(tc_o2.totalPushed("o2_vent") == -2.0 * 0.004);
  tc_o2.reset();
  v.pull(r, 1.0, "n2_vent");
  QVERIFY(tc_n2.totalPushed("n2_vent") == -2.0 * 0.004);
  tc_n2.reset();
  r.set("components.vent.strength", "standard");
  v.pull(r, 1.0, "co2_vent");
  QVERIFY(tc_co2.totalPushed("co2_vent") == -1.0 * 0.004);
  tc_co2.reset();
  v.pull(r, 1.0, "o2_vent");
  QVERIFY(tc_o2.totalPushed("o2_vent") == -1.0 * 0.004);
  tc_o2.reset();
  v.pull(r, 1.0, "n2_vent");
  QVERIFY(tc_n2.totalPushed("n2_vent") == -1.0 * 0.004);
  tc_n2.reset();
  r.set("components.vent.efficiency", 0.5);
  v.pull(r, 1.0, "co2_vent");
  QVERIFY(tc_co2.totalPushed("co2_vent") == -0.5 * 0.004);
  tc_co2.reset();
  v.pull(r, 1.0, "o2_vent");
  QVERIFY(tc_o2.totalPushed("o2_vent") == -0.5 * 0.004);
  tc_o2.reset();
  v.pull(r, 1.0, "n2_vent");
  QVERIFY(tc_n2.totalPushed("n2_vent") == -0.5 * 0.004);
  tc_n2.reset();
  r.set("components.vent.running", false);
  v.pull(r, 1.0, "co2_vent");
  QVERIFY(tc_co2.totalPushed("co2_vent") == 0.0);
  tc_co2.reset();
  v.pull(r, 1.0, "o2_vent");
  QVERIFY(tc_o2.totalPushed("o2_vent") == 0.0);
  tc_o2.reset();
  v.pull(r, 1.0, "n2_vent");
  QVERIFY(tc_n2.totalPushed("n2_vent") == 0.0);
  tc_n2.reset();
}
