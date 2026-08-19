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
#include "simulator.h"
#include <QDebug>

struct Simulator::pimpl {
  pimpl(Simulator *parent)
      : m_o2tank_device{"o2tank"}, m_o2pipe_1_device{"o2pipetankvalve"},
        m_o2pipe_2_device{"o2pipevalvemixer"},
        m_o2pipe_3_device{"o2pipemixercabin"}, m_o2valve_device{"o2valve"},
        m_n2tank_device{"n2tank"}, m_n2pipe_1_device{"n2pipetankvalve"},
        m_n2pipe_2_device{"n2pipevalvemixer"},
        m_n2pipe_3_device{"n2pipemixercabin"}, m_n2valve_device{"n2valve"},
        m_registry{new Registry{parent}} {}

  Tank m_o2tank_device;
  Pipe m_o2pipe_1_device;
  Pipe m_o2pipe_2_device;
  Pipe m_o2pipe_3_device;
  Valve m_o2valve_device;

  Tank m_n2tank_device;
  Pipe m_n2pipe_1_device;
  Pipe m_n2pipe_2_device;
  Pipe m_n2pipe_3_device;
  Valve m_n2valve_device;

  Mixer m_mixer_device;
  Cabin m_cabin_device;
  Control m_control;
  Crew m_crew;

  Heater m_heater;
  Cooler m_cooler;
  Scrubber m_scrubber;
  Vent m_vent;
  Dehumidifier m_dehumidifier;

  Registry *m_registry;
};

Simulator::Simulator(QObject *parent)
    : QObject{parent}, m_pimpl{new pimpl{this}}

{
  // Oxygen feed flow
  m_pimpl->m_o2tank_device.connectSink(&m_pimpl->m_o2pipe_1_device, "o2_feed")
      ->connectSink(&m_pimpl->m_o2valve_device, "o2_feed")
      ->connectSink(&m_pimpl->m_o2pipe_2_device, "o2_feed")
      ->connectSink(&m_pimpl->m_mixer_device, "o2_feed")
      ->connectSink(&m_pimpl->m_o2pipe_3_device, "o2_feed")
      ->connectSink(&m_pimpl->m_cabin_device, "o2_feed");

  // Nitrogen feed flow
  m_pimpl->m_n2tank_device.connectSink(&m_pimpl->m_n2pipe_1_device, "n2_feed")
      ->connectSink(&m_pimpl->m_n2valve_device, "n2_feed")
      ->connectSink(&m_pimpl->m_n2pipe_2_device, "n2_feed")
      ->connectSink(&m_pimpl->m_mixer_device, "n2_feed")
      ->connectSink(&m_pimpl->m_n2pipe_3_device, "n2_feed")
      ->connectSink(&m_pimpl->m_cabin_device, "n2_feed");

  // Temperature flows
  m_pimpl->m_heater.connectSink(&m_pimpl->m_cabin_device, "heater");
  m_pimpl->m_cooler.connectSink(&m_pimpl->m_cabin_device, "cooler");

  // Ventilation flows
  m_pimpl->m_vent.connectSink(&m_pimpl->m_cabin_device, "o2_vent");
  m_pimpl->m_vent.connectSink(&m_pimpl->m_cabin_device, "n2_vent");
  m_pimpl->m_vent.connectSink(&m_pimpl->m_cabin_device, "co2_vent");
  m_pimpl->m_dehumidifier.connectSink(&m_pimpl->m_cabin_device, "humidity");

  // Scrubbing flows
  m_pimpl->m_scrubber.connectSink(&m_pimpl->m_cabin_device, "o2_scrub");
  m_pimpl->m_scrubber.connectSink(&m_pimpl->m_cabin_device, "n2_scrub");
  m_pimpl->m_scrubber.connectSink(&m_pimpl->m_cabin_device, "co2_scrub");

  // Crew
  m_pimpl->m_crew.connectSink(&m_pimpl->m_cabin_device, "o2_crew");
  m_pimpl->m_crew.connectSink(&m_pimpl->m_cabin_device, "co2_crew");
  m_pimpl->m_crew.connectSink(&m_pimpl->m_cabin_device, "heater_crew");
  m_pimpl->m_crew.connectSink(&m_pimpl->m_cabin_device, "humidity_crew");
}

Simulator::~Simulator() = default;

void Simulator::update(unsigned int timestamp) {
  m_pimpl->m_registry->set("system.timestamp", static_cast<double>(timestamp));
  m_pimpl->m_crew.update(*m_pimpl->m_registry);
  // m_pimpl->m_cabin_device.updatePressure(*m_pimpl->m_registry);
  m_pimpl->m_control.update(*m_pimpl->m_registry);
  m_pimpl->m_cabin_device.update(*m_pimpl->m_registry);
  // m_pimpl->m_cabin_device.updatePressure(*m_pimpl->m_registry);
}

