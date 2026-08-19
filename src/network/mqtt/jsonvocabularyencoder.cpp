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
#include "jsonvocabularyencoder.h"
#include <QMap>
#include <QString>

const QMap<QString, QString> keyToIdx = {
    {"system.timestamp", "@0"},
    {"components.cabin.co2", "@1"},
    {"components.cabin.co2.relvalue", "@2"},
    {"components.cabin.gradient", "@3"},
    {"components.cabin.humidity", "@4"},
    {"components.cabin.n2", "@5"},
    {"components.cabin.o2", "@6"},
    {"components.cabin.o2.relvalue", "@7"},
    {"components.cabin.pressure", "@8"},
    {"components.cabin.reftemperature", "@9"},
    {"components.cabin.temperature", "@10"},
    {"components.cabin.volume", "@11"},
    {"components.cold.efficiency", "@12"},
    {"components.cooler.efficiency", "@13"},
    {"components.cooler.running", "@14"},
    {"components.cooler.strength", "@15"},
    {"components.dehumidifier.efficiency", "@16"},
    {"components.dehumidifier.strength", "@17"},
    {"components.heater.efficiency", "@18"},
    {"components.heater.running", "@19"},
    {"components.heater.strength", "@20"},
    {"components.mixer.block", "@21"},
    {"components.n2pipemixercabin.block", "@22"},
    {"components.n2pipemixercabin.leak", "@23"},
    {"components.n2pipetankvalve.block", "@24"},
    {"components.n2pipetankvalve.flow", "@25"},
    {"components.n2pipetankvalve.leak", "@26"},
    {"components.n2pipevalvemixer.block", "@27"},
    {"components.n2pipevalvemixer.flow", "@28"},
    {"components.n2pipevalvemixer.leak", "@29"},
    {"components.n2tank.capacity", "@30"},
    {"components.n2tank.infinite", "@31"},
    {"components.n2tank.open", "@32"},
    {"components.n2tank.volume", "@33"},
    {"components.n2valve.block", "@34"},
    {"components.n2valve.block.factor", "@35"},
    {"components.n2valve.leak", "@36"},
    {"components.n2valve.open", "@37"},
    {"components.n2valve.strength", "@38"},
    {"components.o2pipemixercabin.block", "@39"},
    {"components.o2pipemixercabin.flow", "@40"},
    {"components.o2pipemixercabin.leak", "@41"},
    {"components.o2pipetankvalve.block", "@42"},
    {"components.o2pipetankvalve.flow", "@43"},
    {"components.o2pipetankvalve.leak", "@44"},
    {"components.o2pipevalvemixer.block", "@45"},
    {"components.o2pipevalvemixer.flow", "@46"},
    {"components.o2pipevalvemixer.leak", "@47"},
    {"components.o2tank.capacity", "@48"},
    {"components.o2tank.infinite", "@49"},
    {"components.o2tank.open", "@50"},
    {"components.o2tank.volume", "@51"},
    {"components.o2valve.block", "@52"},
    {"components.o2valve.block.factor", "@53"},
    {"components.o2valve.leak", "@54"},
    {"components.o2valve.open", "@55"},
    {"components.o2valve.strength", "@56"},
    {"components.scrubber.efficiency", "@57"},
    {"components.scrubber.running", "@58"},
    {"components.scrubber.strength", "@59"},
    {"components.vent.efficiency", "@60"},
    {"components.vent.percent", "@61"},
    {"components.vent.running", "@62"},
    {"components.vent.strength", "@63"},
    {"control.humidity.state", "@64"},
    {"control.o2.state", "@65"},
    {"control.pressure.state", "@66"},
    {"control.scrubber.state", "@67"},
    {"control.temperature.state", "@68"},
    {"control.ventilation.state", "@69"},
    {"sensors.co2.failure", "@70"},
    {"sensors.co2.lower", "@71"},
    {"sensors.co2.lower.failure", "@72"},
    {"sensors.co2.upper", "@73"},
    {"sensors.co2.upper.failure", "@74"},
    {"sensors.co2.value", "@75"},
    {"sensors.cold.failure", "@76"},
    {"sensors.cold.lower", "@77"},
    {"sensors.cold.lower.failure", "@78"},
    {"sensors.cold.upper", "@79"},
    {"sensors.cold.upper.failure", "@80"},
    {"sensors.cold.value", "@81"},
    {"sensors.heat.failure", "@82"},
    {"sensors.heat.lower", "@83"},
    {"sensors.heat.lower.failure", "@84"},
    {"sensors.heat.upper", "@85"},
    {"sensors.heat.upper.failure", "@86"},
    {"sensors.heat.value", "@87"},
    {"sensors.humidity.failure", "@88"},
    {"sensors.humidity.lower", "@89"},
    {"sensors.humidity.lower.failure", "@90"},
    {"sensors.humidity.upper", "@91"},
    {"sensors.humidity.upper.failure", "@92"},
    {"sensors.humidity.value", "@93"},
    {"sensors.o2.failure", "@94"},
    {"sensors.o2.lower", "@95"},
    {"sensors.o2.lower.failure", "@96"},
    {"sensors.o2.upper", "@97"},
    {"sensors.o2.upper.failure", "@98"},
    {"sensors.o2.value", "@99"},
    {"sensors.pressure.failure", "@100"},
    {"sensors.pressure.lower", "@101"},
    {"sensors.pressure.lower.failure", "@102"},
    {"sensors.pressure.upper", "@103"},
    {"sensors.pressure.upper.failure", "@104"},
    {"sensors.pressure.value", "@105"},
    {"sensors.ventilation.failure", "@106"},
    {"sensors.ventilation.lower", "@107"},
    {"sensors.ventilation.lower.failure", "@108"},
    {"sensors.ventilation.upper", "@109"},
    {"sensors.ventilation.upper.failure", "@110"},
    {"sensors.ventilation.value", "@111"}};

