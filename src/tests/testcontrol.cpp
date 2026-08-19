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
#include "testcontrol.h"
#include "simulation/control/control.h"
#include <QtTest/QtTest>

TestControl::TestControl(QObject *parent) : QObject(parent) {}

void TestControl::testControlOxygenFlow() {
  Control c;
  Registry r;
  r.set("control.o2.state", "auto", true);
  r.set("components.o2tank.open", true, true);
  r.set("sensors.o2.value", "low", true);
  r.set("components.o2valve.open", false, true);
  c.controlOxygenFlow(r);
  QVERIFY(r.get("components.o2valve.open").toBool());
  r.set("sensors.o2.value", "high");
  c.controlOxygenFlow(r);
  QVERIFY(!r.get("components.o2valve.open").toBool());
  r.set("control.o2.state", "on");
  r.set("sensors.o2.value", "high");
  c.controlOxygenFlow(r);
  QVERIFY(r.get("components.o2valve.open").toBool());
  r.set("sensors.o2.value", "low");
  c.controlOxygenFlow(r);
  QVERIFY(r.get("components.o2valve.open").toBool());
  r.set("control.o2.state", "off");
  r.set("sensors.o2.value", "high");
  c.controlOxygenFlow(r);
  QVERIFY(!r.get("components.o2valve.open").toBool());
  r.set("sensors.o2.value", "low");
  c.controlOxygenFlow(r);
  QVERIFY(!r.get("components.o2valve.open").toBool());
  r.set("control.o2.state", "assisted");
  r.set("components.o2valve.open", false);
  r.set("sensors.o2.value", "low");
  c.controlOxygenFlow(r);
  QVERIFY(!r.get("components.o2valve.open").toBool());
  r.set("sensors.o2.value", "high");
  c.controlOxygenFlow(r);
  QVERIFY(!r.get("components.o2valve.open").toBool());
}

void TestControl::testControlNitrogenAndVentilation() {
  Control c;
  Registry r;
  r.set("control.pressure.state", "auto", true);
  r.set("control.ventilation.state", "auto", true);
  r.set("components.n2tank.open", true, true);
  r.set("sensors.ventilation.value", "normal", true);
  r.set("sensors.pressure.value", "low", true);
  r.set("components.vent.running", false, true);
  r.set("components.n2valve.open", false, true);
  c.controlNitrogenAndVentilation(r);
  QVERIFY(r.get("components.n2valve.open").toBool());
  r.set("sensors.pressure.value", "high");
  c.controlNitrogenAndVentilation(r);
  QVERIFY(!r.get("components.n2valve.open").toBool());
  r.set("control.pressure.state", "on");
  r.set("sensors.pressure.value", "high");
  c.controlNitrogenAndVentilation(r);
  QVERIFY(r.get("components.n2valve.open").toBool());
  r.set("sensors.pressure.value", "low");
  c.controlNitrogenAndVentilation(r);
  QVERIFY(r.get("components.n2valve.open").toBool());
  r.set("control.pressure.state", "off");
  r.set("sensors.pressure.value", "high");
  c.controlNitrogenAndVentilation(r);
  QVERIFY(!r.get("components.n2valve.open").toBool());
  r.set("sensors.pressure.value", "low");
  c.controlNitrogenAndVentilation(r);
  QVERIFY(!r.get("components.n2valve.open").toBool());
  r.set("control.pressure.state", "assisted");
  r.set("components.n2valve.open", false);
  r.set("sensors.pressure.value", "low");
  c.controlNitrogenAndVentilation(r);
  QVERIFY(!r.get("components.n2valve.open").toBool());
  r.set("sensors.pressure.value", "high");
  c.controlNitrogenAndVentilation(r);
  QVERIFY(!r.get("components.n2valve.open").toBool());
  r.set("sensors.ventilation.value", "high");
  c.controlNitrogenAndVentilation(r);
  QVERIFY(r.get("components.vent.running").toBool());
  r.set("sensors.ventilation.value", "low");
  c.controlNitrogenAndVentilation(r);
  QVERIFY(!r.get("components.vent.running").toBool());
  r.set("control.ventilation.state", "on");
  r.set("sensors.pressure.value", "high");
  c.controlNitrogenAndVentilation(r);
  QVERIFY(r.get("components.vent.running").toBool());
  r.set("sensors.pressure.value", "low");
  c.controlNitrogenAndVentilation(r);
  QVERIFY(r.get("components.vent.running").toBool());
  r.set("control.ventilation.state", "off");
  r.set("sensors.pressure.value", "high");
  c.controlNitrogenAndVentilation(r);
  QVERIFY(!r.get("components.vent.running").toBool());
  r.set("sensors.pressure.value", "low");
  c.controlNitrogenAndVentilation(r);
  QVERIFY(!r.get("components.vent.running").toBool());
  r.set("control.ventilation.state", "assisted");
  r.set("components.vent.running", false);
  r.set("sensors.pressure.value", "low");
  c.controlNitrogenAndVentilation(r);
  QVERIFY(!r.get("components.vent.running").toBool());
  r.set("sensors.pressure.value", "high");
  c.controlNitrogenAndVentilation(r);
  QVERIFY(!r.get("components.vent.running").toBool());
}

