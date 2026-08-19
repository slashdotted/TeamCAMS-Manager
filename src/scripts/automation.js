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
// Return next higher LOA
// **************************************************************************************
var getNextLOA = function () {
    var LOAs = "loa1,loa2,loa3,loa4,loa5,loa6".split(",");
    var currentLOA = registry.get("automation.loa.active");
    var validLOAs = registry.get("automation.loa.enabled");
    var next = LOAs.indexOf(currentLOA) + 1;
    while (next < LOAs.length) {
        if (validLOAs.indexOf(LOAs[next]) >= 0) {
            return LOAs[next];
        }
        next += 1;
    }
    return "";
};

// **************************************************************************************
// Return next lower LOA
// **************************************************************************************
var getPrevLOA = function () {
    var LOAs = "loa6,loa5,loa4,loa3,loa2,loa1".split(",");
    var currentLOA = registry.get("automation.loa.active");
    var validLOAs = registry.get("automation.loa.enabled");
    var next = LOAs.indexOf(currentLOA) + 1;
    while (next < LOAs.length) {
        if (validLOAs.indexOf(LOAs[next]) >= 0) {
            return LOAs[next];
        }
        next += 1;
    }
    return "";
};

// **************************************************************************************
// Get next higher LOAs
// **************************************************************************************
var getNextLOAs = function () {
    var LOAs = "loa1,loa2,loa3,loa4,loa5,loa6".split(",");
    var currentLOA = registry.get("automation.loa.active");
    var validLOAs = registry.get("automation.loa.enabled");
    var nextLOAsList = [];
    var next = LOAs.indexOf(currentLOA) + 1;
    while (next < LOAs.length) {
        if (validLOAs.indexOf(LOAs[next]) >= 0) {
            nextLOAsList.push(LOAs[next]);
        }
        next += 1;
    }
    return nextLOAsList.toString();
};

// **************************************************************************************
// Get next lower LOAs
// **************************************************************************************
var getPrevLOAs = function () {
    var LOAs = "loa6,loa5,loa4,loa3,loa2,loa1".split(",");
    var currentLOA = registry.get("automation.loa.active");
    var validLOAs = registry.get("automation.loa.enabled");
    var nextLOAsList = [];
    var next = LOAs.indexOf(currentLOA) + 1;
    while (next < LOAs.length) {
        if (validLOAs.indexOf(LOAs[next]) >= 0) {
            nextLOAsList.push(LOAs[next]);
        }
        next += 1;
    }
    return nextLOAsList.toString();
};

// **************************************************************************************
// Get all other valid LOAs apart from the current one
// **************************************************************************************
var getOtherLOAs = function () {
    var LOAs = "loa1,loa2,loa3,loa4,loa5,loa6".split(",");
    var currentLOA = registry.get("automation.loa.active");
    var validLOAs = registry.get("automation.loa.enabled");
    var otherLOAsList = [];
    var next = 0;
    while (next < LOAs.length) {
        if ((validLOAs.indexOf(LOAs[next]) >= 0) && (LOAs[next] !== currentLOA)) {
            otherLOAsList.push(LOAs[next]);
        }
        next += 1;
    }
    return otherLOAsList.toString();
};

// **************************************************************************************
// Get all valid LOAs apart from the current one
// **************************************************************************************
var getValidLOAs = function () {
    var LOAs = "loa1,loa2,loa3,loa4,loa5,loa6".split(",");
    var currentLOA = registry.get("automation.loa.active");
    var validLOAs = registry.get("automation.loa.enabled");
    var otherLOAsList = [];
    var next = 0;
    while (next < LOAs.length) {
        if (validLOAs.indexOf(LOAs[next]) >= 0) {
            otherLOAsList.push(LOAs[next]);
        }
        next += 1;
    }
    return otherLOAsList.toString();
};

// **************************************************************************************
// Scripted assistance change notification
// **************************************************************************************
var assistanceChange = function (_ctx, parameters) {
    if ("type" in parameters) {
        switch (parameters.type) {
        case "user":
            // Let the user select the assistance level
            var validloas = ("valid" in parameters) ? parameters.valid : "loa1,loa2,loa3,loa4,loa5,loa6";
            registry.set("automation.loa.enabled", validloas);
            break;
        case "ask":
            // Shows a pop-up dialog asking the user to select an assistance level
            var loas = getOtherLOAs();
            if (loas === "")
                return;
            notify("showLOAChangeNotificationDialog", {
                "message": "You may want to change the level of assistance (LOA)." + "<br/>Please select a level of assistance",
                "activeLOA": registry.get("automation.loa.active"),
                "enabledLOAs": loas,
                "showSelector": true,
                "showCloseButton": false,
                "buttonTimeout": false
            });
            break;
        case "force":
            var loas = getOtherLOAs();
            if (loas === "")
                return;
            if (registry.get("automation.loa.active") === "loa1")
                return;
            registry.set("automation.loa.loabeforeforce", registry.get("automation.loa.active"));
            // Notifies the user that he/she needs to change the assistance level within 30s,
            // or the system will switch to a lower assistance automatically (has no effect if current assistance is LOA_1)
            notify("showLOAChangeNotificationDialog", {
                "message": "If you do not change the level of assistance (LOA) within 30s, " + "it will automatically decrease.",
                "activeLOA": registry.get("automation.loa.active"),
                "enabledLOAs": loas,
                "showSelector": true,
                "showCloseButton": true,
                "buttonTimeout": -1
            });
            scheduler.schedule(_ctx._timestamp + 30, "decreaseAssistanceIfNotChanged(_ctx)");
            break;
        default:
            // When an assistance level is forced by a script, the selector in the main window is disabled.
            if (parameters.notify == "true") {
                notify("showLOAChangeNotificationDialog", {
                    "message": "The level of automation will automatically change to " + parameters.type + " in 15 seconds",
                    "showSelector": false,
                    "activeLOA": "",
                    "enabledLOAs": "",
                    "showCloseButton": true,
                    "buttonTimeout": 15
                });
                scheduler.schedule(_ctx._timestamp + 15, "registry.set(\"automation.loa.active\", \"" + parameters.type + "\"); notify(\"hideLOAChangeNotificationDialog\",{})");
            } else {
                registry.set("automation.loa.active", parameters.type);
            }
        }
    }
};

