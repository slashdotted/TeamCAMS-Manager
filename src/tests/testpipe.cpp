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
#include "testpipe.h"
#include "simulation/components/pipe.h"
#include "testcomponent.h"
#include <QtTest/QtTest>

TestPipe::TestPipe(QObject *parent) : QObject(parent) {}

void TestPipe::testPush() {
  Registry r;
  Pipe p{"sample"};
  r.set("components.sample.block", false, true);
  r.set("components.sample.flow", -1.0, true);
  TestComponent tc;
  p.connectSink(&tc, "sample");
  p.push(r, 1.0, "sample");
  QVERIFY(tc.totalPushed("sample") == 1.0);
  QVERIFY(r.get("components.sample.flow").toDouble() == 1.0);
  r.set("components.sample.block", true);
  tc.reset();
  p.push(r, 1.0, "sample");
  QVERIFY(tc.totalPushed("sample") == 0.4);
  QVERIFY(r.get("components.sample.flow").toDouble() == 0.4);
}

void TestPipe::testPull() {
  Registry r;
  Pipe p{"sample"};
  r.set("components.sample.leak", false, true);
  TestComponent tc;
  p.connectSource(&tc, "sample");
  p.pull(r, 1.0, "sample");
  QVERIFY(tc.totalPulled("sample") == 1.0);
  r.set("components.sample.leak", true);
  tc.reset();
  p.pull(r, 1.0, "sample");
  QVERIFY(tc.totalPulled("sample") == 1.6);
}
