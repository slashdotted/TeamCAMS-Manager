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
#include "testscheduler.h"
#include "events/scheduler.h"
#include <QtTest/QtTest>

TestScheduler::TestScheduler(QObject *parent) : QObject(parent) {}

void TestScheduler::onEvaluateRequest(const QString &code,
                                      const QVariantMap &bindings) {
  m_code = code;
  m_bindings = bindings;
}

void TestScheduler::testSchedule() {
  Scheduler s;
  QString eid{s.schedule(42, "testcode", "", "testtype")};
  QVERIFY(s.exists(eid));
  QString eid2{s.schedule(42, 3, "testcode", "", "testtype")};
  QVERIFY(s.exists(eid2));
}

void TestScheduler::testUnschedule() {
  Scheduler s;
  QString eid{s.schedule(42, "testcode", "", "testtype")};
  QVERIFY(s.exists(eid));
  QString eid2{s.schedule(42, 3, "testcode", "", "testtype")};
  QVERIFY(s.exists(eid2));
  QString eid3{s.schedule(42, 3, "testcode", eid2, "testtype")};
  QVERIFY(s.exists(eid3));
  s.unscheduleByUuid(eid);
  QVERIFY(!s.exists(eid));
  QVERIFY(s.exists(eid2));
  s.unscheduleByType("testtype");
  QVERIFY(!s.exists(eid));
  QVERIFY(!s.exists(eid2));
  QVERIFY(!s.exists(eid3));
}

void TestScheduler::testExists() {
  Scheduler s;
  connect(&s, &Scheduler::evaluateRequest, this,
          &TestScheduler::onEvaluateRequest);
  QString eid{s.schedule(42, "testcode", "", "testtype")};
  QVERIFY(s.exists(eid));
  QVERIFY(!s.exists(""));
}

void TestScheduler::testPostpone() {
  Scheduler s;
  connect(&s, &Scheduler::evaluateRequest, this,
          &TestScheduler::onEvaluateRequest);
  QString eid{s.schedule(1, "testcode", "", "testtype")};
  QVERIFY(s.exists(eid));
  s.postpone(1, eid);
  s.processEvents(1);
  QVERIFY(m_code == "");
  clearSignalData();
  s.processEvents(2);
  QVERIFY(m_code == "testcode");
  QVERIFY(m_bindings.contains("_eventid") && (m_bindings["_eventid"] == eid));
}

void TestScheduler::testProcessEvents() {
  Scheduler s;
  connect(&s, &Scheduler::evaluateRequest, this,
          &TestScheduler::onEvaluateRequest);
  QString eid{s.schedule(1, "testcode", "", "testtype")};
  QVERIFY(s.exists(eid));
  s.processEvents(1);
  QVERIFY(m_code == "testcode");
  QVERIFY(m_bindings.contains("_eventid") && (m_bindings["_eventid"] == eid));
  QVERIFY(!s.exists(eid));
  QString eid2{s.schedule(1, 1, "testcode2", "", "testtype")};
  QVERIFY(s.exists(eid2));
  s.processEvents(1);
  QVERIFY(m_code == "testcode2");
  QVERIFY(m_bindings.contains("_eventid") && (m_bindings["_eventid"] == eid2));
  clearSignalData();
  s.processEvents(2);
  QVERIFY(m_code == "testcode2");
  QVERIFY(m_bindings.contains("_eventid") && (m_bindings["_eventid"] == eid2));
}

void TestScheduler::clearSignalData() {
  m_bindings.clear();
  m_code = "";
}
