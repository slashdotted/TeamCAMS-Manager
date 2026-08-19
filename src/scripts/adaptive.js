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
// Adaptive data logging
// **************************************************************************************
registry.set("secondarytask.n2.history", [], true);
registry.set("secondarytask.transmission.history", [], true);
// [transmission|logging|combined|predefinedtransmission|predefinedlogging|none]
registry.set("adaptive.enabled", false, true);
registry.set("adaptive.type", "", true);
registry.set("adaptive.reactionbaselinetransmission", -1, true);
registry.set("adaptive.reactionbaselinetransmissionduringfault", -1, true);
registry.set("adaptive.reactionbaselinelogging", -1, true);
registry.set("adaptive.reactionbaselineloggingduringfault", -1, true);
registry.set("adaptive.adaptivecontrolmaxchange", 0, true);
registry.set("adaptive.reactiontimeloggingpredefinedintervals_a", 0, true);
registry.set("adaptive.reactiontimeloggingpredefinedintervals_b", 0, true);
registry.set("adaptive.reactiontimeloggingpredefinedintervals_c", 0, true);
registry.set("adaptive.reactiontimeloggingpredefinedintervals_d", 0, true);
registry.set("adaptive.reactiontimeloggingpredefinedintervals_e", 0, true);
registry.set("adaptive.reactiontimetransmissionpredefinedintervals_a", 0, true);
registry.set("adaptive.reactiontimetransmissionpredefinedintervals_b", 0, true);
registry.set("adaptive.reactiontimetransmissionpredefinedintervals_c", 0, true);
registry.set("adaptive.reactiontimetransmissionpredefinedintervals_d", 0, true);
registry.set("adaptive.reactiontimetransmissionpredefinedintervals_e", 0, true);

// **************************************************************************************
// Adaptive data logging
// **************************************************************************************

