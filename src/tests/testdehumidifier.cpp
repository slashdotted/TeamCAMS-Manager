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
#include "testdehumidifier.h"
#include "simulation/components/dehumidifier.h"
#include "testcomponent.h"
#include <QtTest/QtTest>

TestDehumidifier::TestDehumidifier(QObject *parent) : QObject(parent) {}

void TestDehumidifier::testPull() {
  Dehumidifier c;
  Registry r;
  r.set("components.dehumidifier.running", true, true);
  r.set("components.dehumidifier.factor", 0.0, true);
  r.set("components.dehumidifier.efficiency", 1.0, true);
  r.set("components.dehumidifier.strength", "medium", true);
  TestComponent tc;
  c.connectSink(&tc, "humidity");
  r.set("components.dehumidifier.running", false);
  c.pull(r, 1.0, "");
  QVERIFY(tc.totalPushed("humidity") == 0.0);
  tc.reset();
  r.set("components.dehumidifier.running", true);
  r.set("components.dehumidifier.factor", 1.0);
  c.pull(r, 1.0, "");
  QVERIFY(tc.totalPushed("humidity") == -0.2);
  tc.reset();
  r.set("components.dehumidifier.strength", "high");
  c.pull(r, 1.0, "");
  QVERIFY(tc.totalPushed("humidity") == -0.4);
  tc.reset();
  r.set("components.dehumidifier.strength", "standard");
  c.pull(r, 1.0, "");
  QVERIFY(tc.totalPushed("humidity") == -0.13);
  tc.reset();
  r.set("components.dehumidifier.efficiency", 0.5);
  c.pull(r, 1.0, "");
  QVERIFY(tc.totalPushed("humidity") == -0.065);
  tc.reset();
}
