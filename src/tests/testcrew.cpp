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
#include "testcrew.h"
#include "simulation/components/crew.h"
#include "testcomponent.h"
#include <QtTest/QtTest>

TestCrew::TestCrew(QObject *parent) : QObject(parent) {}

void TestCrew::testUpdate() {
  Crew c;
  Registry r;
  TestComponent tc_o2;
  TestComponent tc_co2;
  TestComponent tc_heat;
  TestComponent tc_humidity;
  r.set("components.crew.o2consumption", 0.0, true);
  c.connectSink(&tc_o2, "o2_crew");
  c.connectSink(&tc_co2, "co2_crew");
  c.connectSink(&tc_heat, "heater_crew");
  c.connectSink(&tc_humidity, "humidity_crew");
  for (int i = 0; i < 100000; i++) {
    c.update(r);
    double value{r.get("components.crew.o2consumption").toDouble()};
    QVERIFY(value <= 4);
    QVERIFY(value >= 2);
    QVERIFY(tc_o2.totalPushed("o2_crew") == (-1.0 * value));
    tc_o2.reset();
    QVERIFY(tc_co2.totalPushed("co2_crew") == value);
    tc_co2.reset();
    QVERIFY(tc_heat.totalPushed("heater_crew") == 0.04);
    tc_heat.reset();
    QVERIFY(tc_humidity.totalPushed("humidity_crew") == 0.05);
    tc_humidity.reset();
  }
}
