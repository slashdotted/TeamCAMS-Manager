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
#include "control.h"
#include <QDebug>

struct Control::pimpl {
  pimpl()
      : m_o2_sensor{"sensors.o2", "components.cabin.o2.relvalue",
                    "sensors.o2.upper", "sensors.o2.lower"},
        m_pressure_sensor{"sensors.pressure", "components.cabin.pressure",
                          "sensors.pressure.upper", "sensors.pressure.lower"},
        m_ventilation_sensor{"sensors.ventilation", "components.cabin.pressure",
                             "sensors.ventilation.upper",
                             "sensors.ventilation.lower"},
        m_heater_sensor{"sensors.heat", "components.cabin.temperature",
                        "sensors.heat.upper", "sensors.heat.lower"},
        m_cooler_sensor{"sensors.cold", "components.cabin.temperature",
                        "sensors.cold.upper", "sensors.cold.lower"},
        m_humidity_sensor{"sensors.humidity", "components.cabin.humidity",
                          "sensors.humidity.upper", "sensors.humidity.lower"},
        m_co2_sensor{"sensors.co2", "components.cabin.co2.relvalue",
                     "sensors.co2.upper", "sensors.co2.lower"} {}
  Sensor m_o2_sensor;
  Sensor m_pressure_sensor;
  Sensor m_ventilation_sensor;
  Sensor m_heater_sensor;
  Sensor m_cooler_sensor;
  Sensor m_humidity_sensor;
  Sensor m_co2_sensor;
};

Control::Control() : m_pimpl{std::make_unique<pimpl>()} {}

Control::~Control() {}

void Control::controlOxygenFlow(Registry &s) {
  // m_o2_sensor->update(s);
  if (s.get("control.o2.state").toString() == "auto") {
    s.set("components.o2tank.open", true);
    if (s.get("sensors.o2.value").toString() == "low") {
      // s.set("components.o2tank.open", true);
      s.set("components.o2valve.open", true);
    } else if (s.get("sensors.o2.value").toString() == "high") {
      s.set("components.o2valve.open", false);
      // s.set("components.o2tank.open", false);
    }
  } else if (s.get("control.o2.state").toString() != "assisted") {
    if (s.get("control.o2.state").toString() == "on") {
      s.set("components.o2valve.open", true);
      s.set("components.o2tank.open", true);
    } else {
      s.set("components.o2valve.open", false);
      s.set("components.o2tank.open", false);
    }
  }
}

void Control::controlNitrogenAndVentilation(Registry &s) {
  // m_pressure_sensor->update(s);
  if (s.get("control.pressure.state").toString() == "auto") {
    s.set("components.n2tank.open", true);
    if (s.get("sensors.pressure.value").toString() == "low") {
      s.set("components.n2valve.open", true);
      // s.set("components.n2tank.open", true);
    } else if (s.get("sensors.pressure.value").toString() == "high") {
      s.set("components.n2valve.open", false);
      // s.set("components.n2tank.open", false);
    }
  } else if (s.get("control.pressure.state").toString() != "assisted") {
    if (s.get("control.pressure.state").toString() == "on") {
      s.set("components.n2valve.open", true);
      s.set("components.n2tank.open", true);
    } else {
      s.set("components.n2valve.open", false);
      s.set("components.n2tank.open", false);
    }
  }

  // Ventilation control
  if (s.get("control.ventilation.state").toString() == "auto") {
    if (s.get("sensors.ventilation.value").toString() == "high") {
      s.set("components.vent.running", true);
    } else if (s.get("sensors.ventilation.value").toString() == "low") {
      s.set("components.vent.running", false);
    }
  } else if (s.get("control.ventilation.state").toString() != "assisted") {
    if (s.get("control.ventilation.state").toString() == "on") {
      s.set("components.vent.running", true);
    } else {
      s.set("components.vent.running", false);
    }
  }
}

