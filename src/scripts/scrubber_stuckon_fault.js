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
var ScrubberStuckOnFault = function () {};
ScrubberStuckOnFault.prototype = Object.create(generic_fault.prototype);
ScrubberStuckOnFault.prototype.errorName = "ScrubberStuckOnFault";
ScrubberStuckOnFault.prototype.errorMessage = qsTr("CO<sub>2</sub> scrubber stuck on.");
ScrubberStuckOnFault.prototype.assistedControlConfigInitial = [["components.scrubber.strength", "standard"], ["control.scrubber.state", "assisted"]];
ScrubberStuckOnFault.prototype.assistedControlConfigFinal = [["components.scrubber.strength", "standard"], ["control.scrubber.state", "auto"]];
ScrubberStuckOnFault.prototype.suggestions = [qsTr("Turn on and off CO<sub>2</sub> scrubber until level is back to normal range."), qsTr("Control scrubber manually until fault is eradicated."), qsTr("Arrange for repair to be done.")];
ScrubberStuckOnFault.prototype.enableError = function (_ctx, parameters) {
    registry.setAndStick("components.scrubber.running", true);
};
ScrubberStuckOnFault.prototype.disableError = function (_ctx) {
    registry.unstick("components.scrubber.running");
};
ScrubberStuckOnFault.prototype.performAssistedControl = function (_ctx) {
    var relativeValue = registry.get("components.cabin.co2.relvalue");
    var lowerThreshold = registry.get("sensors.co2.lower");
    var upperThreshold = registry.get("sensors.co2.upper");
    if (relativeValue < (lowerThreshold + 0.05)) {
        registry.set("components.scrubber.strength", "standard");
        registry.set("components.scrubber.running", false);
        registry.set("automation.loa.message", qsTr("CO<sub>2</sub> scrubber strength set to standard."));
    } else if (relativeValue > (upperThreshold - 0.05)) {
        registry.set("components.scrubber.strength", "high");
        registry.set("components.scrubber.running", true); // FIX ME
        registry.set("automation.loa.message", qsTr("CO<sub>2</sub> scrubber strength set to high."));
    }
};
