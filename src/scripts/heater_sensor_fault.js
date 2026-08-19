// TeamCAMS - reborn Cabin Air Management System
// Copyright (C) 2015-2026  Amos Brocco,
//                          Cognitive Ergonomics and Work Psychology Team,
//                          Psychology Department of Fribourg University, Switzerland /
//                          Department of Innovative Technologies
//                          University of Applied Sciences and Arts of Southern Switzerland,
//                          Contact: amos.brocco@supsi.ch
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
// **************************************************************************************
// Fault class definition
// **************************************************************************************
var HeaterSensorFault = function () {};
HeaterSensorFault.prototype = Object.create(generic_fault.prototype);
HeaterSensorFault.prototype.errorName = "HeaterSensorFault";
HeaterSensorFault.prototype.errorMessage = qsTr("Switch point failure in heater control");
HeaterSensorFault.prototype.errorOn = [["sensors.heat.failure", true]];
HeaterSensorFault.prototype.errorOff = [["sensors.heat.failure", false]];
HeaterSensorFault.prototype.assistedControlConfigInitial = [["components.cooler.strength", "high"], ["control.temperature.state", "assisted"]];
HeaterSensorFault.prototype.assistedControlConfigFinal = [["components.cooler.strength", "standard"], ["control.temperature.state", "auto"]];
HeaterSensorFault.prototype.suggestions = [qsTr("Turn on and off cooler and heater until level is back to normal range."), qsTr("Control heater manually until fault is eradicated."), qsTr("Arrange for repair to be done.")];
HeaterSensorFault.prototype.performAssistedControl = function (_ctx) {
    var value = registry.get("components.cabin.temperature");
    var lowerThreshold = registry.get("sensors.heat.lower");
    var upperThreshold = registry.get("sensors.heat.upper");
    if (value < lowerThreshold) {
        registry.set("components.heater.strength", "standard");
        registry.set("components.cooler.strength", "standard");
        registry.set("components.cooler.running", false);
        registry.set("components.heater.running", true);
        registry.set("automation.loa.message", qsTr("Cooler turned off. Heater turned on."));
    } else if (value > upperThreshold) {
        registry.set("components.cooler.strength", "high");
        registry.set("components.heater.strength", "standard");
        registry.set("components.heater.running", false);
        registry.set("components.cooler.running", true);
        registry.set("automation.loa.message", qsTr("Cooler turned on. Heater turned off."));
    }
};

// **************************************************************************************
// Fault class definition
// **************************************************************************************
var HeaterSensorFaultUpper = function () {};
HeaterSensorFaultUpper.prototype = Object.create(HeaterSensorFault.prototype);
HeaterSensorFaultUpper.prototype.errorName = "HeaterSensorFaultUpper";
HeaterSensorFaultUpper.prototype.validRepairs = ["HeaterSensorFault", "HeaterSensorFaultUpper"];
HeaterSensorFaultUpper.prototype.errorOn = [["sensors.heat.upper.failure", true]];
HeaterSensorFaultUpper.prototype.errorOff = [["sensors.heat.upper.failure", false]];

// **************************************************************************************
// Fault class definition
// **************************************************************************************
var HeaterSensorFaultLower = function () {};
HeaterSensorFaultLower.prototype = Object.create(HeaterSensorFault.prototype);
HeaterSensorFaultLower.prototype.errorName = "HeaterSensorFaultLower";
HeaterSensorFaultLower.prototype.validRepairs = ["HeaterSensorFault", "HeaterSensorFaultLower"];
HeaterSensorFaultLower.prototype.errorOn = [["sensors.heat.lower.failure", true]];
HeaterSensorFaultLower.prototype.errorOff = [["sensors.heat.lower.failure", false]];
