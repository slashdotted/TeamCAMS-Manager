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
#include "testtriggers.h"
#include "events/triggers.h"
#include <QtTest/QtTest>

TestTriggers::TestTriggers(QObject *parent) : QObject(parent) {}

void TestTriggers::onEvaluateRequest(const QString &code,
                                     const QVariantMap &bindings) {
  Q_UNUSED(bindings);
  m_code = code;
}

void TestTriggers::testActivate() {
  Triggers tr;
  QString tid{tr.activate("test", "code")};
  QString tid2{tr.activate("test", "code", tid)};
  QVERIFY(tr.exists(tid));
  QVERIFY(tid == tid2);
}

void TestTriggers::testDeactivate() {
  Triggers tr;
  QString tid{tr.activate("test", "code")};
  QString tid2{tr.activate("test", "code", "", "type")};
  QString tid3{tr.activate("test", "code", tid2, "othertype")};
  QVERIFY(tr.exists(tid));
  tr.deactivateByUuid(tid);
  QVERIFY(!tr.exists(tid));
  QVERIFY(tr.exists(tid2));
  QVERIFY(tr.exists(tid3));
  tr.deactivateByType("type");
  QVERIFY(!tr.exists(tid));
  QVERIFY(!tr.exists(tid2));
  QVERIFY(!tr.exists(tid3));
}

void TestTriggers::testProcessTriggers() {
  Triggers tr;
  connect(&tr, &Triggers::evaluateRequest, this,
          &TestTriggers::onEvaluateRequest);
  QString tid{tr.activate("test", "code")};
  QVariantMap map;
  tr.processTriggers("test", map);
  QVERIFY(m_code == "code");
}

void TestTriggers::clearSignalData() { m_code = ""; }