// **************************************************************************************
// Check if LOA changed, otherwise lower assistance level
// **************************************************************************************
var decreaseAssistanceIfNotChanged = function (_ctx) {
    var loaBefore = registry.get("automation.loa.loabeforeforce");
    var currentloa = registry.get("automation.loa.active");
    if (currentloa === loaBefore) {
        var prevloa = getPrevLOA();
        if (prevloa !== "") {
            registry.set("automation.loa.active", prevloa);
        }
        notify("hideLOAChangeNotificationDialog", {});
    }
};

// **************************************************************************************
// Assistance proposition notification
// **************************************************************************************
var assistanceNotification = function (_ctx) {
    var currentloa = registry.get("automation.loa.active");
    var currentFault = registry.get("system.error");
    var currentRepair = registry.get("system.repair.fault");
    var notifyint = registry.get("automation.loa.notify"); // notify interval
    var dismissed = registry.get("automation.loa.dismissed");
    var repeatNotify = registry.get("automation.loa.repeatnotify"); // repeat notifications?
    var nextLOAs = getNextLOAs();
    var detected = registry.get("system.error.detected");
    if (detected && !dismissed) {
        if (currentFault !== "" && currentRepair === "") {
            if (currentloa === "loa1" && (nextLOAs !== "")) {
                /* Currently no automation is used */
                notify("showLOAChangeNotificationDialog", {
                    "message": "A failure was detected but not yet repaired." + "<br/>You may want to activate assistance to help you. <br/>Activate a level of assistance (LOA):",
                    "showSelector": true,
                    "activeLOA": registry.get("automation.loa.active"),
                    "enabledLOAs": nextLOAs,
                    "showCloseButton": true,
                    "buttonTimeout": -1
                });
            } else if (currentloa === "loa5" && (nextLOAs !== "")) {
                notify("showLOAChangeNotificationDialog", {
                    "message": "A failure was detected but not yet repaired." + "You should repair faulty components by clicking on the \"Repair\" button or " + "you may want the assistance system to repair them " + "for you by clicking on the \"Accept\" button." + "<br/>Otherwise you could increase the level of assistance (LOA) to help you. " + "Select a higher level of assistance:",
                    "showSelector": true,
                    "activeLOA": registry.get("automation.loa.active"),
                    "enabledLOAs": nextLOAs,
                    "showCloseButton": true,
                    "buttonTimeout": -1
                });
            } else if (currentloa === "loa5" && (nextLOAs === "")) {
                notify("showLOAChangeNotificationDialog", {
                    "message": "A failure was detected but not yet repaired." + "<br/>You should repair faulty components by clicking on the \"Repair\" button" + "<br/>or you may want the assistance system to repair them" + "<br/>for you by clicking on the \"Accept\" button",
                    "showSelector": false,
                    "activeLOA": "",
                    "enabledLOAs": "",
                    "showCloseButton": true,
                    "buttonTimeout": -1
                });
            } else if (currentloa === "loa6") {
                notify("showLOAChangeNotificationDialog", {
                    "message": "A failure was detected but not yet repaired." + "<br/>You should repair faulty components by clicking on the \"Repair\" button",
                    "showSelector": false,
                    "activeLOA": registry.get("automation.loa.active"),
                    "enabledLOAs": nextLOAs,
                    "showCloseButton": true,
                    "buttonTimeout": -1
                });
            } else if (nextLOAs !== "") {
                /* Other LOAs */
                notify("showLOAChangeNotificationDialog", {
                    "message": "A failure was detected but not yet repaired." + "<br/>You may want to follow instructions of the assistance or" + "<br/>you could increase the level of assistance (LOA) to help you:",
                    "activeLOA": registry.get("automation.loa.active"),
                    "showSelector": true,
                    "enabledLOAs": nextLOAs,
                    "showCloseButton": true,
                    "buttonTimeout": -1
                });
            }
        }
    }
    if (!detected || repeatNotify) {
        scheduler.schedule(_ctx._timestamp + notifyint, "assistanceNotification(_ctx)", _ctx._eventid);
    }
};
