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
var OxygenStuckOpenFault = function () {};
OxygenStuckOpenFault.prototype = Object.create(generic_fault.prototype);
OxygenStuckOpenFault.prototype.errorName = "OxygenStuckOpenFault";
OxygenStuckOpenFault.prototype.errorMessage = qsTr("Stuck open O<sub>2</sub> valve");
OxygenStuckOpenFault.prototype.assistedControlConfigInitial = [["control.o2.state", "off"], ["components.o2tank.open", false]];
OxygenStuckOpenFault.prototype.assistedControlConfigFinal = [["control.o2.state", "auto"], ["components.o2tank.open", true]];
OxygenStuckOpenFault.prototype.suggestions = [qsTr("Turn on and off O<sub>2</sub> feed flow until level is back to normal range."), qsTr("Arrange for repair to be done.")];
OxygenStuckOpenFault.prototype.enableError = function (_ctx, parameters) {
    registry.setAndStick("components.o2valve.open", true);
};
OxygenStuckOpenFault.prototype.disableError = function (_ctx) {
    registry.unstick("components.o2valve.open");
};
OxygenStuckOpenFault.prototype.performAssistedControl = function (_ctx) {
    var relativeValue = registry.get("components.cabin.o2.relvalue");
    var lowerThreshold = registry.get("sensors.o2.lower");
    var upperThreshold = registry.get("sensors.o2.upper");
    if (relativeValue < lowerThreshold) {
        registry.set("components.o2tank.open", true);
        registry.set("automation.loa.message", qsTr("O<sub>2</sub> feed flow turned on."));
    } else if (relativeValue > upperThreshold) {
        registry.set("components.o2tank.open", false);
        registry.set("automation.loa.message", qsTr("O<sub>2</sub> feed flow turned off."));
    }
};
