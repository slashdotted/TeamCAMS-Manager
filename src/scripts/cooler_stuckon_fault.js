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
var CoolerStuckOnFault = function () {};
CoolerStuckOnFault.prototype = Object.create(generic_fault.prototype);
CoolerStuckOnFault.prototype.errorName = "CoolerStuckOnFault";
CoolerStuckOnFault.prototype.errorMessage = qsTr("Cooler stuck on");
CoolerStuckOnFault.prototype.assistedControlConfigInitial = [["components.cooler.strength", "high"], ["control.temperature.state", "assisted"]];
CoolerStuckOnFault.prototype.assistedControlConfigFinal = [["components.cooler.strength", "standard"], ["control.temperature.state", "auto"]];
CoolerStuckOnFault.prototype.suggestions = [qsTr("Turn on and off cooler and heater until level is back to normal range."), qsTr("Control cooler manually until fault is eradicated."), qsTr("Arrange for repair to be done.")];
CoolerStuckOnFault.prototype.performAssistedControl = function (_ctx) {
    var value = registry.get("components.cabin.temperature");
    var lowerThreshold = registry.get("sensors.cold.lower");
    var upperThreshold = registry.get("sensors.cold.upper");
    if (value < lowerThreshold) {
        registry.set("components.heater.strength", "standard");
        registry.set("components.cooler.strength", "standard");
        registry.set("components.heater.running", true);
        registry.set("automation.loa.message", qsTr("Cooler turned off. Heater turned on."));
    } else if (value > upperThreshold) {
        registry.set("components.cooler.strength", "high");
        registry.set("components.heater.strength", "standard");
        registry.set("components.heater.running", false);
        registry.set("automation.loa.message", qsTr("Cooler turned on. Heater turned off."));
    }
};
CoolerStuckOnFault.prototype.enableError = function (_ctx, parameters) {
    registry.setAndStick("components.cooler.running", true);
};
CoolerStuckOnFault.prototype.disableError = function (_ctx) {
    registry.unstick("components.cooler.running");
};
