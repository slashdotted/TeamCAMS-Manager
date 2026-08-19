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
var CoolerSensorFault = function () {};
CoolerSensorFault.prototype = Object.create(generic_fault.prototype);
CoolerSensorFault.prototype.errorName = "CoolerSensorFault";
CoolerSensorFault.prototype.errorMessage = qsTr("Switch point failure in cooler control");
CoolerSensorFault.prototype.errorOn = [["sensors.cold.failure", true]];
CoolerSensorFault.prototype.errorOff = [["sensors.cold.failure", false]];
CoolerSensorFault.prototype.assistedControlConfigInitial = [["components.cooler.strength", "high"], ["control.temperature.state", "assisted"]];
CoolerSensorFault.prototype.assistedControlConfigFinal = [["components.cooler.strength", "standard"], ["control.temperature.state", "auto"]];
CoolerSensorFault.prototype.suggestions = [qsTr("Turn on and off cooler and heater until level is back to normal range."), qsTr("Control cooler manually until fault is eradicated."), qsTr("Arrange for repair to be done.")];
CoolerSensorFault.prototype.performAssistedControl = function (_ctx) {
    var value = registry.get("components.cabin.temperature");
    var lowerThreshold = registry.get("sensors.cold.lower");
    var upperThreshold = registry.get("sensors.cold.upper");
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
var CoolerSensorFaultUpper = function () {};
CoolerSensorFaultUpper.prototype = Object.create(CoolerSensorFault.prototype);
CoolerSensorFaultUpper.prototype.errorName = "CoolerSensorFaultUpper";
CoolerSensorFaultUpper.prototype.validRepairs = ["CoolerSensorFault", "CoolerSensorFaultUpper"];
CoolerSensorFaultUpper.prototype.errorOn = [["sensors.cold.upper.failure", true]];
CoolerSensorFaultUpper.prototype.errorOff = [["sensors.cold.upper.failure", false]];

// **************************************************************************************
// Fault class definition
// **************************************************************************************
var CoolerSensorFaultLower = function () {};
CoolerSensorFaultLower.prototype = Object.create(CoolerSensorFault.prototype);
CoolerSensorFaultLower.prototype.errorName = "CoolerSensorFaultLower";
CoolerSensorFaultLower.prototype.validRepairs = ["CoolerSensorFault", "CoolerSensorFaultLower"];
CoolerSensorFaultLower.prototype.errorOn = [["sensors.cold.lower.failure", true]];
CoolerSensorFaultLower.prototype.errorOff = [["sensors.cold.lower.failure", false]];
