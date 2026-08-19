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
var PressureSensorFault = function () {};
PressureSensorFault.prototype = Object.create(generic_fault.prototype);
PressureSensorFault.prototype.errorName = "PressureSensorFault";
PressureSensorFault.prototype.errorMessage = qsTr("Switch point failure in N<sub>2</sub> control");
PressureSensorFault.prototype.errorOn = [["sensors.pressure.failure", true]];
PressureSensorFault.prototype.errorOff = [["sensors.pressure.failure", false]];
PressureSensorFault.prototype.assistedControlConfigInitial = [["control.pressure.state", "assisted"], ["control.ventilation.state", "assisted"]];
PressureSensorFault.prototype.assistedControlConfigFinal = [["control.pressure.state", "auto"], ["control.ventilation.state", "auto"]];
PressureSensorFault.prototype.suggestions = [qsTr("Turn on and off O<sub>2</sub> feed flow until level is back to normal range."), qsTr("Arrange for repair to be done."), qsTr("Manual control of N<sub>2</sub> feed flow until fault is eradicated.")];
PressureSensorFault.prototype.performAssistedControl = function (_ctx) {
    var pvalue = registry.get("components.cabin.pressure");
    var plowerThreshold = registry.get("sensors.pressure.lower");
    var pupperThreshold = registry.get("sensors.pressure.upper");
    if (pvalue < plowerThreshold) {
        registry.set("components.n2tank.open", true);
        registry.set("components.n2valve.open", true);
        registry.set("automation.loa.message", qsTr("N<sub>2</sub> feed flow turned on."));
    } else if (pvalue > pupperThreshold) {
        registry.set("components.n2tank.open", false);
        registry.set("components.n2valve.open", false);
        registry.set("automation.loa.message", qsTr("N<sub>2</sub> feed flow turned off."));
    }

    var vvalue = registry.get("components.cabin.pressure");
    var vlowerThreshold = registry.get("sensors.ventilation.lower");
    var vupperThreshold = registry.get("sensors.ventilation.upper");
    if (vvalue < vlowerThreshold) {
        registry.set("components.vent.running", false);
    } else if (vvalue > vupperThreshold) {
        registry.set("components.vent.running", true);
    }
};

// **************************************************************************************
// Fault class definition
// **************************************************************************************
var PressureSensorFaultUpper = function () {};
PressureSensorFaultUpper.prototype = Object.create(PressureSensorFault.prototype);
PressureSensorFaultUpper.prototype.errorName = "PressureSensorFaultUpper";
PressureSensorFaultUpper.prototype.validRepairs = ["PressureSensorFault", "PressureSensorFaultUpper"];
PressureSensorFaultUpper.prototype.errorOn = [["sensors.pressure.upper.failure", true]];
PressureSensorFaultUpper.prototype.errorOff = [["sensors.pressure.upper.failure", false]];

// **************************************************************************************
// Fault class definition
// **************************************************************************************
var PressureSensorFaultLower = function () {};
PressureSensorFaultLower.prototype = Object.create(PressureSensorFault.prototype);
PressureSensorFaultLower.prototype.errorName = "PressureSensorFaultLower";
PressureSensorFaultLower.prototype.validRepairs = ["PressureSensorFault", "PressureSensorFaultLower"];
PressureSensorFaultLower.prototype.errorOn = [["sensors.pressure.lower.failure", true]];
PressureSensorFaultLower.prototype.errorOff = [["sensors.pressure.lower.failure", false]];