Registry *Simulator::registry() { return m_pimpl->m_registry; }

void Simulator::reset() { m_pimpl->m_registry->reset(); }

void Simulator::initialize() {
  m_pimpl->m_registry->set("system.timestamp", 0, true);
  m_pimpl->m_registry->set("control.o2.state", "auto", true);
  m_pimpl->m_registry->set("control.pressure.state", "auto", true);
  m_pimpl->m_registry->set("control.ventilation.state", "auto", true);
  m_pimpl->m_registry->set("control.temperature.state", "auto", true);
  m_pimpl->m_registry->set("control.humidity.state", "auto", true);
  m_pimpl->m_registry->set("control.scrubber.state", "auto", true);
  m_pimpl->m_registry->set("sensors.o2.upper", 20.0, true);
  m_pimpl->m_registry->set("sensors.o2.lower", 19.6, true);
  m_pimpl->m_registry->set("sensors.o2.failure", false, true);
  m_pimpl->m_registry->set("sensors.o2.upper.failure", false, true);
  m_pimpl->m_registry->set("sensors.o2.lower.failure", false, true);
  m_pimpl->m_registry->set("sensors.o2.value", "normal", true);
  m_pimpl->m_registry->set("sensors.pressure.upper", 1.02, true);
  m_pimpl->m_registry->set("sensors.pressure.lower", 0.992, true);
  m_pimpl->m_registry->set("sensors.pressure.failure", false, true);
  m_pimpl->m_registry->set("sensors.pressure.upper.failure", false, true);
  m_pimpl->m_registry->set("sensors.pressure.lower.failure", false, true);
  m_pimpl->m_registry->set("sensors.pressure.value", "normal", true);
  m_pimpl->m_registry->set("sensors.ventilation.upper", 1.022, true);
  m_pimpl->m_registry->set("sensors.ventilation.lower", 1.018, true);
  m_pimpl->m_registry->set("sensors.ventilation.failure", false, true);
  m_pimpl->m_registry->set("sensors.ventilation.upper.failure", false, true);
  m_pimpl->m_registry->set("sensors.ventilation.lower.failure", false, true);
  m_pimpl->m_registry->set("sensors.ventilation.value", "normal", true);
  m_pimpl->m_registry->set("sensors.heat.upper", 20.2, true);
  m_pimpl->m_registry->set("sensors.heat.lower", 19.8, true);
  m_pimpl->m_registry->set("sensors.heat.failure", false, true);
  m_pimpl->m_registry->set("sensors.heat.upper.failure", false, true);
  m_pimpl->m_registry->set("sensors.heat.lower.failure", false, true);
  m_pimpl->m_registry->set("sensors.heat.value", "normal", true);
  m_pimpl->m_registry->set("sensors.cold.upper", 21.6, true);
  m_pimpl->m_registry->set("sensors.cold.failure", false, true);
  m_pimpl->m_registry->set("sensors.cold.upper.failure", false, true);
  m_pimpl->m_registry->set("sensors.cold.lower.failure", false, true);
  m_pimpl->m_registry->set("sensors.cold.lower", 19.8, true);
  m_pimpl->m_registry->set("sensors.cold.value", "normal", true);
  m_pimpl->m_registry->set("sensors.humidity.upper", 41.7, true);
  m_pimpl->m_registry->set("sensors.humidity.lower", 40, true);
  m_pimpl->m_registry->set("sensors.humidity.failure", false, true);
  m_pimpl->m_registry->set("sensors.humidity.upper.failure", false, true);
  m_pimpl->m_registry->set("sensors.humidity.lower.failure", false, true);
  m_pimpl->m_registry->set("sensors.humidity.value", "normal", true);
  m_pimpl->m_registry->set("sensors.co2.upper", 0.6, true);
  m_pimpl->m_registry->set("sensors.co2.lower", 0.3, true);
  m_pimpl->m_registry->set("sensors.co2.failure", false, true);
  m_pimpl->m_registry->set("sensors.co2.upper.failure", false, true);
  m_pimpl->m_registry->set("sensors.co2.lower.failure", false, true);
  m_pimpl->m_registry->set("sensors.co2.value", "normal", true);
  m_pimpl->m_registry->set("components.heater.efficiency", 1.0, true);
  m_pimpl->m_registry->set("components.heater.strength", "standard", true);
  m_pimpl->m_registry->set("components.cooler.efficiency", 1.0, true);
  m_pimpl->m_registry->set("components.cooler.strength", "standard", true);
  m_pimpl->m_registry->set("components.dehumidifier.efficiency", 1.0, true);
  m_pimpl->m_registry->set("components.dehumidifier.strength", "standard",
                           true);
  m_pimpl->m_registry->set("components.scrubber.efficiency", 1.0, true);
  m_pimpl->m_registry->set("components.scrubber.strength", "standard", true);
  m_pimpl->m_registry->set("components.vent.efficiency", 1.0, true);
  m_pimpl->m_registry->set("components.vent.strength", "standard", true);
  m_pimpl->m_registry->set("components.vent.percent", 0.004, true);
  m_pimpl->m_registry->set("components.o2tank.infinite", false, true);
  m_pimpl->m_registry->set("components.n2tank.infinite", false, true);
  m_pimpl->m_registry->set("components.o2tank.capacity", 90000, true);
  m_pimpl->m_registry->set("components.n2tank.capacity", 90000, true);
  m_pimpl->m_registry->set("components.o2tank.volume", 90000, true);
  m_pimpl->m_registry->set("components.n2tank.volume", 90000, true);
  m_pimpl->m_registry->set("components.o2pipemixercabin.leak", false, true);
  m_pimpl->m_registry->set("components.mixer.block", false, true);
  m_pimpl->m_registry->set("components.o2pipevalvemixer.leak", false, true);
  m_pimpl->m_registry->set("components.o2pipetankvalve.leak", false, true);
  m_pimpl->m_registry->set("components.o2pipemixercabin.block", false, true);
  m_pimpl->m_registry->set("components.o2pipevalvemixer.block", false, true);
  m_pimpl->m_registry->set("components.o2pipetankvalve.block", false, true);
  m_pimpl->m_registry->set("components.o2tank.open", false, true);
  m_pimpl->m_registry->set("components.o2pipetankvalve.flow", 0.0, true);
  m_pimpl->m_registry->set("components.o2pipevalvemixer.flow", 0.0, true);
  m_pimpl->m_registry->set("components.o2pipemixercabin.flow", 0.0, true);
  m_pimpl->m_registry->set("components.n2pipemixercabin.leak", false, true);
  m_pimpl->m_registry->set("components.n2pipevalvemixer.leak", false, true);
  m_pimpl->m_registry->set("components.n2pipetankvalve.leak", false, true);
  m_pimpl->m_registry->set("components.n2pipemixercabin.block", false, true);
  m_pimpl->m_registry->set("components.n2pipevalvemixer.block", false, true);
  m_pimpl->m_registry->set("components.n2pipetankvalve.block", false, true);
  m_pimpl->m_registry->set("components.n2tank.open", false, true);
  m_pimpl->m_registry->set("components.n2pipetankvalve.flow", 0.0, true);
  m_pimpl->m_registry->set("components.n2pipevalvemixer.flow", 0.0, true);
  m_pimpl->m_registry->set("components.n2pipemixercabin.flow", 0.0, true);
  m_pimpl->m_registry->set("components.o2valve.strength", "standard", true);
  m_pimpl->m_registry->set("components.n2valve.strength", "standard", true);
  m_pimpl->m_registry->set("components.o2valve.block", false, true);
  m_pimpl->m_registry->set("components.o2valve.leak", false, true);
  m_pimpl->m_registry->set("components.n2valve.open", false, true);
  m_pimpl->m_registry->set("components.n2valve.leak", false, true);
  m_pimpl->m_registry->set("components.n2valve.block", false, true);
  m_pimpl->m_registry->set("components.n2valve.block.factor", 0.4, true);
  m_pimpl->m_registry->set("components.o2valve.open", false, true);
  m_pimpl->m_registry->set("components.o2valve.block.factor", 0.4, true);
  m_pimpl->m_registry->set("components.cabin.co2.relvalue", 0.45, true);
  m_pimpl->m_registry->set("components.cabin.o2.relvalue", 19.75, true);
  m_pimpl->m_registry->set("components.cabin.o2", 2000.0, true);
  m_pimpl->m_registry->set("components.cabin.n2", 8000.0, true);
  m_pimpl->m_registry->set("components.cabin.co2", 50.0, true);
  m_pimpl->m_registry->set("components.cabin.humidity", 40.0, true);
  m_pimpl->m_registry->set("components.cabin.temperature", 21.0, true);
  m_pimpl->m_registry->set("components.cabin.volume", 10000.0, true);
  m_pimpl->m_registry->set("components.cabin.reftemperature", 20.5, true);
  m_pimpl->m_registry->set("components.cabin.pressure", 1.005, true);
  m_pimpl->m_registry->set("components.cabin.gradient", -1, true);
  m_pimpl->m_registry->set("components.cooler.running", false, true);
  m_pimpl->m_registry->set("components.heater.running", false, true);
  m_pimpl->m_registry->set("components.vent.running", false, true);
  m_pimpl->m_registry->set("components.scrubber.running", false, true);
  m_pimpl->m_registry->set("components.cooler.factor", 0, true);
  m_pimpl->m_registry->set("components.heater.factor", 0, true);
  m_pimpl->m_registry->set("components.dehumidifier.factor", 0, true);
  m_pimpl->m_registry->set("components.dehumidifier.running", false, true);
  m_pimpl->m_registry->set("components.crew.o2consumption", 0, true);
}
