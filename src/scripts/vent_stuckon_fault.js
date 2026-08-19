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
var VentStuckOnFault = function () {};
VentStuckOnFault.prototype = Object.create(generic_fault.prototype);
VentStuckOnFault.prototype.errorName = "VentStuckOnFault";
VentStuckOnFault.prototype.errorMessage = qsTr("Vent stuck on");
VentStuckOnFault.prototype.assistedControlConfigInitial = [["components.n2tank.open", true], ["components.n2valve.strength", "high"], ["components.o2tank.open", true], ["components.o2valve.strength", "high"], ["control.ventilation.state", "assisted"], ["control.pressure.state", "assisted"], ["control.o2.state", "assisted"]];
VentStuckOnFault.prototype.assistedControlConfigFinal = [["components.n2tank.open", true], ["components.n2valve.strength", "standard"], ["components.o2tank.open", true], ["components.o2valve.strength", "standard"], ["control.ventilation.state", "auto"], ["control.pressure.state", "auto"], ["control.o2.state", "auto"]];
VentStuckOnFault.prototype.suggestions = [qsTr("Turn on and off vent and N<sub>2</sub> and O<sub>2</sub> until level is back to normal range."), qsTr("Arrange for repair to be done.")];
VentStuckOnFault.prototype.performAssistedControl = function (_ctx) {
    var value = registry.get("components.cabin.pressure");
    var lowerThreshold = registry.get("sensors.pressure.lower");
    var upperThreshold = registry.get("sensors.pressure.upper");
    if (value < lowerThreshold) {
        registry.set("components.n2tank.open", true);
        registry.set("components.n2valve.open", true);
        registry.set("components.o2tank.open", true);
        registry.set("components.o2valve.open", true);
        registry.set("components.vent.strength", "medium");
        registry.set("components.o2valve.strength", "high");
        registry.set("components.n2valve.strength", "high");
        registry.set("automation.loa.message", qsTr("N<sub>2</sub> and O<sub>2</sub> flows started. Ventilation strength is medium."));
    } else if (value > upperThreshold) {
        registry.set("components.o2tank.open", false);
        registry.set("components.o2valve.open", false);
        registry.set("components.n2tank.open", false);
        registry.set("components.n2valve.open", false);
        registry.set("components.vent.strength", "high");
        registry.set("components.o2valve.strength", "medium");
        registry.set("components.n2valve.strength", "medium");
        registry.set("automation.loa.message", qsTr("N<sub>2</sub> and O<sub>2</sub> flows stopped. Ventilation strength is high."));
    }
};
VentStuckOnFault.prototype.enableError = function (_ctx, parameters) {
    registry.setAndStick("components.vent.running", true);
};
VentStuckOnFault.prototype.disableError = function (_ctx) {
    registry.unstick("components.vent.running");
};
