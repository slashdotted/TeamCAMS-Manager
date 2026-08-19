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
var HumiditySensorFault = function () {};
HumiditySensorFault.prototype = Object.create(generic_fault.prototype);
HumiditySensorFault.prototype.errorName = "HumiditySensorFault";
HumiditySensorFault.prototype.errorMessage = qsTr("Switch point failure in dehumidifier control");
HumiditySensorFault.prototype.errorOn = [["sensors.humidity.failure", true]];
HumiditySensorFault.prototype.errorOff = [["sensors.humidity.failure", false]];
HumiditySensorFault.prototype.assistedControlConfigInitial = [["control.humidity.state", "assisted"], ["components.dehumidifier.strength", "high"]];
HumiditySensorFault.prototype.assistedControlConfigFinal = [["control.humidity.state", "auto"], ["components.dehumidifier.strength", "standard"]];
this.suggestions = [qsTr("Turn on and off dehumidifier feed flow until level is back to normal range."), qsTr("Control dehumidifier manually until fault is eradicated."), qsTr("Arrange for repair to be done.")];
HumiditySensorFault.prototype.performAssistedControl = function (_ctx) {
    var value = registry.get("components.cabin.humidity");
    var lowerThreshold = registry.get("sensors.humidity.lower");
    var upperThreshold = registry.get("sensors.humidity.upper");
    if (value < lowerThreshold) {
        registry.set("components.dehumidifier.strength", "standard");
        registry.set("components.dehumidifier.running", false);
        registry.set("automation.loa.message", qsTr("Dehumidifier turned off."));
    } else if (value > upperThreshold) {
        registry.set("components.dehumidifier.strength", "high");
        registry.set("components.dehumidifier.running", true);
        registry.set("automation.loa.message", qsTr("Dehumidifier turned on."));
    }
};

// **************************************************************************************
// Fault class definition
// **************************************************************************************
var HumiditySensorFaultUpper = function () {};
HumiditySensorFaultUpper.prototype = Object.create(HumiditySensorFault.prototype);
HumiditySensorFaultUpper.prototype.errorName = "HumiditySensorFaultUpper";
HumiditySensorFaultUpper.prototype.validRepairs = ["HumiditySensorFault", "HumiditySensorFaultUpper"];
HumiditySensorFaultUpper.prototype.errorOn = [["sensors.humidity.upper.failure", true]];
HumiditySensorFaultUpper.prototype.errorOff = [["sensors.humidity.upper.failure", false]];

// **************************************************************************************
// Fault class definition
// **************************************************************************************
var HumiditySensorFaultLower = function () {};
HumiditySensorFaultLower.prototype = Object.create(HumiditySensorFault.prototype);
HumiditySensorFaultLower.prototype.errorName = "HumiditySensorFaultLower";
HumiditySensorFaultLower.prototype.validRepairs = ["HumiditySensorFault", "HumiditySensorFaultLower"];
HumiditySensorFaultLower.prototype.errorOn = [["sensors.humidity.lower.failure", true]];
HumiditySensorFaultLower.prototype.errorOff = [["sensors.humidity.lower.failure", false]];
