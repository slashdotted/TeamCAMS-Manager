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
var VentIneffectiveFault = function () {};
VentIneffectiveFault.prototype = Object.create(generic_fault.prototype);
VentIneffectiveFault.prototype.errorName = "VentIneffectiveFault";
VentIneffectiveFault.prototype.errorMessage = qsTr("Vent ineffective");
VentIneffectiveFault.prototype.assistedControlConfigInitial = [["components.vent.strength", "standard"], ["control.ventilation.state", "assisted"], ["control.pressure.state", "assisted"]];
VentIneffectiveFault.prototype.assistedControlConfigFinal = [["components.vent.strength", "standard"], ["control.ventilation.state", "auto"], ["control.pressure.state", "auto"]];
VentIneffectiveFault.prototype.suggestions = [qsTr("Set vent to high and retain automatic mode of control."), qsTr("Arrange for repair to be done.")];
VentIneffectiveFault.prototype.enableError = function (_ctx, parameters) {
    var efficiency = ("efficiency" in parameters) ? parameters.efficiency : 0.5;
    registry.set("components.vent.efficiency", efficiency);
};
VentIneffectiveFault.prototype.disableError = function (_ctx) {
    registry.set("components.vent.efficiency", 1.0);
};
VentIneffectiveFault.prototype.performAssistedControl = function (_ctx) {
    var value = registry.get("components.cabin.pressure");
    var lowerThreshold = registry.get("sensors.pressure.lower");
    var upperThreshold = registry.get("sensors.pressure.upper");
    if (value < lowerThreshold) {
        registry.set("components.vent.running", false);
        registry.set("components.vent.strength", "medium");
        registry.set("components.n2tank.open", true);
        registry.set("components.n2valve.open", true);
        registry.set("automation.loa.message", qsTr("N<sub>2</sub> flow started. Ventilation off."));
    } else if (value > upperThreshold) {
        registry.set("components.vent.running", true);
        registry.set("components.vent.strength", "high");
        registry.set("components.n2tank.open", false);
        registry.set("components.n2valve.open", false);
        registry.set("automation.loa.message", qsTr("N<sub>2</sub> flow stopped. Ventilation on, with high strength."));
    }
};
