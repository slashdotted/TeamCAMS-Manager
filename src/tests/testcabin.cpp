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
#include "testcabin.h"
#include "simulation/components/cabin.h"
#include "testcomponent.h"
#include <QtTest/QtTest>

static void initRegistry(Registry &r);

static void initRegistry(Registry &r) {
  // Initialize some required properties
  r.set("components.cabin.co2.relvalue", 0.45, true);
  r.set("components.cabin.o2.relvalue", 19.75, true);
  r.set("components.cabin.o2", 2000.0, true);
  r.set("components.cabin.n2", 8000.0, true);
  r.set("components.cabin.co2", 50.0, true);
  r.set("components.cabin.humidity", 40.0, true);
  r.set("components.cabin.temperature", 21.0, true);
  r.set("components.cabin.volume", 10000.0, true);
  r.set("components.cabin.reftemperature", 20.5, true);
  r.set("components.cabin.pressure", 1.005, true);
  r.set("components.cabin.gradient", -1, true);
}

TestCabin::TestCabin(QObject *parent) : QObject(parent) {}

void TestCabin::testPushO2() {
  for (QString key : {"o2_feed", "o2_vent", "o2_scrub", "o2_crew"}) {
    Cabin c;
    Registry r;
    initRegistry(r);
    c.push(r, 1.0, key);
    double value = r.get("components.cabin.o2").toDouble();
    QVERIFY(value == 2001.0);
    double temp = r.get("components.cabin.temperature").toDouble();
    QVERIFY(temp == 21.0 - 0.00125);
    c.push(r, -1.0, key);
    value = r.get("components.cabin.o2").toDouble();
    QVERIFY(value == 2000.0);
    temp = r.get("components.cabin.temperature").toDouble();
    QVERIFY(temp == 21.0 - 0.00125);
    c.push(r, -10000.0, key);
    value = r.get("components.cabin.o2").toDouble();
    QVERIFY(value == 0.0);
  }
}

void TestCabin::testPushN2() {
  for (QString key : {"n2_feed", "n2_vent", "n2_scrub", "n2_crew"}) {
    Cabin c;
    Registry r;
    initRegistry(r);
    r.set("components.cabin.n2", 8000.0);
    r.set("components.cabin.temperature", 21.0);
    c.push(r, 1.0, key);
    double value = r.get("components.cabin.n2").toDouble();
    QVERIFY(value == 8001.0);
    double temp = r.get("components.cabin.temperature").toDouble();
    QVERIFY(temp == 21.0 - 0.0005);
    c.push(r, -1.0, key);
    value = r.get("components.cabin.n2").toDouble();
    QVERIFY(value == 8000.0);
    temp = r.get("components.cabin.temperature").toDouble();
    QVERIFY(temp == 21.0 - 0.0005);
    c.push(r, -10000.0, key);
    value = r.get("components.cabin.n2").toDouble();
    QVERIFY(value == 0.0);
  }
}

void TestCabin::testPushCO2() {
  for (QString key : {"co2_feed", "co2_vent", "co2_scrub", "co2_crew"}) {
    Cabin c;
    Registry r;
    initRegistry(r);
    r.set("components.cabin.co2", 50.0);
    c.push(r, 1.0, key);
    double value = r.get("components.cabin.co2").toDouble();
    QVERIFY(value == 51.0);
    c.push(r, -1000.0, key);
    value = r.get("components.cabin.co2").toDouble();
    QVERIFY(value == 0.0);
  }
}

void TestCabin::testPushHumidity() {
  for (QString key : {"humidity", "humidity_crew"}) {
    Cabin c;
    Registry r;
    initRegistry(r);
    r.set("components.cabin.humidity", 40.0);
    r.set("components.cabin.temperature", 21.0);
    c.push(r, -1.0, key);
    double value = r.get("components.cabin.humidity").toDouble();
    QVERIFY(value == 39.0);
    double temp = r.get("components.cabin.temperature").toDouble();
    QVERIFY(temp == 21.0 - 0.05);
    c.push(r, 1.0, key);
    value = r.get("components.cabin.humidity").toDouble();
    QVERIFY(value == 40.0);
    temp = r.get("components.cabin.temperature").toDouble();
    QVERIFY(temp == 21.0 - 0.05);
  }
}

void TestCabin::testPushHeat() {
  for (QString key : {"heater", "heater_crew"}) {
    Cabin c;
    Registry r;
    initRegistry(r);
    r.set("components.cabin.temperature", 21.0);
    c.push(r, 1.0, key);
    double temp = r.get("components.cabin.temperature").toDouble();
    QVERIFY(temp == 22.0);
  }
}

void TestCabin::testPushCold() {
  for (QString key : {"cooler"}) {
    Cabin c;
    Registry r;
    initRegistry(r);
    r.set("components.cabin.temperature", 21.0);
    c.push(r, -1.0, key);
    double temp = r.get("components.cabin.temperature").toDouble();
    QVERIFY(temp == 20.0);
  }
}
