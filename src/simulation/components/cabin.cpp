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
#include "cabin.h"

static double updatePressure(Registry &s);

void Cabin::pull(Registry &s, double amount, const QString &flow) {
  Q_UNUSED(s)
  Q_UNUSED(amount)
  Q_UNUSED(flow)
  Q_UNREACHABLE();
}

void Cabin::push(Registry &s, double amount, const QString &flow) {
  if (flow == "o2_feed" || flow == "o2_vent" || flow == "o2_scrub" ||
      flow == "o2_crew") {
    double o2{s.get("components.cabin.o2").toDouble()};
    s.set("components.cabin.o2", qMax(o2 + amount, 0.0));
    if (amount > 0) {
      push(s, -1.0 * (0.02 * amount / 16.0), "cooler_o2");
    }
  } else if (flow == "n2_feed" || flow == "n2_vent" || flow == "n2_scrub" ||
             flow == "n2_crew") {
    double n2{s.get("components.cabin.n2").toDouble()};
    s.set("components.cabin.n2", qMax(n2 + amount, 0.0));
    if (amount > 0) {
      push(s, -1.0 * (0.02 * amount / 40.0), "cooler_n2");
    }
  } else if (flow == "co2_feed" || flow == "co2_vent" || flow == "co2_scrub" ||
             flow == "co2_crew") {
    double co2{s.get("components.cabin.co2").toDouble()};
    s.set("components.cabin.co2", qMax(co2 + amount, 0.0));
  } else if (flow == "humidity" || flow == "humidity_crew") {
    double humidity{s.get("components.cabin.humidity").toDouble()};
    s.set("components.cabin.humidity", qMax(humidity + amount, 0.0));
    if (amount < 0) {
      push(s, 0.005 * amount * 10.0, "cooler_dehumidifier");
    }
  } else if (flow == "heater" || flow == "heater_crew") {
    double temperature{s.get("components.cabin.temperature").toDouble()};
    s.set("components.cabin.temperature", qMax(temperature + amount, -273.15));
    if (amount < 0.0) {
      Q_UNREACHABLE(); // Heater is supposed to increase temp
    }
  } else if (flow == "cooler" || flow == "cooler_o2" || flow == "cooler_n2" ||
             flow == "cooler_dehumidifier") {
    double temperature{s.get("components.cabin.temperature").toDouble()};
    s.set("components.cabin.temperature", qMax(temperature + amount, -273.15));
    if (amount > 0.0) {
      Q_UNREACHABLE(); // Cooler is supposed to decrease temp
    }
  } else {
    Q_UNREACHABLE();
  }
  updatePressure(s);
}

void Cabin::update(Registry &s) {
  double o2{s.get("components.cabin.o2").toDouble()};
  double n2{s.get("components.cabin.n2").toDouble()};
  double co2{s.get("components.cabin.co2").toDouble()};
  feedGas(s);
  ventilate(s, o2, n2, co2);
  regulateTemp(s);
  // updatePressure(s);
  regulateHumidity(s);
  scrub(s, o2, n2, co2);

  // Limit the maximum scrubbing/venting capacity
  // double max_o2{s.get("sensors.o2.upper").toDouble() *
  // s.get("components.cabin.volume").toDouble() / 100.0}; double
  // max_co2{s.get("sensors.co2.upper").toDouble() *
  // s.get("components.cabin.volume").toDouble() / 100.0}; double
  // max_n2{s.get("components.cabin.volume").toDouble()- max_o2 - max_co2};

  // ventilate(s, qMin(o2, max_o2 *0.002), qMin(n2, max_n2 *0.002), qMin(co2,
  // max_co2 *0.002)); scrub(s, qMin(o2, max_o2*0.001), qMin(n2, max_n2*0.001),
  // qMin(co2, max_co2 *0.002)); updatePressure(s);
}

void Cabin::regulateTemp(Registry &s) {
  pullFromSources(s, 0, "heater");
  pullFromSources(s, 0, "cooler");
}

void Cabin::regulateHumidity(Registry &s) { pullFromSources(s, 0, "humidity"); }

void Cabin::feedGas(Registry &s) {
  // double gradient = updatePressure(s);

  /*double o2{s.get("components.cabin.o2").toDouble()};
  double n2{s.get("components.cabin.n2").toDouble()};
  double co2{s.get("components.cabin.co2").toDouble()};
  double volume{s.get("components.cabin.volume").toDouble()};
  double temperature{s.get("components.cabin.temperature").toDouble()};
  double refTemperature{s.get("components.cabin.reftemperature").toDouble()};
  double pressure = ((co2 + n2 + o2) / volume) * (1.0 + (temperature -
  refTemperature) / 100.0); double gradient = qMax((1.1 - pressure) / 0.2,
  0.0);*/
  double gradient = s.get("components.cabin.gradient").toDouble();
  pullFromSources(s, gradient, "o2_feed");
  pullFromSources(s, gradient, "n2_feed");
}

static double updatePressure(Registry &s) {
  double o2{s.get("components.cabin.o2").toDouble()};
  double n2{s.get("components.cabin.n2").toDouble()};
  double co2{s.get("components.cabin.co2").toDouble()};
  double volume{s.get("components.cabin.volume").toDouble()};
  double temperature{s.get("components.cabin.temperature").toDouble()};
  double refTemperature{s.get("components.cabin.reftemperature").toDouble()};
  double pressure = ((co2 + n2 + o2) / volume) *
                    (1.0 + (temperature - refTemperature) / 100.0);
  s.set("components.cabin.pressure", pressure);
  double gradient = qMax((1.1 - pressure) / 0.2, 0.0);
  s.set("components.cabin.gradient", gradient);
  double totalgas = o2 + n2 + co2;
  s.set("components.cabin.o2.relvalue", o2 / totalgas * 100.0);
  s.set("components.cabin.co2.relvalue", co2 / totalgas * 100.0);
  // s.set("components.cabin.o2.relvalue", o2 / volume * 100.0);
  // s.set("components.cabin.co2.relvalue", co2 / volume * 100.0);
  return gradient;
}

void Cabin::ventilate(Registry &s, double o2, double n2, double co2) {
  pullFromSources(s, o2, "o2_vent");
  pullFromSources(s, n2, "n2_vent");
  pullFromSources(s, co2, "co2_vent");
}

void Cabin::scrub(Registry &s, double o2, double n2, double co2) {
  pullFromSources(s, o2, "o2_scrub");
  pullFromSources(s, n2, "n2_scrub");
  pullFromSources(s, co2, "co2_scrub");
}
