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
var DehumidifierIneffectiveFault = function () {};
DehumidifierIneffectiveFault.prototype = Object.create(generic_fault.prototype);
DehumidifierIneffectiveFault.prototype.errorName = "DehumidifierIneffectiveFault";
DehumidifierIneffectiveFault.prototype.errorMessage = qsTr("Dehumidifier ineffective");
DehumidifierIneffectiveFault.prototype.assistedControlConfigInitial = [["control.humidity.state", "assisted"], ["components.dehumidifier.strength", "high"]];
DehumidifierIneffectiveFault.prototype.assistedControlConfigFinal = [["control.humidity.state", "auto"], ["components.dehumidifier.strength", "standard"]];
DehumidifierIneffectiveFault.prototype.suggestions = [qsTr("Set dehumidifier to high and retain automatic mode of control."), qsTr("Arrange for repair to be done.")];
DehumidifierIneffectiveFault.prototype.performAssistedControl = function (_ctx) {
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
DehumidifierIneffectiveFault.prototype.enableError = function (_ctx, parameters) {
    var efficiency = ("efficiency" in parameters) ? parameters.efficiency : 0.5;
    registry.set("components.dehumidifier.efficiency", efficiency);
};
DehumidifierIneffectiveFault.prototype.disableError = function (_ctx) {
    registry.set("components.dehumidifier.efficiency", 1.0);
};
