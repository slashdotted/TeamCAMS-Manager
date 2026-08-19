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
#include "testmixer.h"
#include "simulation/components/mixer.h"
#include "testcomponent.h"
#include <QtTest/QtTest>

TestMixer::TestMixer(QObject *parent) : QObject(parent) {}

void TestMixer::testPull() {
  Mixer m;
  Registry r;
  TestComponent tc_o2feed;
  m.connectSource(&tc_o2feed, "o2_feed");
  TestComponent tc_n2feed;
  m.connectSource(&tc_n2feed, "n2_feed");
  r.set("components.mixer.block", true, true);
  m.pull(r, 1.0, "o2_feed");
  QVERIFY(tc_o2feed.totalPulled("o2_feed") == 0.4);
  QVERIFY(tc_o2feed.totalPulled("n2_feed") == 0.0);
  QVERIFY(tc_n2feed.totalPulled("n2_feed") == 0.0);
  QVERIFY(tc_n2feed.totalPulled("o2_feed") == 0.0);
  tc_o2feed.reset();
  tc_n2feed.reset();
  m.pull(r, 1.0, "n2_feed");
  QVERIFY(tc_o2feed.totalPulled("o2_feed") == 0.0);
  QVERIFY(tc_o2feed.totalPulled("n2_feed") == 0.0);
  QVERIFY(tc_n2feed.totalPulled("n2_feed") == 0.5);
  QVERIFY(tc_n2feed.totalPulled("o2_feed") == 0.0);
  tc_o2feed.reset();
  tc_n2feed.reset();
  r.set("components.mixer.block", false);
  m.pull(r, 1.0, "o2_feed");
  QVERIFY(tc_o2feed.totalPulled("o2_feed") == 1.0);
  QVERIFY(tc_o2feed.totalPulled("n2_feed") == 0.0);
  QVERIFY(tc_n2feed.totalPulled("n2_feed") == 0.0);
  QVERIFY(tc_n2feed.totalPulled("o2_feed") == 0.0);
  tc_o2feed.reset();
  tc_n2feed.reset();
  m.pull(r, 1.0, "n2_feed");
  QVERIFY(tc_o2feed.totalPulled("o2_feed") == 0.0);
  QVERIFY(tc_o2feed.totalPulled("n2_feed") == 0.0);
  QVERIFY(tc_n2feed.totalPulled("n2_feed") == 1.0);
  QVERIFY(tc_n2feed.totalPulled("o2_feed") == 0.0);
  tc_o2feed.reset();
  tc_n2feed.reset();
}
