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
#include "testsimulator.h"
#include "simulation/simulator.h"
#include <QtTest/QtTest>

TestSimulator::TestSimulator(QObject *parent) : QObject(parent) {}

void TestSimulator::testInitialize() {
  Simulator s;
  s.reset();
  s.initialize();
  QVERIFY(s.registry()->exists("system.timestamp"));
  QVERIFY(s.registry()->exists("control.o2.state"));
  QVERIFY(s.registry()->exists("control.pressure.state"));
  QVERIFY(s.registry()->exists("control.ventilation.state"));
  QVERIFY(s.registry()->exists("control.temperature.state"));
  QVERIFY(s.registry()->exists("control.humidity.state"));
  QVERIFY(s.registry()->exists("control.scrubber.state"));
  QVERIFY(s.registry()->exists("sensors.o2.upper"));
  QVERIFY(s.registry()->exists("sensors.o2.lower"));
  QVERIFY(s.registry()->exists("sensors.o2.failure"));
  QVERIFY(s.registry()->exists("sensors.o2.upper.failure"));
  QVERIFY(s.registry()->exists("sensors.o2.lower.failure"));
  QVERIFY(s.registry()->exists("sensors.o2.value"));
  QVERIFY(s.registry()->exists("sensors.pressure.upper"));
  QVERIFY(s.registry()->exists("sensors.pressure.lower"));
  QVERIFY(s.registry()->exists("sensors.pressure.failure"));
  QVERIFY(s.registry()->exists("sensors.pressure.upper.failure"));
  QVERIFY(s.registry()->exists("sensors.pressure.lower.failure"));
  QVERIFY(s.registry()->exists("sensors.pressure.value"));
  QVERIFY(s.registry()->exists("sensors.ventilation.upper"));
  QVERIFY(s.registry()->exists("sensors.ventilation.lower"));
  QVERIFY(s.registry()->exists("sensors.ventilation.failure"));
  QVERIFY(s.registry()->exists("sensors.ventilation.upper.failure"));
  QVERIFY(s.registry()->exists("sensors.ventilation.lower.failure"));
  QVERIFY(s.registry()->exists("sensors.ventilation.value"));
  QVERIFY(s.registry()->exists("sensors.heat.upper"));
  QVERIFY(s.registry()->exists("sensors.heat.lower"));
  QVERIFY(s.registry()->exists("sensors.heat.failure"));
  QVERIFY(s.registry()->exists("sensors.heat.upper.failure"));
  QVERIFY(s.registry()->exists("sensors.heat.lower.failure"));
  QVERIFY(s.registry()->exists("sensors.heat.value"));
  QVERIFY(s.registry()->exists("sensors.cold.upper"));
  QVERIFY(s.registry()->exists("sensors.cold.failure"));
  QVERIFY(s.registry()->exists("sensors.cold.upper.failure"));
  QVERIFY(s.registry()->exists("sensors.cold.lower.failure"));
  QVERIFY(s.registry()->exists("sensors.cold.lower"));
  QVERIFY(s.registry()->exists("sensors.cold.value"));
  QVERIFY(s.registry()->exists("sensors.humidity.upper"));
  QVERIFY(s.registry()->exists("sensors.humidity.lower"));
  QVERIFY(s.registry()->exists("sensors.humidity.failure"));
  QVERIFY(s.registry()->exists("sensors.humidity.upper.failure"));
  QVERIFY(s.registry()->exists("sensors.humidity.lower.failure"));
  QVERIFY(s.registry()->exists("sensors.humidity.value"));
  QVERIFY(s.registry()->exists("sensors.co2.upper"));
  QVERIFY(s.registry()->exists("sensors.co2.lower"));
  QVERIFY(s.registry()->exists("sensors.co2.failure"));
  QVERIFY(s.registry()->exists("sensors.co2.upper.failure"));
  QVERIFY(s.registry()->exists("sensors.co2.lower.failure"));
  QVERIFY(s.registry()->exists("sensors.co2.value"));
  QVERIFY(s.registry()->exists("components.heater.efficiency"));
  QVERIFY(s.registry()->exists("components.heater.strength"));
  QVERIFY(s.registry()->exists("components.cooler.efficiency"));
  QVERIFY(s.registry()->exists("components.cooler.strength"));
  QVERIFY(s.registry()->exists("components.dehumidifier.efficiency"));
  QVERIFY(s.registry()->exists("components.dehumidifier.strength"));
  QVERIFY(s.registry()->exists("components.scrubber.efficiency"));
  QVERIFY(s.registry()->exists("components.scrubber.strength"));
  QVERIFY(s.registry()->exists("components.vent.efficiency"));
  QVERIFY(s.registry()->exists("components.vent.strength"));
  QVERIFY(s.registry()->exists("components.vent.percent"));
  QVERIFY(s.registry()->exists("components.o2tank.infinite"));
  QVERIFY(s.registry()->exists("components.n2tank.infinite"));
  QVERIFY(s.registry()->exists("components.o2tank.capacity"));
  QVERIFY(s.registry()->exists("components.n2tank.capacity"));
  QVERIFY(s.registry()->exists("components.o2tank.volume"));
  QVERIFY(s.registry()->exists("components.n2tank.volume"));
  QVERIFY(s.registry()->exists("components.o2pipemixercabin.leak"));
  QVERIFY(s.registry()->exists("components.mixer.block"));
  QVERIFY(s.registry()->exists("components.o2pipevalvemixer.leak"));
  QVERIFY(s.registry()->exists("components.o2pipetankvalve.leak"));
  QVERIFY(s.registry()->exists("components.o2pipemixercabin.block"));
  QVERIFY(s.registry()->exists("components.o2pipevalvemixer.block"));
  QVERIFY(s.registry()->exists("components.o2pipetankvalve.block"));
  QVERIFY(s.registry()->exists("components.o2tank.open"));
  QVERIFY(s.registry()->exists("components.o2pipetankvalve.flow"));
  QVERIFY(s.registry()->exists("components.o2pipevalvemixer.flow"));
  QVERIFY(s.registry()->exists("components.o2pipemixercabin.flow"));
  QVERIFY(s.registry()->exists("components.n2pipemixercabin.leak"));
  QVERIFY(s.registry()->exists("components.n2pipevalvemixer.leak"));
  QVERIFY(s.registry()->exists("components.n2pipetankvalve.leak"));
  QVERIFY(s.registry()->exists("components.n2pipemixercabin.block"));
  QVERIFY(s.registry()->exists("components.n2pipevalvemixer.block"));
  QVERIFY(s.registry()->exists("components.n2pipetankvalve.block"));
  QVERIFY(s.registry()->exists("components.n2tank.open"));
  QVERIFY(s.registry()->exists("components.n2pipetankvalve.flow"));
  QVERIFY(s.registry()->exists("components.n2pipevalvemixer.flow"));
  QVERIFY(s.registry()->exists("components.n2pipemixercabin.flow"));
  QVERIFY(s.registry()->exists("components.cabin.co2.relvalue"));
  QVERIFY(s.registry()->exists("components.cabin.o2.relvalue"));
  QVERIFY(s.registry()->exists("components.o2valve.strength"));
  QVERIFY(s.registry()->exists("components.n2valve.strength"));
  QVERIFY(s.registry()->exists("components.o2valve.block"));
  QVERIFY(s.registry()->exists("components.o2valve.leak"));
  QVERIFY(s.registry()->exists("components.n2valve.open"));
  QVERIFY(s.registry()->exists("components.n2valve.leak"));
  QVERIFY(s.registry()->exists("components.n2valve.block"));
  QVERIFY(s.registry()->exists("components.o2valve.open"));
  QVERIFY(s.registry()->exists("components.cabin.o2"));
  QVERIFY(s.registry()->exists("components.cabin.n2"));
  QVERIFY(s.registry()->exists("components.cabin.co2"));
  QVERIFY(s.registry()->exists("components.cabin.humidity"));
  QVERIFY(s.registry()->exists("components.cabin.temperature"));
  QVERIFY(s.registry()->exists("components.cabin.volume"));
  QVERIFY(s.registry()->exists("components.cabin.reftemperature"));
  QVERIFY(s.registry()->exists("components.cabin.pressure"));
  QVERIFY(s.registry()->exists("components.cabin.gradient"));
  QVERIFY(s.registry()->exists("components.cooler.running"));
  QVERIFY(s.registry()->exists("components.heater.running"));
  QVERIFY(s.registry()->exists("components.vent.running"));
  QVERIFY(s.registry()->exists("components.scrubber.running"));
  QVERIFY(s.registry()->exists("components.cooler.factor"));
  QVERIFY(s.registry()->exists("components.heater.factor"));
  QVERIFY(s.registry()->exists("components.dehumidifier.factor"));
  QVERIFY(s.registry()->exists("components.dehumidifier.running"));
  QVERIFY(s.registry()->exists("components.crew.o2consumption"));
}
