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
#include "testexperimentlist.h"
#include "models/experimentlist.h"
#include <QtTest/QtTest>

TestExperimentList::TestExperimentList(QObject *parent) : QObject(parent) {}

void TestExperimentList::testAddExperiment() {
  ExperimentList el{nullptr};
  QVERIFY(el.count() == 0);
  el.addExperiment("test");
  QVERIFY(el.count() == 1);
  el.addExperiment("other");
  QVERIFY(el.count() == 2);
}

void TestExperimentList::testRemoveExperiment() {
  ExperimentList el{nullptr};
  el.addExperiment("test");
  el.addExperiment("other");
  QVERIFY(el.count() == 2);
  el.removeExperiment("other");
  QVERIFY(el.count() == 1);
  el.removeExperiment(0);
  QVERIFY(el.count() == 0);
}

void TestExperimentList::testGetExperiment() {
  ExperimentList el{nullptr};
  QVERIFY(el.addExperiment("test"));
  QVERIFY(el.getExperiment(0) == el.getExperiment("test"));
}

void TestExperimentList::testHasExperiment() {
  ExperimentList el{nullptr};
  el.addExperiment("test");
  QVERIFY(el.hasExperiment("test"));
  QVERIFY(!el.hasExperiment("other"));
}

void TestExperimentList::testSerialization() {
  QJsonObject data;
  ExperimentList el{nullptr};
  el.addExperiment("test");
  el.addExperiment("other");
  el.save(data);
  ExperimentList el2{nullptr};
  el2.load(data);
  QVERIFY(el2.count() == 2);
  QVERIFY(el2.hasExperiment("test"));
  QVERIFY(el2.hasExperiment("other"));
}
