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
#include "testregistry.h"
#include "experiment/exceptions.h"
#include <QtTest/QtTest>

TestRegistry::TestRegistry(QObject *parent) : QObject(parent) {}

void TestRegistry::undefinedProperty() {
  Registry r;
  QVERIFY_THROWS_EXCEPTION(UndefinedPropertyException,
                           r.set("someproperty", 44));
  QVERIFY_THROWS_EXCEPTION(UndefinedPropertyException, r.get("someproperty"));
}

void TestRegistry::get() {
  Registry r;
  r.set("sample_int", 42, true);
  r.set("sample_string", "example", true);
  QVERIFY(r.get("sample_int").toInt() == 42);
  QVERIFY(r.get("sample_string").toString() == "example");
}

void TestRegistry::set() {
  Registry r;
  r.set("sample_int", 42, true);
  r.set("sample_string", "example", true);
  QVERIFY(r.get("sample_int").toInt() == 42);
  QVERIFY(r.get("sample_string").toString() == "example");
  QVERIFY_THROWS_EXCEPTION(ReinitializePropertyException,
                           r.set("sample_int", 44, true));
}

void TestRegistry::stick() {
  Registry r;
  r.set("sample_string", "example", true);
  QVERIFY(r.get("sample_string").toString() == "example");
  r.stick("sample_string");
  r.set("sample_string", "other");
  QVERIFY(r.get("sample_string").toString() == "example");
  r.set("sample_int", 42, true);
  r.stick("sample_int", 13);
  QVERIFY(r.get("sample_int").toInt() == 42);
  r.set("sample_int", 13);
  QVERIFY(r.get("sample_int").toInt() == 13);
  r.set("sample_int", 42);
  QVERIFY(r.get("sample_int").toInt() == 13);
  r.unstick("sample_int");
  r.stick("sample_int");
  r.set("sample_int", 17);
  QVERIFY(r.get("sample_int").toInt() == 13);
}

void TestRegistry::setAndStick() {
  Registry r;
  r.set("sample_property", "example", true);
  r.setAndStick("sample_property", "value");
  QVERIFY(r.get("sample_property").toString() == "value");
  r.set("sample_property", "example");
  QVERIFY(r.get("sample_property").toString() == "value");
}

void TestRegistry::unstick() {
  Registry r;
  r.set("sample_property", "example", true);
  r.setAndStick("sample_property", "value");
  QVERIFY(r.get("sample_property").toString() == "value");
  r.set("sample_property", "example");
  QVERIFY(r.get("sample_property").toString() == "value");
  r.unstick(("sample_property"));
  r.set("sample_property", "example");
  QVERIFY(r.get("sample_property").toString() == "example");
}

void TestRegistry::isStuck() {
  Registry r;
  r.set("sample_property", "example", true);
  r.setAndStick("sample_property", "value");
  QVERIFY(r.isStuck("sample_property"));
  r.set("other_property", "example", true);
  QVERIFY(!r.isStuck("other_property"));
}

void TestRegistry::stuckProperties() {
  Registry r;
  r.set("sample_property", "example", true);
  r.setAndStick("sample_property", "value");
  QVERIFY(r.stuckProperties().contains("sample_property"));
  r.unstick("sample_property");
  QVERIFY(!r.stuckProperties().contains("sample_property"));
}

// #ifdef TESTING
// QTEST_MAIN(TestRegistry)
// #include "testregistry.moc"
// #endif
