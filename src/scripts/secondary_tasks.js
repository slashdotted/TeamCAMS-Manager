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
// Secondary task activity values
// **************************************************************************************
registry.set("secondarytask.n2.interval", 60, true);
registry.set("secondarytask.n2.beforeafter", 15, true);
registry.set("secondarytask.transmission.interval", 30, true);
registry.set("secondarytask.transmission.visible", 10, true);

// **************************************************************************************
// Secondary task activity (transmission check)
// **************************************************************************************
var transmissionCheckPing = function (_ctx) {
    var interval = registry.get("secondarytask.transmission.interval");
    var cpayload = {};
    notify("transmissioncheck", cpayload);
    logging.log("secondary:transmission.notify", "<system>", JSON.stringify(connection.connectedUsers()));
    var next = _ctx._timestamp + Math.random() * interval + interval;
    scheduler.schedule(next, "transmissionCheckPing(_ctx)");
    scheduler.schedule(_ctx._timestamp + registry.get("secondarytask.transmission.visible"), "notify(\"transmissioncheckreset\", {})");
};

var transmissionCheckPong = function (_ctx) {
    if (_ctx.responseTime >= 0) {
        // Log reply time
        var data = {
            "responseTime": _ctx.responseTime,
            "appeared": _ctx.appeared,
            "clicked": _ctx.clicked
        };
        logging.log("secondary:transmission.response", _ctx._sender, JSON.stringify(data));
        if (registry.exists("secondarytask.transmission.history")) {
            var history = registry.get("secondarytask.transmission.history");
            history.push([_ctx._timestamp, _ctx.responseTime]);
            registry.set("secondarytask.transmission.history", history);
        }
    } else {
        // Log unwanted transmission check / or invalid (if the sim was ever paused)
        logging.log("secondary:transmission.invalid", _ctx._sender, _ctx.isValid ? "invalid" : "unexpected");
    }
};

notify("transmissioncheckreset", {}); // Ensure that clients clear the transmission check lamp
scheduler.schedule(registry.get("secondarytask.transmission.interval"), "transmissionCheckPing(_ctx)");
triggers.activate("transmissioncheck", "transmissionCheckPong(_ctx)");

// **************************************************************************************
// Secondary task activity (N2 Log)
// **************************************************************************************
var n2loggingShow = function (_ctx) {
    var beforeafter = registry.get("secondarytask.n2.beforeafter");
    var interval = registry.get("secondarytask.n2.interval");
    var cpayload = {};
    cpayload["due"] = _ctx._timestamp + beforeafter;
    notify("n2log", cpayload);
    logging.log("secondary:n2.notify", "<system>", JSON.stringify(connection.connectedUsers()));
    scheduler.schedule(_ctx._timestamp + 2 * beforeafter, "notify(\"n2loghide\", {})");
    scheduler.schedule(_ctx._timestamp + interval, "n2loggingShow(_ctx)");
};

var n2loggingReceived = function (_ctx) {
    if (_ctx.dueTime < 0) {
        logging.log("secondary:n2.invalid", _ctx._sender, "unexpected");
    } else {
        var amount = registry.get("components.n2tank.volume");
        var difference = _ctx.n2Amount - amount;
        var data = {
            "logged": _ctx.n2Amount,
            "real": amount,
            "difference": difference,
            "due": _ctx.dueTime,
            "responseTime": _ctx.responseTime
        };
        logging.log("secondary:n2.response", _ctx._sender, JSON.stringify(data));
        if (registry.exists("secondarytask.n2.history")) {
            var history = registry.get("secondarytask.n2.history");
            history.push([_ctx._timestamp, _ctx.responseTime]);
            registry.set("secondarytask.n2.history", history);
        }
    }
};

//scheduler.schedule(0, "n2loggingRequest(_ctx)")
var n2loggingfirsttime = registry.get("secondarytask.n2.interval") - registry.get("secondarytask.n2.beforeafter");
scheduler.schedule(n2loggingfirsttime, "n2loggingShow(_ctx)");
triggers.activate("n2log", "n2loggingReceived(_ctx)");

// **************************************************************************************
// Messaging system (chat)
// **************************************************************************************
registry.set("secondarytasks.messaging.users", connection.connectedUserAliases(), true);

triggers.activate("<connection>", "updateUserList(_ctx)");
triggers.activate("<disconnection>", "updateUserList(_ctx)");
triggers.activate("<aliaschanged>", "updateUserList(_ctx)");
var updateUserList = function (_ctx) {
    var users = connection.connectedUserAliases();
    registry.set("secondarytasks.messaging.users", users);
};

var chatMessageReceived = function (_ctx) {
    // Sender and recipients are chat aliases
    var data = {
        "from": _ctx.sender,
        "message": _ctx.message,
        "recipient": _ctx.recipient
    };
    logging.log("secondary:chat.message", _ctx._sender, JSON.stringify(data));
    var cpayload = {};
    cpayload["sender"] = _ctx.sender;
    cpayload["message"] = _ctx.message;
    cpayload["recipient"] = _ctx.recipient;
    notify("chat", cpayload);
};

var sendChatMessage = function (sender, message, recipient) {
    // Sender and recipients are chat aliases (empty string means to all users)
    var data = {
        "from": sender,
        "message": message,
        "recipient": recipient
    };
    logging.log("secondary:chat.message", "<system>", JSON.stringify(data));
    var cpayload = {};
    cpayload["sender"] = sender;
    cpayload["message"] = message;
    cpayload["recipient"] = recipient;
    notify("chat", cpayload);
};

triggers.activate("chat", "chatMessageReceived(_ctx)");
