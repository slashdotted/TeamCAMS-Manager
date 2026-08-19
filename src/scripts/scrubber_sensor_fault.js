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
var ScrubberSensorFault = function () {};
ScrubberSensorFault.prototype = Object.create(generic_fault.prototype);
ScrubberSensorFault.prototype.errorName = "ScrubberSensorFault";
ScrubberSensorFault.prototype.errorMessage = qsTr("Switch point failure in N<sub>2</sub> control");
ScrubberSensorFault.prototype.errorOn = [["sensors.co2.failure", true]];
ScrubberSensorFault.prototype.errorOff = [["sensors.co2.failure", false]];
ScrubberSensorFault.prototype.assistedControlConfigInitial = [["control.scrubber.state", "assisted"], ["components.scrubber.strength", "high"]];
ScrubberSensorFault.prototype.assistedControlConfigFinal = [["control.scrubber.state", "auto"], ["components.scrubber.strength", "standard"]];
ScrubberSensorFault.prototype.suggestions = [qsTr("Turn on and off CO<sub>2</sub> scrubber until level is back to normal range."), qsTr("Control scrubber manually until fault is eradicated."), qsTr("Arrange for repair to be done.")];
ScrubberSensorFault.prototype.performAssistedControl = function (_ctx) {
    var value = registry.get("components.cabin.pressure");
    var lowerThreshold = registry.get("sensors.co2.lower");
    var upperThreshold = registry.get("sensors.co2.upper");
    if (value < (lowerThreshold + 0.05)) {
        registry.set("components.scrubber.strength", "standard");
        registry.set("components.scrubber.running", false);
        registry.set("automation.loa.message", qsTr("CO<sub>2</sub> scrubber turned off."));
    } else if (value > (upperThreshold - 0.05)) {
        registry.set("components.scrubber.strength", "high");
        registry.set("components.scrubber.running", true);
        registry.set("automation.loa.message", qsTr("CO<sub>2</sub> scrubber turned on."));
    }
};

// **************************************************************************************
// Fault class definition
// **************************************************************************************
var ScrubberSensorFaultUpper = function () {};
ScrubberSensorFaultUpper.prototype = Object.create(ScrubberSensorFault.prototype);
ScrubberSensorFaultUpper.prototype.errorName = "ScrubberSensorFaultUpper";
ScrubberSensorFaultUpper.prototype.validRepairs = ["ScrubberSensorFault", "ScrubberSensorFaultUpper"];
ScrubberSensorFaultUpper.prototype.errorOn = [["sensors.co2.upper.failure", true]];
ScrubberSensorFaultUpper.prototype.errorOff = [["sensors.co2.upper.failure", false]];

// **************************************************************************************
// Fault class definition
// **************************************************************************************
var ScrubberSensorFaultLower = function () {};
ScrubberSensorFaultLower.prototype = Object.create(ScrubberSensorFault.prototype);
ScrubberSensorFaultLower.prototype.errorName = "ScrubberSensorFaultLower";
ScrubberSensorFaultLower.prototype.validRepairs = ["ScrubberSensorFault", "ScrubberSensorFaultLower"];
ScrubberSensorFaultLower.prototype.errorOn = [["sensors.co2.lower.failure", true]];
ScrubberSensorFaultLower.prototype.errorOff = [["sensors.co2.lower.failure", false]];
