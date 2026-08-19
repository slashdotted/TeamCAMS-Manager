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
var ScrubberIneffectiveFault = function () {};
ScrubberIneffectiveFault.prototype = Object.create(generic_fault.prototype);
ScrubberIneffectiveFault.prototype.errorName = "ScrubberIneffectiveFault";
ScrubberIneffectiveFault.prototype.errorMessage = qsTr("CO<sub>2</sub> scrubber ineffective");
ScrubberIneffectiveFault.prototype.assistedControlConfigInitial = [["components.scrubber.strength", "high"], ["control.scrubber.state", "assisted"]];
ScrubberIneffectiveFault.prototype.assistedControlConfigFinal = [["components.scrubber.strength", "standard"], ["control.scrubber.state", "auto"]];
ScrubberIneffectiveFault.prototype.suggestions = [qsTr("Set scrubber to high and retain automatic mode of control."), qsTr("Arrange for repair to be done.")];
ScrubberIneffectiveFault.prototype.enableError = function (_ctx, parameters) {
    var efficiency = ("efficiency" in parameters) ? parameters.efficiency : 0.5;
    registry.set("components.scrubber.efficiency", efficiency);
};

ScrubberIneffectiveFault.prototype.disableError = function (_ctx) {
    registry.set("components.scrubber.efficiency", 1.0);
};
ScrubberIneffectiveFault.prototype.performAssistedControl = function (_ctx) {
    var value = registry.get("components.cabin.co2.relvalue");
    var lowerThreshold = registry.get("sensors.co2.lower");
    var upperThreshold = registry.get("sensors.co2.upper");
    if (value < (lowerThreshold + 0.05)) {
        registry.set("components.scrubber.strength", "standard");
        registry.set("components.scrubber.running", false);
        registry.set("automation.loa.message", qsTr("CO<sub>2</sub> scrubber turned off."));
    } else if (value > (upperThreshold - 0.05)) {
        registry.set("components.scrubber.strength", "high");
        registry.set("components.scrubber.running", true);
        registry.set("automation.loa.message", qsTr("CO<sub>2</sub> scrubber turned off."));
    }
};
