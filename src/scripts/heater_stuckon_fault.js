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
var HeaterStuckOnFault = function () {};
HeaterStuckOnFault.prototype = Object.create(generic_fault.prototype);
HeaterStuckOnFault.prototype.errorName = "HeaterStuckOnFault";
HeaterStuckOnFault.prototype.errorMessage = qsTr("Heater stuck on");
HeaterStuckOnFault.prototype.assistedControlConfigInitial = [["components.heater.strength", "high"], ["control.temperature.state", "assisted"]];
HeaterStuckOnFault.prototype.assistedControlConfigFinal = [["components.heater.strength", "standard"], ["control.temperature.state", "auto"]];
HeaterStuckOnFault.prototype.suggestions = [qsTr("Turn on and off cooler and heater until level is back to normal range."), qsTr("Control heater manually until fault is eradicated."), qsTr("Arrange for repair to be done.")];
HeaterStuckOnFault.prototype.performAssistedControl = function (_ctx) {
    var value = registry.get("components.cabin.temperature");
    var lowerThreshold = registry.get("sensors.cold.lower");
    var upperThreshold = registry.get("sensors.cold.upper");
    if (value < lowerThreshold) {
        registry.set("components.heater.strength", "standard");
        registry.set("components.cooler.strength", "standard");
        registry.set("components.cooler.running", false);
        registry.set("automation.loa.message", qsTr("Cooler turned off."));
    } else if (value > upperThreshold) {
        registry.set("components.cooler.strength", "high");
        registry.set("components.heater.strength", "standard");
        registry.set("components.cooler.running", true);
        registry.set("automation.loa.message", qsTr("Cooler turned on."));
    }
};
HeaterStuckOnFault.prototype.enableError = function (_ctx, parameters) {
    registry.setAndStick("components.heater.running", true);
};
HeaterStuckOnFault.prototype.disableError = function (_ctx) {
    registry.unstick("components.heater.running");
};
