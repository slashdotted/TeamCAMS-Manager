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
var NitrogenStuckOpenFault = function () {};
NitrogenStuckOpenFault.prototype = Object.create(generic_fault.prototype);
NitrogenStuckOpenFault.prototype.errorName = "NitrogenStuckOpenFault";
NitrogenStuckOpenFault.prototype.errorMessage = qsTr("Stuck open N<sub>2</sub> valve");
NitrogenStuckOpenFault.prototype.assistedControlConfigInitial = [["control.pressure.state", "assisted"], ["components.n2tank.open", false]];
NitrogenStuckOpenFault.prototype.assistedControlConfigFinal = [["control.pressure.state", "auto"], ["components.n2tank.open", true]];
NitrogenStuckOpenFault.prototype.suggestions = [qsTr("Turn off N<sub>2</sub> feed flow."), qsTr("Arrange for repair to be done."), qsTr("Manual control of N<sub>2</sub> feed flow until fault is eradicated.")];
NitrogenStuckOpenFault.prototype.performAssistedControl = function (_ctx) {
    var value = registry.get("components.cabin.pressure");
    var lowerThreshold = registry.get("sensors.pressure.lower");
    var upperThreshold = registry.get("sensors.pressure.upper");
    if (value < lowerThreshold) {
        registry.set("components.n2tank.open", true);
        registry.set("components.n2valve.open", true);
        registry.set("automation.loa.message", qsTr("N<sub>2</sub> feed flow turned on."));
    } else if (value > upperThreshold) {
        if (registry.get("components.n2pipetankvalve.flow") > 0.0) {
            registry.set("components.n2tank.open", false);
            registry.set("components.n2valve.open", false);
            registry.set("automation.loa.message", qsTr("N<sub>2</sub> feed flow turned off."));
        }
    }
};
NitrogenStuckOpenFault.prototype.enableError = function (_ctx, parameters) {
    registry.setAndStick("components.n2valve.open", true);
};
NitrogenStuckOpenFault.prototype.disableError = function (_ctx) {
    registry.unstick("components.n2valve.open");
};
