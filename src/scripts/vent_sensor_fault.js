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
var VentSensorFault = function () {};
VentSensorFault.prototype = Object.create(generic_fault.prototype);
VentSensorFault.prototype.errorName = "VentSensorFault";
VentSensorFault.prototype.errorMessage = qsTr("Switch point failure in vent control");
VentSensorFault.prototype.errorOn = [["sensors.ventilation.failure", true]];
VentSensorFault.prototype.errorOff = [["sensors.ventilation.failure", false]];
VentSensorFault.prototype.assistedControlConfigInitial = [["components.vent.strength", "standard"], ["control.ventilation.state", "assisted"], ["control.pressure.state", "assisted"]];
VentSensorFault.prototype.assistedControlConfigFinal = [["components.vent.strength", "standard"], ["control.ventilation.state", "auto"], ["control.pressure.state", "auto"]];
VentSensorFault.prototype.suggestions = [qsTr("Turn on and off vent and N<sub>2</sub> and O<sub>2</sub> until level is back to normal range.."), qsTr("Control vent manually until fault is eradicated."), qsTr("Arrange for repair to be done.")];
VentSensorFault.prototype.performAssistedControl = function (_ctx) {
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

// **************************************************************************************
// Fault class definition
// **************************************************************************************
var VentSensorFaultUpper = function () {};
VentSensorFaultUpper.prototype = Object.create(VentSensorFault.prototype);
VentSensorFaultUpper.prototype.errorName = "VentSensorFaultUpper";
VentSensorFaultUpper.prototype.validRepairs = ["VentSensorFault", "VentSensorFaultUpper"];
VentSensorFaultUpper.prototype.errorOn = [["sensors.ventilation.upper.failure", true]];
VentSensorFaultUpper.prototype.errorOff = [["sensors.ventilation.upper.failure", false]];

// **************************************************************************************
// Fault class definition
// **************************************************************************************
var VentSensorFaultLower = function () {};
VentSensorFaultLower.prototype = Object.create(VentSensorFault.prototype);
VentSensorFaultLower.prototype.errorName = "VentSensorFaultLower";
VentSensorFaultLower.prototype.validRepairs = ["VentSensorFault", "VentSensorFaultLower"];
VentSensorFaultLower.prototype.errorOn = [["sensors.ventilation.lower.failure", true]];
VentSensorFaultLower.prototype.errorOff = [["sensors.ventilation.lower.failure", false]];