// Entry point for adaptive control
var adaptiveControl = function (ctx, parameters) {
    if (!registry.get("adaptive.enabled"))
        return;
    var adaptiveControlMaxChange = ("adaptiveControlMaxChange" in parameters) ? parameters.adaptiveControlMaxChange : registry.get("adaptive.adaptivecontrolmaxchange");
    var adaptiveControlParam = ("adaptiveControl" in parameters) ? parameters.adaptiveControlParam : registry.get("adaptive.type");
    var reactionbaselinetransmission = ("reactionbaselinetransmission" in parameters) ? parameters.reactionbaselinetransmission : registry.get("adaptive.reactionbaselinetransmission");
    var reactionbaselinetransmissionDuringFault = ("reactionbaselinetransmissionDuringFault" in parameters) ? parameters.reactionbaselinetransmissionDuringFault : registry.get("adaptive.reactionbaselinetransmissionduringfault");
    var reactionbaselinelogging = ("reactionbaselinelogging" in parameters) ? parameters.reactionbaselinelogging : registry.get("adaptive.reactionbaselinelogging");
    var reactionbaselineloggingduringfault = ("reactionbaselineloggingduringfault" in parameters) ? parameters.reactionbaselineloggingduringfault : registry.get("adaptive.reactionbaselineloggingduringfault");
    var n2A = ("reactionTimeLoggingPredefinedIntervalsA" in parameters) ? parameters.reactionTimeLoggingPredefinedIntervalsA : registry.get("adaptive.reactiontimeloggingpredefinedintervals_a");
    var n2B = ("reactionTimeLoggingPredefinedIntervalsB" in parameters) ? parameters.reactionTimeLoggingPredefinedIntervalsB : registry.get("adaptive.reactiontimeloggingpredefinedintervals_b");
    var n2C = ("reactionTimeLoggingPredefinedIntervalsC" in parameters) ? parameters.reactionTimeLoggingPredefinedIntervalsC : registry.get("adaptive.reactiontimeloggingpredefinedintervals_c");
    var n2D = ("reactionTimeLoggingPredefinedIntervalsD" in parameters) ? parameters.reactionTimeLoggingPredefinedIntervalsD : registry.get("adaptive.reactiontimeloggingpredefinedintervals_d");
    var n2E = ("reactionTimeLoggingPredefinedIntervalsE" in parameters) ? parameters.reactionTimeLoggingPredefinedIntervalsE : registry.get("adaptive.reactiontimeloggingpredefinedintervals_e");
    var trA = ("reactionTimeTransmissionPredefinedIntervalsA" in parameters) ? parameters.reactionTimeTransmissionPredefinedIntervalsA : registry.get("adaptive.reactiontimetransmissionpredefinedintervals_a");
    var trB = ("reactionTimeTransmissionPredefinedIntervalsB" in parameters) ? parameters.reactionTimeTransmissionPredefinedIntervalsB : registry.get("adaptive.reactiontimetransmissionpredefinedintervals_b");
    var trC = ("reactionTimeTransmissionPredefinedIntervalsC" in parameters) ? parameters.reactionTimeTransmissionPredefinedIntervalsC : registry.get("adaptive.reactiontimetransmissionpredefinedintervals_c");
    var trD = ("reactionTimeTransmissionPredefinedIntervalsD" in parameters) ? parameters.reactionTimeTransmissionPredefinedIntervalsD : registry.get("adaptive.reactiontimetransmissionpredefinedintervals_d");
    var trE = ("reactionTimeTransmissionPredefinedIntervalsE" in parameters) ? parameters.reactionTimeTransmissionPredefinedIntervalsE : registry.get("adaptive.reactiontimetransmissionpredefinedintervals_e");
    var onPeriodFrom = ("onPeriodFrom" in parameters) ? parameters.onPeriodFrom : -1;
    var onPeriodTo = ("onPeriodTo" in parameters) ? parameters.onPeriodTo : -1;
    var onSamples = ("onSamples" in parameters) ? parameters.onSamples : -1;
    var onPast = ("onPast" in parameters) ? parameters.onPast : -1;

    var n2History = registry.get("secondarytask.n2.history");
    var transmissionHistory = registry.get("secondarytask.transmission.history");

    var n2Data = [];
    var transmissionData = [];
    if (onPeriodFrom >= 0) {
        n2Data = getSamplesFromPeriod(n2History, onPeriodFrom, onPeriodTo);
        transmissionData = getSamplesFromPeriod(transmissionHistory, onPeriodFrom, onPeriodTo);
    } else if (onSamples > 0) {
        n2Data = getLastNSamples(n2History, onSamples);
        transmissionData = getLastNSamples(transmissionHistory, onSamples);
    } else if (onPast > 0) {
        n2Data = getLastPeriodSamples(n2History, _ctx._timestamp, onPast);
        transmissionData = getLastPeriodSamples(transmissionHistory, _ctx._timestamp, onPast);
    } else {
        return;
    }

    var stdevN2 = getStdDev(n2Data);
    var stdevTr = getStdDev(transmissionData);

    var baselineTr = -1;
    var baselineN2 = -1;
    if (registry.get("system.error") !== "") {
        baselineTr = reactionbaselinetransmissionDuringFault;
        baselineN2 = reactionbaselineloggingduringfault;
    } else {
        baselineTr = reactionbaselinetransmission;
        baselineN2 = reactionbaselinetransmissionDuringFault;
    }

    switch (adaptiveControlParam) {
    case "transmission":
        if (stdevTr > baselineTr * 2.0) {
            adaptiveIncreaseLOA(_ctx);
        } else if (stdevTr < baseLineTr / 2.0) {
            adaptiveDecreaseLOA(_ctx);
        } else {
            adaptiveMaintainLOA(_ctx);
        }
        break;
    case "logging":
        if (stdevN2 > baselineN2 * 2.0) {
            adaptiveIncreaseLOA(_ctx);
        } else if (stdevN2 < baseLineN2 / 2.0) {
            adaptiveDecreaseLOA(_ctx);
        } else {
            adaptiveMaintainLOA(_ctx);
        }
        break;
    case "combined":
        var decision = 0;
        if (stdevTr > baselineTr * 2.0) {
            decision += 1;
        } else if (stdevTr < baseLineTr / 2.0) {
            decision -= 1;
        }
        if (stdevN2 > baselineN2 * 2.0) {
            decision += 1;
        } else if (stdevN2 < baseLineN2 / 2.0) {
            decision -= 1;
        }
        if (decision < 0) {
            adaptiveDecreaseLOA(_ctx);
        } else if (decision > 0) {
            adaptiveIncreaseLOA(_ctx);
        } else {
            adaptiveMaintainLOA(_ctx);
        }
        break;
    case "predefinedtransmission":
        if (stdevTr >= trA && stdev < trB) {
            adaptiveChangeLOA(_ctx, "loa1", adaptiveControlMaxChange);
        } else if (stdevTr >= trB && stdev < trC) {
            adaptiveChangeLOA(_ctx, "loa2", adaptiveControlMaxChange);
        } else if (stdevTr >= trC && stdev < trD) {
            adaptiveChangeLOA(_ctx, "loa3", adaptiveControlMaxChange);
        } else if (stdevTr >= trD && stdev < trE) {
            adaptiveChangeLOA(_ctx, "loa4", adaptiveControlMaxChange);
        } else if (stdevTr >= trE) {
            adaptiveChangeLOA(_ctx, "loa5", adaptiveControlMaxChange);
        }
        break;
    case "predefinedlogging":
        if (stdevN2 >= n2A && stdev < n2B) {
            adaptiveChangeLOA(_ctx, "loa1", adaptiveControlMaxChange);
        } else if (stdevN2 >= n2B && stdev < n2C) {
            adaptiveChangeLOA(_ctx, "loa2", adaptiveControlMaxChange);
        } else if (stdevN2 >= n2C && stdev < n2D) {
            adaptiveChangeLOA(_ctx, "loa3", adaptiveControlMaxChange);
        } else if (stdevN2 >= n2D && stdev < n2E) {
            adaptiveChangeLOA(_ctx, "loa4", adaptiveControlMaxChange);
        } else if (stdevN2 >= n2E) {
            adaptiveChangeLOA(_ctx, "loa5", adaptiveControlMaxChange);
        }
        break;
    case "none":
        // Do nothing
        break;
    }
};