void Control::controlTemperature(Registry &s) {
  // m_cooler_sensor->update(s);
  //  m_heater_sensor->update(s);
  if (s.get("control.temperature.state").toString() == "auto") {
    if (s.get("sensors.cold.value").toString() == "high") {
      s.set("components.cooler.running", true);
    } else if (s.get("sensors.cold.value") == "low") {
      s.set("components.cooler.running", false);
    }
    if (s.get("sensors.heat.value").toString() == "low") {
      s.set("components.heater.running", true);
    } else if (s.get("sensors.heat.value") == "high") {
      s.set("components.heater.running", false);
    }
  } else if (s.get("control.temperature.state").toString() != "assisted") {
    if (s.get("control.temperature.state").toString() == "heater") {
      s.set("components.cooler.running", false);
      s.set("components.heater.running", true);
    } else if (s.get("control.temperature.state").toString() == "cooler") {
      s.set("components.cooler.running", true);
      s.set("components.heater.running", false);
    } else {
      s.set("components.cooler.running", false);
      s.set("components.heater.running", false);
    }
  }
}

void Control::controlHumidity(Registry &s) {
  if (s.get("control.humidity.state").toString() == "auto") {
    if (s.get("sensors.humidity.value").toString() == "high") {
      s.set("components.dehumidifier.running", true);
    } else if (s.get("sensors.humidity.value").toString() == "low") {
      s.set("components.dehumidifier.running", false);
    }
  } else if (s.get("control.humidity.state").toString() != "assisted") {
    if (s.get("control.humidity.state").toString() == "on") {
      s.set("components.dehumidifier.running", true);
    } else {
      s.set("components.dehumidifier.running", false);
    }
  }
}

void Control::controlCarbonDioxide(Registry &s) {
  if (s.get("control.scrubber.state").toString() == "auto") {
    if (s.get("sensors.co2.value").toString() == "high") {
      s.set("components.scrubber.running", true);
    } else if (s.get("sensors.co2.value").toString() == "low") {
      s.set("components.scrubber.running", false);
    }
  } else if (s.get("control.scrubber.state").toString() != "assisted") {
    if (s.get("control.scrubber.state").toString() == "on") {
      s.set("components.scrubber.running", true);
    } else {
      s.set("components.scrubber.running", false);
    }
  }
}

void Control::adjustCoolerFactor(Registry &s) {
  double factor{0.0};
  factor = s.get("components.cooler.factor").toDouble();
  if (s.get("components.cooler.running").toBool()) {
    factor += 0.03;
  } else {
    factor -= 0.03;
  }
  factor = qMax(qMin(factor, 1.0), 0.0);
  s.set("components.cooler.factor", factor);
}

void Control::adjustHeaterFactor(Registry &s) {
  double factor = s.get("components.heater.factor").toDouble();
  if (s.get("components.heater.running").toBool()) {
    factor += 0.03;
  } else {
    factor -= 0.03;
  }
  factor = qMax(qMin(factor, 1.0), 0.0);
  s.set("components.heater.factor", factor);
}

void Control::adjustDehumidifierFactor(Registry &s) {
  double factor = s.get("components.dehumidifier.factor").toDouble();
  if (s.get("components.dehumidifier.running").toBool()) {
    factor += 0.03;
  } else {
    factor -= 0.04;
  }
  factor = qMax(qMin(factor, 1.0), 0.0);
  s.set("components.dehumidifier.factor", factor);
}

void Control::update(Registry &s) {
  updateSensorReadings(s);
  controlOxygenFlow(s);
  controlNitrogenAndVentilation(s);
  controlTemperature(s);
  controlHumidity(s);
  controlCarbonDioxide(s);
  adjustDehumidifierFactor(s);
  adjustCoolerFactor(s);
  adjustHeaterFactor(s);
}

void Control::updateSensorReadings(Registry &s) {
  m_pimpl->m_co2_sensor.update(s);
  m_pimpl->m_o2_sensor.update(s);
  m_pimpl->m_pressure_sensor.update(s);
  m_pimpl->m_heater_sensor.update(s);
  m_pimpl->m_cooler_sensor.update(s);
  m_pimpl->m_ventilation_sensor.update(s);
  m_pimpl->m_humidity_sensor.update(s);
}
