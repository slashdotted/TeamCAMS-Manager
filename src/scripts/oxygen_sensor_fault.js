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
var OxygenSensorFault = function () {};
OxygenSensorFault.prototype = Object.create(generic_fault.prototype);
OxygenSensorFault.prototype.errorName = "OxygenSensorFault";
OxygenSensorFault.prototype.errorMessage = qsTr("Switch point failure in O<sub>2</sub> control");
OxygenSensorFault.prototype.errorOn = [["sensors.o2.failure", true]];
OxygenSensorFault.prototype.errorOff = [["sensors.o2.failure", false]];
OxygenSensorFault.prototype.assistedControlConfigInitial = [["control.o2.state", "assisted"], ["components.o2tank.open", false]];
OxygenSensorFault.prototype.assistedControlConfigFinal = [["control.o2.state", "auto"], ["components.o2tank.open", true]];
OxygenSensorFault.prototype.suggestions = [qsTr("Turn on and off O<sub>2</sub> feed flow until level is back to normal range."), qsTr("Arrange for repair to be done."), qsTr("Manual control of O<sub>2</sub> feed flow until fault is eradicated.")];
OxygenSensorFault.prototype.performAssistedControl = function (_ctx) {
    var value = registry.get("components.cabin.o2.relvalue");
    var lowerThreshold = registry.get("sensors.o2.lower");
    var upperThreshold = registry.get("sensors.o2.upper");
    if (value < lowerThreshold) {
        registry.set("components.o2tank.open", true);
        registry.set("components.o2valve.open", true);
        registry.set("automation.loa.message", qsTr("O<sub>2</sub> feed flow turned on."));
    } else if (value > upperThreshold) {
        registry.set("components.o2tank.open", false);
        registry.set("components.o2valve.open", false);
        registry.set("automation.loa.message", qsTr("O<sub>2</sub> feed flow turned off."));
    }
};

// **************************************************************************************
// Fault class definition
// **************************************************************************************
var OxygenSensorFaultUpper = function () {};
OxygenSensorFaultUpper.prototype = Object.create(OxygenSensorFault.prototype);
OxygenSensorFaultUpper.prototype.errorName = "OxygenSensorFaultUpper";
OxygenSensorFaultUpper.prototype.validRepairs = ["OxygenSensorFault", "OxygenSensorFaultUpper"];
OxygenSensorFaultUpper.prototype.errorOn = [["sensors.o2.upper.failure", true]];
OxygenSensorFaultUpper.prototype.errorOff = [["sensors.o2.upper.failure", false]];

// **************************************************************************************
// Fault class definition
// **************************************************************************************
var OxygenSensorFaultLower = function () {};
OxygenSensorFaultLower.prototype = Object.create(OxygenSensorFault.prototype);
OxygenSensorFaultLower.prototype.errorName = "OxygenSensorFaultLower";
OxygenSensorFaultLower.prototype.validRepairs = ["OxygenSensorFault", "OxygenSensorFaultLower"];
OxygenSensorFaultLower.prototype.errorOn = [["sensors.o2.lower.failure", true]];
OxygenSensorFaultLower.prototype.errorOff = [["sensors.o2.lower.failure", false]];
