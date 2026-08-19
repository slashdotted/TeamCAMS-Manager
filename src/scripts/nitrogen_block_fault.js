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
var NitrogenBlockFault = function () {};
NitrogenBlockFault.prototype = Object.create(generic_fault.prototype);
NitrogenBlockFault.prototype.errorName = "NitrogenBlockFault";
NitrogenBlockFault.prototype.errorMessage = qsTr("Block in N<sub>2</sub> valve");
NitrogenBlockFault.prototype.errorOn = [["components.n2valve.block", true], ["components.n2valve.block.factor", 0.4]];
NitrogenBlockFault.prototype.errorOff = [["components.n2valve.block", false], ["components.n2valve.block.factor", 0.4]];
NitrogenBlockFault.prototype.assistedControlConfigInitial = [["components.n2valve.strength", "high"]];
NitrogenBlockFault.prototype.assistedControlConfigFinal = [["components.n2valve.strength", "standard"]];
NitrogenBlockFault.prototype.suggestions = [qsTr("Set N<sub>2</sub> feed flow to high and retain automatic mode of control."), qsTr("Arrange for repair to be done.")];
