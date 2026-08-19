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
#include "testexperiment.h"
#include "experiment/experiment.h"
#include <QtTest/QtTest>

TestExperiment::TestExperiment(QObject *parent) : QObject(parent) {}

void TestExperiment::testName() {
  Experiment e{"test", nullptr, this};
  QVERIFY(e.name() == "test");
}

void TestExperiment::testSetName() {
  Experiment e{"test", nullptr, this};
  QVERIFY(e.name() == "test");
  e.name("other");
  QVERIFY(e.name() == "other");
}

void TestExperiment::testDescription() {
  Experiment e{"test", nullptr, this};
  QVERIFY(e.description() == "");
}

void TestExperiment::testSetDescription() {
  Experiment e{"test", nullptr, this};
  e.description("my description");
  QVERIFY(e.description() == "my description");
}

void TestExperiment::testSetScript() {
  Experiment e{"test", nullptr, this};
  e.script("script.xml");
  QVERIFY(e.script() == "script.xml");
}

void TestExperiment::testScript() {
  Experiment e{"test", nullptr, this};
  QVERIFY(e.script() == "");
}

void TestExperiment::testSerialization() {
  Experiment e{"test", nullptr, this};
  e.description("description");
  e.script("myscript.xml");
  QJsonObject data;
  e.save(data);
  Experiment f{"test2", nullptr, this};
  f.load(data);
  QVERIFY(e.name() != f.name());
  f.load(data, false);
  QVERIFY(e.name() == f.name());
  QVERIFY(e.description() == f.description());
  QVERIFY(e.script() == f.script());
}
