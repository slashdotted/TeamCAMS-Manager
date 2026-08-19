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
/* Base definitions */

// Disable translations (we are ready here but for now the operator will only see messages in english)
qsTr = function (text) {
    return text;
};

qsTranslate = function (context, text) {
    return text;
};

qsTrId = function (id) {
    return id;
};

// Broadcast a notification
var notify = function (event, payload) {
    triggers.processTriggersWithBindings("<notify>", JSON.stringify({
        "event": event,
        "value": payload
    })); // Used for loopback during tests
    connection.broadcastNotify(event, payload);
};

// Sends an update to specific users
var sendUserUpdate = function (key, value, userpattern) {
    var matchingusers = connection.connectUsersMatching(userpattern);
    for (i = 0; i < matchingusers.length; i++) {
        logging.log("update", "<system:" + matchingusers[i] + ">", JSON.stringify({
            "key": key,
            "value": value
        }));
        connection.sendUpdate(key, value, matchingusers[i]);
    }
};

var realConnectedUsers = function () {
    var cu = connection.connectedUsers();
    var rcu = [];
    for (i = 0; i < cu.length; i++) {
        if (!cu[i].startsWith("$")) {
            rcu.push(cu[i]);
        }
    }
    return rcu;
};

var resetSimulationParameters = function () {
    var euuid = registry.get("system.error.eid");
    if (euuid !== "") {
        scheduler.unscheduleByUuid(euuid);
        triggers.deactivateByUuid(euuid);
    }
    triggers.deactivateByType("survey");
    triggers.deactivateByUuid(registry.get("tutorial.trigger.log"));
    triggers.deactivateByUuid(registry.get("tutorial.trigger.generic"));
    triggers.deactivateByUuid(registry.get("tutorial.trigger.update"));
    var ctime = registry.get("system.timestamp");
    simulator.reset();
    simulator.initialize();
    registry.set("system.timestamp", ctime, false);
    notify("start", {});
};

scripting.include("fault_definitions.js");
scripting.include("secondary_tasks.js");
scripting.include("automation.js");
scripting.include("adaptive.js");
scripting.include("interface.js");
scripting.include("tutorial.js");

notify("start", {});