// **************************************************************************************
// Adaptive control decisions
// **************************************************************************************

var adaptiveIncreaseLOA = function (ctx) {
    var currentloa = registry.get("automation.loa.active");
    var nextloa = getNextLOA();
    if (currentloa !== nextloa) {
        notify("showLOAChangeNotificationDialog", {
            "message": "The level of automation will automatically increase to " + nextloa + " in 15 seconds",
            "showSelector": false,
            "activeLOA": "",
            "enabledLOAs": "",
            "showCloseButton": true,
            "buttonTimeout": 15
        });
        scheduler.schedule(_ctx._timestamp + 15, "registry.set(\"automation.loa.active\", \"" + nextloa + "\")");
    }
};

var adaptiveDecreaseLOA = function (ctx) {
    var currentloa = registry.get("automation.loa.active");
    var prevloa = getNextLOA();
    if (currentloa !== prevloa) {
        notify("showLOAChangeNotificationDialog", {
            "message": "The level of automation will automatically decrease to " + prevloa + " in 15 seconds",
            "showSelector": false,
            "activeLOA": "",
            "enabledLOAs": "",
            "showCloseButton": true,
            "buttonTimeout": 15
        });
        scheduler.schedule(_ctx._timestamp + 15, "registry.set(\"automation.loa.active\", \"" + prevloa + "\")");
    }
};

var adaptiveMaintainLOA = function (ctx, maxchange) {
// do nothing
};

var adaptiveChangeLOA = function (ctx, targetloa, maxchange) {
    var currentloa = registry.get("automation.loa.active");
    var currentloaidx = "loa1,loa2,loa3,loa4,loa5,loa6".indexOf(currentloa);
    var nextloaidx = "loa1,loa2,loa3,loa4,loa5,loa6".indexOf(targetloa);
    if ((currentloaidx != nextloaidx) && (Math.abs(nextloaidx - currentloaidx) <= maxchange)) {
        notify("showLOAChangeNotificationDialog", {
            "message": "The level of automation will automatically change to " + targetloa + " in 15 seconds",
            "showSelector": false,
            "activeLOA": "",
            "enabledLOAs": "",
            "showCloseButton": true,
            "buttonTimeout": 15
        });
        scheduler.schedule(_ctx._timestamp + 15, "registry.set(\"automation.loa.active\", \"" + targetloa + "\")");
    }
};

// **************************************************************************************
// Adaptive control support functions
// **************************************************************************************

var getSamplesFromPeriod = function (samples, fromtime, totime) {
    var data = [];
    for (var i = 0; i < samples.length; i++) {
        var s = samples[i];
        if (s[0] >= fromtime && s[0] < totime) {
            data.push([s[0], s[1]]);
        }
    }
    return data;
};

var getLastPeriodSamples = function (samples, now, period) {
    var beginning = now * 1000 - period;
    var data = [];
    for (var i = 0; i < samples.length; i++) {
        var s = samples[i];
        if (s[0] >= beginning) {
            data.push([s[0], s[1]]);
        }
    }
    return data;
};

var getLastNSamples = function (samples, count) {
    return samples.slice(-count);
};

var getStdDev = function (samples) {
    var average = 0;
    for (var i = 0; i < samples.length; i++) {
        average += samples[i][1];
    }
    average = average / samples.length;
    var variance = 0;
    for (var i = 0; i < samples.length; i++) {
        variance += Math.pow(samples[i][1] - average, 2);
    }
    variance = variance / samples.length;
    return Math.sqrt(variance);
};