void TestControl::testControlTemperature() {
  Control c;
  Registry r;
  r.set("control.temperature.state", "auto", true);
  r.set("sensors.cold.value", "high", true);
  r.set("components.cooler.running", false, true);
  r.set("sensors.heat.value", "high", true);
  r.set("components.heater.running", false, true);
  c.controlTemperature(r);
  QVERIFY(r.get("components.cooler.running").toBool());
  QVERIFY(!r.get("components.heater.running").toBool());
  r.set("sensors.cold.value", "low");
  c.controlTemperature(r);
  QVERIFY(!r.get("components.cooler.running").toBool());
  QVERIFY(!r.get("components.heater.running").toBool());
  r.set("sensors.heat.value", "low");
  c.controlTemperature(r);
  QVERIFY(!r.get("components.cooler.running").toBool());
  QVERIFY(r.get("components.heater.running").toBool());
  r.set("control.temperature.state", "heater");
  r.set("sensors.heat.value", "high");
  r.set("sensors.cold.value", "high");
  c.controlTemperature(r);
  QVERIFY(r.get("components.heater.running").toBool());
  QVERIFY(!r.get("components.cooler.running").toBool());
  r.set("control.temperature.state", "cooler");
  r.set("sensors.heat.value", "low");
  r.set("sensors.cold.value", "low");
  c.controlTemperature(r);
  QVERIFY(!r.get("components.heater.running").toBool());
  QVERIFY(r.get("components.cooler.running").toBool());
}

void TestControl::testControlHumidity() {
  Control c;
  Registry r;
  r.set("control.humidity.state", "auto", true);
  r.set("sensors.humidity.value", "high", true);
  r.set("components.dehumidifier.running", false, true);
  c.controlHumidity(r);
  QVERIFY(r.get("components.dehumidifier.running").toBool());
  r.set("sensors.humidity.value", "low");
  c.controlHumidity(r);
  QVERIFY(!r.get("components.dehumidifier.running").toBool());
  r.set("control.humidity.state", "on");
  r.set("sensors.humidity.value", "low");
  c.controlHumidity(r);
  QVERIFY(r.get("components.dehumidifier.running").toBool());
  r.set("sensors.humidity.value", "high");
  c.controlHumidity(r);
  QVERIFY(r.get("components.dehumidifier.running").toBool());
  r.set("control.humidity.state", "off");
  r.set("sensors.humidity.value", "low");
  c.controlHumidity(r);
  QVERIFY(!r.get("components.dehumidifier.running").toBool());
  r.set("sensors.humidity.value", "high");
  c.controlHumidity(r);
  QVERIFY(!r.get("components.dehumidifier.running").toBool());
  r.set("control.humidity.state", "assisted");
  c.controlHumidity(r);
  QVERIFY(!r.get("components.dehumidifier.running").toBool());
}

