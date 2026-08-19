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
#include "testvalve.h"
#include "simulation/components/valve.h"
#include "testcomponent.h"
#include <QtTest/QtTest>

TestValve::TestValve(QObject *parent) : QObject(parent) {}

void TestValve::testPush() {
  Valve v{"gasvalve"};
  Registry r;
  r.set("components.gasvalve.block.factor", 0.4, true);
  r.set("components.gasvalve.open", true, true);
  r.set("components.gasvalve.leak", false, true);
  TestComponent tc;
  v.connectSink(&tc, "n2_feed");
  v.push(r, 1.0, "n2_feed");
  QVERIFY(tc.totalPushed("n2_feed") == 1.0);
  tc.reset();
  r.set("components.gasvalve.leak", true);
  v.push(r, 1.0, "n2_feed");
  QVERIFY(tc.totalPushed("n2_feed") == 0.2);
  tc.reset();
  r.set("components.gasvalve.open", false);
  v.push(r, 1.0, "n2_feed");
  QVERIFY(tc.totalPushed("n2_feed") == 0.0);
}

void TestValve::testPull() {
  Valve v{"gasvalve"};
  Registry r;
  r.set("components.gasvalve.block.factor", 0.4, true);
  r.set("components.gasvalve.open", true, true);
  r.set("components.gasvalve.leak", false, true);
  r.set("components.gasvalve.block", false, true);
  r.set("components.gasvalve.strength", "high", true);
  TestComponent tc_o2;
  v.connectSource(&tc_o2, "o2_feed");
  TestComponent tc_n2;
  v.connectSource(&tc_n2, "n2_feed");
  v.pull(r, 1.0, "o2_feed");
  QVERIFY(tc_o2.totalPulled("o2_feed") == 64.0);
  tc_o2.reset();
  r.set("components.gasvalve.strength", "medium");
  v.pull(r, 1.0, "o2_feed");
  QVERIFY(tc_o2.totalPulled("o2_feed") == 32.0);
  tc_o2.reset();
  r.set("components.gasvalve.strength", "standard");
  v.pull(r, 1.0, "o2_feed");
  QVERIFY(tc_o2.totalPulled("o2_feed") == 20.0);
  tc_o2.reset();
  r.set("components.gasvalve.block", true);
  v.pull(r, 1.0, "o2_feed");
  QVERIFY(tc_o2.totalPulled("o2_feed") == 12.0);
  tc_o2.reset();
  r.set("components.gasvalve.block", false);
  r.set("components.gasvalve.leak", true);
  v.pull(r, 1.0, "o2_feed");
  QVERIFY(tc_o2.totalPulled("o2_feed") == 20.0);
  tc_o2.reset();
  r.set("components.gasvalve.leak", false);
  r.set("components.gasvalve.open", false);
  v.pull(r, 1.0, "o2_feed");
  QVERIFY(tc_o2.totalPulled("o2_feed") == 0.0);
  tc_o2.reset();
  r.set("components.gasvalve.open", true);
  r.set("components.gasvalve.strength", "high");
  v.pull(r, 1.0, "n2_feed");
  QVERIFY(tc_n2.totalPulled("n2_feed") == 160.0);
  tc_n2.reset();
  r.set("components.gasvalve.strength", "medium");
  v.pull(r, 1.0, "n2_feed");
  QVERIFY(tc_n2.totalPulled("n2_feed") == 80.0);
  tc_n2.reset();
  r.set("components.gasvalve.strength", "standard");
  v.pull(r, 1.0, "n2_feed");
  QVERIFY(tc_n2.totalPulled("n2_feed") == 40.0);
  tc_n2.reset();
}