const QMap<QString, QString> idxToKey = []() {
  QMap<QString, QString> inverse;
  for (auto it = keyToIdx.begin(); it != keyToIdx.end(); ++it) {
    inverse.insert(it.value(), it.key());
  }
  return inverse;
}();

QJsonObject JsonVocabularyEncoder::shrink(QJsonObject t) {
  QJsonObject compressed;

  QString typeVal = t.value("type").toString();
  if (typeVal == "update")
    typeVal = "u";
  else if (typeVal == "notify")
    typeVal = "n";
  else if (typeVal == "control")
    typeVal = "c";
  else if (typeVal == "heartbeat")
    typeVal = "h";
  compressed.insert("t", typeVal);

  QString keyVal = t.value("key").toString();
  if (keyToIdx.contains(keyVal)) {
    keyVal = keyToIdx.value(keyVal);
  } else if (keyVal.startsWith('@')) {
    keyVal = "@" + keyVal;
  }
  compressed.insert("k", keyVal);

  if (t.contains("payload")) {
    compressed.insert("p", t.value("payload"));
  }

  return compressed;
}

QJsonObject JsonVocabularyEncoder::expand(QJsonObject t) {
  QJsonObject expanded;

  QString typeVal = t.value("t").toString();
  if (typeVal == "u")
    typeVal = "update";
  else if (typeVal == "n")
    typeVal = "notify";
  else if (typeVal == "c")
    typeVal = "control";
  else if (typeVal == "h")
    typeVal = "heartbeat";
  expanded.insert("type", typeVal);

  QString keyVal = t.value("k").toString();
  if (idxToKey.contains(keyVal)) {
    keyVal = idxToKey.value(keyVal);
  } else if (keyVal.startsWith("@@")) {
    keyVal = keyVal.mid(1);
  }
  expanded.insert("key", keyVal);

  if (t.contains("p")) {
    expanded.insert("payload", t.value("p"));
  }

  return expanded;
}