void TestControl::testControlCarbonDioxide() {
  Control c;
  Registry r;
  r.set("control.scrubber.state", "auto", true);
  r.set("sensors.co2.value", "high", true);
  r.set("components.scrubber.running", false, true);
  c.controlCarbonDioxide(r);
  QVERIFY(r.get("components.scrubber.running").toBool());
  r.set("sensors.co2.value", "low");
  c.controlCarbonDioxide(r);
  QVERIFY(!r.get("components.scrubber.running").toBool());
  r.set("control.scrubber.state", "on");
  r.set("sensors.co2.value", "high");
  c.controlCarbonDioxide(r);
  QVERIFY(r.get("components.scrubber.running").toBool());
  r.set("sensors.co2.value", "low");
  c.controlCarbonDioxide(r);
  QVERIFY(r.get("components.scrubber.running").toBool());
  r.set("control.scrubber.state", "off");
  r.set("sensors.co2.value", "high");
  c.controlCarbonDioxide(r);
  QVERIFY(!r.get("components.scrubber.running").toBool());
  r.set("sensors.co2.value", "low");
  c.controlCarbonDioxide(r);
  QVERIFY(!r.get("components.scrubber.running").toBool());
  r.set("control.scrubber.state", "assisted");
  r.set("sensors.co2.value", "high");
  c.controlCarbonDioxide(r);
  QVERIFY(!r.get("components.scrubber.running").toBool());
}

void TestControl::testAdjustCoolerFactor() {
  Control c;
  Registry r;
  r.set("components.cooler.factor", 0.5, true);
  r.set("components.cooler.running", true, true);
  c.adjustCoolerFactor(r);
  double value{r.get("components.cooler.factor").toDouble()};
  QVERIFY(value == 0.53);
  r.set("components.cooler.factor", 0.5);
  r.set("components.cooler.running", false);
  c.adjustCoolerFactor(r);
  value = r.get("components.cooler.factor").toDouble();
  QVERIFY(value == 0.47);
  r.set("components.cooler.factor", 0.01);
  r.set("components.cooler.running", false);
  c.adjustCoolerFactor(r);
  value = r.get("components.cooler.factor").toDouble();
  QVERIFY(value == 0.0);
  r.set("components.cooler.factor", 0.98);
  r.set("components.cooler.running", true);
  c.adjustCoolerFactor(r);
  value = r.get("components.cooler.factor").toDouble();
  QVERIFY(value == 1.0);
}

void TestControl::testAdjustHeaterFactor() {
  Control c;
  Registry r;
  r.set("components.heater.factor", 0.5, true);
  r.set("components.heater.running", true, true);
  c.adjustHeaterFactor(r);
  double value{r.get("components.heater.factor").toDouble()};
  QVERIFY(value == 0.53);
  r.set("components.heater.factor", 0.5);
  r.set("components.heater.running", false);
  c.adjustHeaterFactor(r);
  value = r.get("components.heater.factor").toDouble();
  QVERIFY(value == 0.47);
  r.set("components.heater.factor", 0.01);
  r.set("components.heater.running", false);
  c.adjustHeaterFactor(r);
  value = r.get("components.heater.factor").toDouble();
  QVERIFY(value == 0.0);
  r.set("components.heater.factor", 0.98);
  r.set("components.heater.running", true);
  c.adjustHeaterFactor(r);
  value = r.get("components.heater.factor").toDouble();
  QVERIFY(value == 1.0);
}

void TestControl::testAdjustDehumidifierFactor() {
  Control c;
  Registry r;
  r.set("components.dehumidifier.factor", 0.5, true);
  r.set("components.dehumidifier.running", true, true);
  c.adjustDehumidifierFactor(r);
  double value{r.get("components.dehumidifier.factor").toDouble()};
  QVERIFY(value == 0.53);
  r.set("components.dehumidifier.factor", 0.5);
  r.set("components.dehumidifier.running", false);
  c.adjustDehumidifierFactor(r);
  value = r.get("components.dehumidifier.factor").toDouble();
  QVERIFY(value == 0.46);
  r.set("components.dehumidifier.factor", 0.01);
  r.set("components.dehumidifier.running", false);
  c.adjustDehumidifierFactor(r);
  value = r.get("components.dehumidifier.factor").toDouble();
  QVERIFY(value == 0.0);
  r.set("components.dehumidifier.factor", 0.98);
  r.set("components.dehumidifier.running", true);
  c.adjustDehumidifierFactor(r);
  value = r.get("components.dehumidifier.factor").toDouble();
  QVERIFY(value == 1.0);
}
