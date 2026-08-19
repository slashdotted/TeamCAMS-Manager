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
#include "testscrubber.h"
#include "simulation/components/scrubber.h"
#include "testcomponent.h"
#include <QtTest/QtTest>

TestScrubber::TestScrubber(QObject *parent) : QObject(parent) {}

void TestScrubber::testPull() {
  TestComponent tc_co2;
  TestComponent tc_o2;
  TestComponent tc_n2;
  Scrubber s;
  s.connectSink(&tc_co2, "co2_scrub");
  s.connectSink(&tc_o2, "o2_scrub");
  s.connectSink(&tc_n2, "n2_scrub");
  Registry r;
  r.set("components.scrubber.running", true, true);
  r.set("components.scrubber.efficiency", 1.0, true);
  r.set("components.scrubber.strength", "high", true);
  s.pull(r, 1.0, "co2_scrub");
  QVERIFY(tc_co2.totalPushed("co2_scrub") == -7.0);
  tc_co2.reset();
  s.pull(r, 0.4, "co2_scrub");
  QVERIFY(tc_co2.totalPushed("co2_scrub") == -5.0);
  tc_co2.reset();
  s.pull(r, 0.3, "co2_scrub");
  QVERIFY(tc_co2.totalPushed("co2_scrub") == -3.6);
  tc_co2.reset();
  r.set("components.scrubber.strength", "medium");
  s.pull(r, 1.0, "co2_scrub");
  QVERIFY(tc_co2.totalPushed("co2_scrub") == -4.5);
  tc_co2.reset();
  s.pull(r, 0.4, "co2_scrub");
  QVERIFY(tc_co2.totalPushed("co2_scrub") == -3.6);
  tc_co2.reset();
  r.set("components.scrubber.strength", "standard");
  s.pull(r, 1.0, "co2_scrub");
  QVERIFY(tc_co2.totalPushed("co2_scrub") == -4.0);
  tc_co2.reset();
  s.pull(r, 0.4, "co2_scrub");
  QVERIFY(tc_co2.totalPushed("co2_scrub") == -3.4);
  tc_co2.reset();
  r.set("components.scrubber.efficiency", 0.5);
  s.pull(r, 0.4, "co2_scrub");
  QVERIFY(tc_co2.totalPushed("co2_scrub") == -1.7);
  tc_co2.reset();
  r.set("components.scrubber.running", false);
  s.pull(r, 0.4, "co2_scrub");
  QVERIFY(tc_co2.totalPushed("co2_scrub") == 0.0);
  tc_co2.reset();

  r.set("components.scrubber.running", true);
  r.set("components.scrubber.efficiency", 1.0);
  r.set("components.scrubber.strength", "high");
  s.pull(r, 1.0, "o2_scrub");
  QVERIFY(tc_o2.totalPushed("o2_scrub") == -0.003);
  tc_o2.reset();
  r.set("components.scrubber.strength", "medium");
  s.pull(r, 1.0, "o2_scrub");
  QVERIFY(tc_o2.totalPushed("o2_scrub") == -0.0015);
  tc_o2.reset();
  r.set("components.scrubber.strength", "standard");
  s.pull(r, 100.0, "o2_scrub");
  QVERIFY(tc_o2.totalPushed("o2_scrub") + 0.12 < 0.000001);
  tc_o2.reset();
  r.set("components.scrubber.efficiency", 0.5);
  s.pull(r, 100.0, "o2_scrub");
  QVERIFY(tc_o2.totalPushed("o2_scrub") + 0.06 < 0.000001);
  tc_o2.reset();
  r.set("components.scrubber.running", false);
  s.pull(r, 1.0, "o2_scrub");
  QVERIFY(tc_o2.totalPushed("o2_scrub") == 0.0);
  tc_o2.reset();

  r.set("components.scrubber.running", true);
  r.set("components.scrubber.efficiency", 1.0);
  r.set("components.scrubber.strength", "high");
  s.pull(r, 1.0, "n2_scrub");
  QVERIFY(tc_n2.totalPushed("n2_scrub") == -0.003);
  tc_n2.reset();
  r.set("components.scrubber.strength", "medium");
  s.pull(r, 1.0, "n2_scrub");
  QVERIFY(tc_n2.totalPushed("n2_scrub") == -0.0015);
  tc_n2.reset();
  r.set("components.scrubber.strength", "standard");
  s.pull(r, 100.0, "n2_scrub");
  QVERIFY(tc_n2.totalPushed("n2_scrub") + 0.12 < 0.000001);
  tc_n2.reset();
  r.set("components.scrubber.efficiency", 0.5);
  s.pull(r, 100.0, "n2_scrub");
  QVERIFY(tc_n2.totalPushed("n2_scrub") + 0.06 < 0.000001);
  tc_n2.reset();
  r.set("components.scrubber.running", false);
  s.pull(r, 1.0, "n2_scrub");
  QVERIFY(tc_n2.totalPushed("n2_scrub") == 0.0);
  tc_n2.reset();
}
