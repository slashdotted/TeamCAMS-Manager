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
// System error and repair properties
// **************************************************************************************
registry.set("system.error.misdiagnosis", "", true);
registry.set("system.error", "", true);
registry.set("system.error.label", "", true);
registry.set("system.repair.label", "", true);
registry.set("system.repair.fault", "", true);
registry.set("system.error.validrepairs", "", true);
registry.set("system.error.repairtime", 60, true);
registry.set("system.error.eid", "", true);
registry.set("system.error.fake", 0, true);
registry.set("system.error.detected", false, true);
registry.set("system.error.alarm", false, true);
registry.set("system.error.activationtime", -1, true);
registry.set("system.error.detectiontime", -1, true);
registry.set("system.error.detectiondelay", 10, true);
registry.set("system.error.allowlaterepair", false, true);
registry.set("system.survey.expectedreplies", -1, true);

// **************************************************************************************
// Register repair triggers
// **************************************************************************************
triggers.activate("mixer_block repair", "new MixerBlockFault().repair(_ctx, false)");
triggers.activate("o2_valve_block_repair", "new OxygenBlockFault().repair(_ctx, false)");
triggers.activate("o2_valve_leak_repair", "new OxygenLeakFault().repair(_ctx, false)");
triggers.activate("o2_switch_point_failure_repair", "new OxygenSensorFault().repair(_ctx, false)");
triggers.activate("n2_switch_point_failure_repair", "new PressureSensorFault().repair(_ctx, false)");
triggers.activate("o2_valve_stuck_open_repair", "new OxygenStuckOpenFault().repair(_ctx, false)");
triggers.activate("n2_valve_block_repair", "new NitrogenBlockFault().repair(_ctx, false)");
triggers.activate("n2_valve_leak_repair", "new NitrogenLeakFault().repair(_ctx, false)");
triggers.activate("n2_valve_stuck_open_repair", "new NitrogenStuckOpenFault().repair(_ctx, false)");
triggers.activate("mixer_block_repair", "new MixerBlockFault().repair(_ctx, false)");
triggers.activate("dehumidifier_stuck_on_repair", "new DehumidifierStuckOnFault().repair(_ctx, false)");
triggers.activate("dehumidifier_switch_point_failure_repair", "new HumiditySensorFault().repair(_ctx, false)");
triggers.activate("dehumidifier_ineffective_repair", "new DehumidifierIneffectiveFault().repair(_ctx, false)");
triggers.activate("scrubber_stuck_on_repair", "new ScrubberStuckOnFault().repair(_ctx, false)");
triggers.activate("scrubber_switch_point_failure_repair", "new ScrubberSensorFault().repair(_ctx, false)");
triggers.activate("scrubber_ineffective_repair", "new ScrubberIneffectiveFault().repair(_ctx, false)");
triggers.activate("vent_stuck_on_repair", "new VentStuckOnFault().repair(_ctx, false)");
triggers.activate("vent_switch_point_failure_repair", "new VentSensorFault().repair(_ctx, false)");
triggers.activate("vent_ineffective_repair", "new VentIneffectiveFault().repair(_ctx, false)");
triggers.activate("heater_stuck_on_repair", "new HeaterStuckOnFault().repair(_ctx, false)");
triggers.activate("heater_switch_point_failure_repair", "new HeaterSensorFault().repair(_ctx, false)");
triggers.activate("heater_ineffective_repair", "new HeaterIneffectiveFault().repair(_ctx, false)");
triggers.activate("cooler_stuck_on_repair", "new CoolerStuckOnFault().repair(_ctx, false)");
triggers.activate("cooler_switch_point_failure_repair", "new CoolerSensorFault().repair(_ctx, false)");
triggers.activate("cooler_ineffective_repair", "new CoolerIneffectiveFault().repair(_ctx, false)");

var assertProperty = function (prop, value) {
    var rvalue = registry.get(prop);
    if (rvalue !== value)
        logging.critical(prop + " = " + rvalue + " != " + value);
};

var verifyErrorCondition = function (errName) {
    assertProperty("system.error.fake", 0);
    assertProperty("system.error.detected", true);
    assertProperty("system.error.alarm", true);
    assertProperty("system.error", errName);
};

var verifySystemIsNormal = function () {
    if (registry.get("components.cabin.o2.relvalue") > 20.5) {
        logging.critical("O2 too high: " + registry.get("components.cabin.o2.relvalue") + " > 20.5");
    }
    if (registry.get("components.cabin.o2.relvalue") < 19) {
        logging.critical("O2 too low: " + registry.get("components.cabin.o2.relvalue") + " < 19");
    }
    if (registry.get("components.cabin.co2.relvalue") > 0.8) {
        logging.critical("CO2 too high: " + registry.get("components.cabin.co2.relvalue") + " > 0.8");
    }
    if (registry.get("components.cabin.co2.relvalue") < 0.1) {
        logging.critical("CO2 too low: " + registry.get("components.cabin.co2.relvalue") + " < 0.1");
    }
    if (registry.get("components.cabin.pressure") > 1.040) {
        logging.critical("Pressure too high: " + registry.get("components.cabin.pressure") + " > 1.040");
    }
    if (registry.get("components.cabin.pressure") < 0.970) {
        logging.critical("Pressure too low: " + registry.get("components.cabin.pressure") + " < 0.970");
    }
    if (registry.get("components.cabin.temperature") > 23) {
        logging.critical("Temperature too high: " + registry.get("components.cabin.temperature") + " > 23");
    }
    if (registry.get("components.cabin.temperature") < 18.5) {
        logging.critical("Temperature too low: " + registry.get("components.cabin.temperature") + " < 18.5");
    }
    if (registry.get("components.cabin.humidity") > 44) {
        logging.critical("Humidity too high: " + registry.get("components.cabin.humidity") + " > 44");
    }
    if (registry.get("components.cabin.humidity") < 36.5) {
        logging.critical("Humidity too low: " + registry.get("components.cabin.humidity") + " < 36.5");
    }
    assertProperty("system.error", "");
    assertProperty("system.error.label", "");
    assertProperty("system.repair.label", "");
    assertProperty("system.repair.fault", "");
    if (registry.stuckProperties().length > 0) {
        logging.critical("Still some stuck properties: " + registry.stuckProperties());
    }
};

// **************************************************************************************
// Activate diagnostics
// **************************************************************************************
var diagnoseError = function (_ctx) {
    // level 1: alarm is triggered, assistance is not activated
    // level 2: assistance is activated, but alarm is not triggered
    // level 3: both assistance and alarm are triggered
    if (registry.get("system.error.misdiagnosis") === "") {
        if (registry.get("system.error", this.errorName) === "")
            return;
    }
    var fakeLevel = registry.get("system.error.fake");
    switch (fakeLevel) {
    case 1:
        registry.set("system.error.alarm", true);
        registry.set("system.error.detectiontime", _ctx._timestamp);
        break;
    case 2:
        registry.set("system.error.alarm", false);
        registry.set("system.error.detected", true);
        registry.set("system.error.detectiontime", _ctx._timestamp);
        break;
    case 3:
        registry.set("system.error.alarm", true);
        registry.set("system.error.detected", true);
        registry.set("system.error.detectiontime", _ctx._timestamp);
        break;
    default:
        registry.set("system.error.alarm", true);
        registry.set("system.error.detected", true);
        registry.set("system.error.detectiontime", _ctx._timestamp);
    }
};

// **************************************************************************************
// Reset regulation parameters
// **************************************************************************************
var resetRegulation = function (_ctx) {
    registry.set("control.o2.state", "auto");
    registry.set("control.pressure.state", "auto");
    registry.set("control.ventilation.state", "auto");
    registry.set("control.temperature.state", "auto");
    registry.set("control.humidity.state", "auto");
    registry.set("control.scrubber.state", "auto");
    registry.set("components.heater.strength", "standard");
    registry.set("components.cooler.strength", "standard");
    registry.set("components.dehumidifier.strength", "standard");
    registry.set("components.scrubber.strength", "standard");
    registry.set("components.vent.strength", "standard");
    registry.set("components.o2valve.strength", "standard");
    registry.set("components.n2valve.strength", "standard");
};

// **************************************************************************************
// Generic fault (base class for all faults)
// **************************************************************************************
var finalizeErrorSituation = function (_ctx) {
    triggers.deactivateByUuid(registry.get("system.error.eid"));
    scheduler.unscheduleByUuid(registry.get("system.error.eid"));
    registry.set("automation.loa.notify", -1);
    registry.set("automation.loa.repeatnotify", false);
    registry.set("system.repair.label", "");
    registry.set("system.repair.fault", "");
    registry.set("system.error.detected", false);
    registry.set("system.error.fake", 0);
    registry.set("system.error.alarm", false);
    registry.set("system.error", "");
    registry.set("system.error.misdiagnosis", "");
    registry.set("system.error.label", "");
    registry.set("system.error.eid", "");
    registry.set("system.error.detectiontime", -1);
    registry.set("automation.loa.enabled", "loa1,loa2,loa3,loa4,loa5,loa6");
    registry.set("automation.loa.afterrepair.delay", -1);
    registry.set("automation.loa.afterrepair", "");
    clearSuggestions(_ctx);
    resetRegulation(_ctx);
};

// **************************************************************************************
// Generic fault (base class for all faults)
// **************************************************************************************
var generic_fault = function () {};

generic_fault.prototype.errorName = "generic_fault";
generic_fault.prototype.errorMessage = qsTr("Generic fault");
generic_fault.prototype.errorOn = [];
generic_fault.prototype.errorOff = [];
generic_fault.prototype.assistedControlConfigInitial = [];
generic_fault.prototype.assistedControlConfigFinal = [];
generic_fault.prototype.validRepairs = [];
generic_fault.prototype.suggestions = [qsTr("Suggestion 1."), qsTr("Suggestion 2."), qsTr("Suggestion 3.")];

// **************************************************************************************
// Generic fault activation
// **************************************************************************************
generic_fault.prototype.activate = function (_ctx, parameters) {
    scheduler.unscheduleByType("loaendmessage");
    registry.set("automation.loa.endmessage", "");

    /* If the system is still in an error condition, postpone event */
    if (registry.get("system.error") !== "") {
        scheduler.postpone(1, _ctx._eventid);
        return;
    }

    /* Parse parameters */
    var diagnostics = ("diagnostics" in parameters) ? parameters.diagnostics : "true";
    var fake = ("fake" in parameters) ? parameters.fake : 0;
    var clear = ("clear" in parameters) ? parameters.clear : -1;
    var loaAfterRepair = ("loa" in parameters) ? parameters.loaAfterRepair : "";
    var loaDelay = ("loadelay" in parameters) ? parameters.loaDelay : -1;
    var notify = ("notify" in parameters) ? parameters.notify : -1;
    var repeatNotify = ("repeatNotify" in parameters) ? parameters.repeatNotify : false;
    var allowLateRepair = ("allowLateRepair" in parameters) ? parameters.allowLateRepair : false;

    var survey = ("survey" in parameters) ? parameters.survey : "";
    var surveyOk = ("surveyOk" in parameters) ? parameters.surveyOk : "";
    var surveyDelay = ("surveyDelay" in parameters) ? parameters.surveyDelay : "";

    /* Log failure activation */
    var data = {
        "fault": this.errorName,
        "diagnostics": diagnostics,
        "fake": fake
    };
    logging.log("failure:start", "<system>", JSON.stringify(data));

    /* LOA suggestions */
    if (diagnostics === "true" || diagnostics === "miss") {
        registry.set("automation.loa.suggestion.1", this.suggestions.length > 0 ? this.suggestions[0] : "");
        registry.set("automation.loa.suggestion.2", this.suggestions.length > 1 ? this.suggestions[1] : "");
        registry.set("automation.loa.suggestion.3", this.suggestions.length > 2 ? this.suggestions[2] : "");
        registry.set("automation.loa.suggestion.count", this.suggestions.length);
        registry.set("system.error.label", this.errorMessage);
    } else {
        var alterror = eval("new " + diagnostics + "()");
        registry.set("automation.loa.suggestion.1", alterror.suggestions.length > 0 ? alterror.suggestions[0] : "");
        registry.set("automation.loa.suggestion.2", alterror.suggestions.length > 1 ? alterror.suggestions[1] : "");
        registry.set("automation.loa.suggestion.3", alterror.suggestions.length > 2 ? alterror.suggestions[2] : "");
        registry.set("automation.loa.suggestion.count", alterror.suggestions.length);
        registry.set("system.error.label", diagnostics);
        registry.set("system.error.misdiagnosis", diagnostics);
    }

    registry.set("system.error.fake", fake);

    /* Clear after X seconds */
    if (clear > 0) {
        scheduler.schedule(_ctx._timestamp + clear, "new " + this.errorName + "().clear(_ctx)", _ctx._eventid);
    }

    /* If notify register notify() reminder */
    if (notify > 0) {
        registry.set("automation.loa.repeatnotify", repeatNotify);
        registry.set("automation.loa.notify", notify);
        scheduler.schedule(_ctx._timestamp + notify, "assistanceNotification(_ctx)", _ctx._eventid);
    } else {
        registry.set("automation.loa.repeatnotify", false);
        registry.set("automation.loa.notify", -1);
    }

    registry.set("automation.loa.dismissed", false);

    /* Should the problem be diagnosed ? */
    if (diagnostics !== "miss") {
        var detectiondelay = registry.get("system.error.detectiondelay");
        scheduler.schedule(_ctx._timestamp + detectiondelay, "diagnoseError(_ctx)", _ctx._eventid);
    }

    registry.set("system.error.activationtime", _ctx._timestamp);
    registry.set("system.error", this.errorName);
    registry.set("system.error.eid", _ctx._eventid);

    /* Triggers for automation */
    triggers.activate("loa accept", "new " + this.errorName + "().loa_accept(_ctx)", _ctx._eventid);
    triggers.activate("loa cancel", "new " + this.errorName + "().loa_cancel(_ctx)", _ctx._eventid);
    triggers.activate("loa veto", "new " + this.errorName + "().loa_veto(_ctx)", _ctx._eventid);
    registry.set("automation.loa.afterrepair", loaAfterRepair);
    registry.set("automation.loa.afterrepair.delay", loaDelay);

    if (fake <= 0) {
        this.enableError(_ctx, parameters);
        registry.set("system.error.allowlaterepair", allowLateRepair);
        if (this.validRepairs.length === 0) {
            registry.set("system.error.validrepairs", this.errorName);
        } else {
            registry.set("system.error.validrepairs", this.validRepairs.join());
        }
        registry.set("automation.survey.questions", survey);
        registry.set("automation.survey.questionsok", surveyOk);
        registry.set("automation.survey.delay", surveyDelay);
    }
};

// **************************************************************************************
// Generic fault repair request
// **************************************************************************************
generic_fault.prototype.repair = function (_ctx, loainitiated) {
    registry.set("system.repair.label", this.errorMessage);
    registry.set("system.repair.fault", this.errorName);
    var repair_time = registry.get("system.error.repairtime");
    var valid_repairs = registry.get("system.error.validrepairs").split(",");
    if (this.errorName === "") {
        // Repair in normal situation
        if (!loainitiated) {
            var data = {
                "fault": this.errorName
            };
            logging.log("failure:repair-wrong-nofault", _ctx._sender, JSON.stringify(data));
        } else {
            var data = {
                "fault": this.errorName
            };
            logging.log("failure:loa-repair-wrong-nofault", _ctx._sender, JSON.stringify(data));
        }
        scheduler.schedule(_ctx._timestamp + repair_time, "new " + this.errorName + "().wrongrepaired(_ctx)", "", "repair");
    } else if (valid_repairs.indexOf(this.errorName) < 0) {
        // Wrong repair
        // Schedule end of repair
        var data = {
            "fault": this.errorName
        };
        if (!loainitiated) {
            var data = {
                "fault": this.errorName
            };
            logging.log("failure:repair-wrong", _ctx._sender, JSON.stringify(data));
        } else {
            var data = {
                "fault": this.errorName
            };
            logging.log("failure:loa-repair-wrong", _ctx._sender, JSON.stringify(data));
        }
        scheduler.schedule(_ctx._timestamp + repair_time, "new " + this.errorName + "().wrongrepaired(_ctx)", "", "repair");
    } else {
        // Correct repair
        // Schedule end of repair
        if (!loainitiated) {
            var data = {
                "fault": this.errorName
            };
            logging.log("failure:repair-correct", _ctx._sender, JSON.stringify(data));
        } else {
            var data = {
                "fault": this.errorName
            };
            logging.log("failure:loa-repair-correct", _ctx._sender, JSON.stringify(data));
        }
        scheduler.schedule(_ctx._timestamp + repair_time, "new " + this.errorName + "().repaired(_ctx, " + loainitiated + ")", "", "repair");
    }
};

// **************************************************************************************
// Generic fault clear
// **************************************************************************************
generic_fault.prototype.clear = function (_ctx) {
    var lateRepair = registry.get("system.error.allowlaterepair");
    if (lateRepair) {
        var valid_repairs = registry.get("system.error.validrepairs").split(",");
        if (valid_repairs.indexOf(registry.get("system.repair.fault")) >= 0) {
            return;
        }
    }
    this.disableError(_ctx);
    finalizeErrorSituation(_ctx);
    logging.log("failure:cleared", "<system>", this.errorName);
    registry.set("automation.loa.repeatnotify", false);
    registry.set("automation.loa.notify", -1);

    var questions = registry.get("automation.survey.questions");
    if (questions !== "") {
        var qpayload = {};
        var questionList = questions.split(",");
        qpayload["message"] = registry.get("automation.survey.question.heading");
        var qobjs = [];
        for (var i = 0; i < questionList.length; i++) {
            var qid = questionList[i].trim().toLowerCase();
            var qkey = "automation.survey.question." + qid;
            if (registry.exists(qkey)) {
                qobjs.push({
                    "id": qid,
                    "text": registry.get(qkey)
                });
            }
        }
        if (qobjs.length > 0) {
            qpayload["questions"] = qobjs;
            triggers.activate("survey", "processSurveyReply(_ctx)");
            registry.set("system.survey.expectedreplies", realConnectedUsers().length);
            var delay = registry.get("automation.survey.delay");
            if (delay > 0) {
                experiment.schedule(_ctx._timestamp + delay, "execution.pause(); notify(\"survey\", " + qpayload + " );");
            } else {
                execution.pause();
                notify("showSurveyDialog", qpayload);
            }
        }
    }

    registry.set("automation.loa.afterrepair", "");
    registry.set("automation.loa.afterrepair.delay", -1);
    notify("hideLOAChangeNotificationDialog", {});
};

// **************************************************************************************
// Enable error "callback"
// **************************************************************************************
generic_fault.prototype.enableError = function (_ctx, parameters) {
    /* Activate fault */
    for (var i = 0; i < this.errorOn.length; i++) {
        registry.set(this.errorOn[i][0], this.errorOn[i][1]);
    }
};

// **************************************************************************************
// Enable error "callback"
// **************************************************************************************
generic_fault.prototype.disableError = function (_ctx) {
    /* Activate fault */
    for (var i = 0; i < this.errorOff.length; i++) {
        registry.set(this.errorOff[i][0], this.errorOff[i][1]);
    }
};

// **************************************************************************************
// Fault repaired "callback"
// **************************************************************************************
generic_fault.prototype.repaired = function (_ctx, loainitiated) {
    this.disableError(_ctx);
    if (!loainitiated) {
        logging.log("repair:correct-finished", "<system>", this.errorName);
        var loaafter = registry.get("automation.loa.afterrepair");
        var loaafterdelay = registry.get("automation.loa.afterrepair.delay");
        if (loaafter !== "" && loaafterdelay >= 0) {
            scheduler.schedule(_ctx._timestamp + loaafterdelay, "assistanceChange(_ctx, { \"type\" : " + loaafter + ", \"notify\" : false })");
        }
        var questions = registry.get("automation.survey.questionsok");
        if (questions !== "") {
            var qpayload = {};
            var questionList = questions.split(",");
            qpayload["message"] = registry.get("automation.survey.question.headingok");
            var qobjs = [];
            for (var i = 0; i < questionList.length; i++) {
                var qid = questionList[i].trim().toLowerCase();
                var qkey = "automation.survey.question." + qid;
                if (registry.exists(qkey)) {
                    qobjs.push({
                        "id": qid,
                        "text": registry.get(qkey)
                    });
                }
            }
            if (qobjs.length > 0) {
                qpayload["questions"] = qobjs;
                triggers.activate("survey", "processSurveyReply(_ctx)");

                execution.pause();
                notify("showSurveyDialog", qpayload);
            }
        }
    }
    finalizeErrorSituation(_ctx);
};

// **************************************************************************************
// Fault wrong repaired "callback"
// **************************************************************************************
generic_fault.prototype.wrongrepaired = function (_ctx) {
    // log wrong repair finished
    registry.set("system.repair.label", "");
    logging.log("repair:wrong-finished", "<system>", this.errorName);
};

// **************************************************************************************
// Assisted parameter control secondary "callback"
// **************************************************************************************
generic_fault.prototype.performAssistedControl = function (_ctx) {};

// **************************************************************************************
// Fault LOA accept "callback"
// **************************************************************************************
generic_fault.prototype.loa_accept = function (_ctx) {
    logging.log("loa:accept", _ctx.sender, "");
    registry.set("automation.loa.enabled", "loa5");
    registry.set("automation.loa.active", "loa5");
    notify("loa accept", {});
    resetRegulation(_ctx);
    this.setupAssistedControl(_ctx);
    var misdiagnosed = registry.get("system.error.misdiagnosis");
    if (misdiagnosed !== "") {
        scheduler.schedule(1, 1, "new " + misdiagnosed + "().assistedControl(_ctx)", "", "loacontrol");
        scheduler.schedule(1, "new " + misdiagnosed + "().repair(_ctx, true)", "", "repair");
    } else {
        scheduler.schedule(1, 1, "new " + this.errorName + "().assistedControl(_ctx)", "", "loacontrol");
        scheduler.schedule(1, "new " + this.errorName + "().repair(_ctx, true)", "", "repair");
    }
};

// **************************************************************************************
// Fault LOA cancel "callback"
// **************************************************************************************
generic_fault.prototype.loa_cancel = function (_ctx) {
    notify("loa cancel", {});
    logging.log("loa:cancel", _ctx.sender, "");
    registry.set("automation.loa.dismissed", true);
};

// **************************************************************************************
// Fault LOA6 veto "callback"
// **************************************************************************************
generic_fault.prototype.loa_veto = function (_ctx) {
    notify("loa veto", {});
    logging.log("loa:veto", _ctx.sender, "");
    registry.set("automation.loa.repeatnotify", false);
    registry.set("automation.loa.dismissed", true);
};

// **************************************************************************************
// Assisted parameter control "callback"
// **************************************************************************************
generic_fault.prototype.setupAssistedControl = function (_ctx) {
    for (var i = 0; i < this.assistedControlConfigInitial.length; i++) {
        registry.set(this.assistedControlConfigInitial[i][0], this.assistedControlConfigInitial[i][1]);
    }
};

// **************************************************************************************
// Assisted parameter primary "callback"
// **************************************************************************************
generic_fault.prototype.assistedControl = function (_ctx) {
    if (registry.get("system.error") !== "") {
        this.performAssistedControl(_ctx);
    } else {
        // End of error situation
        for (var i = 0; i < this.assistedControlConfigFinal.length; i++) {
            registry.set(this.assistedControlConfigFinal[i][0], this.assistedControlConfigFinal[i][1]);
        }
        registry.set("automation.loa.message", "");
        registry.set("automation.loa.endmessage", qsTr("<center>Repair executed.<br/>System reset to standard settings.</center>"));
        scheduler.schedule(_ctx._timestamp + 10, "hideLOAEndMessage(_ctx)", "", "loamessage");
        scheduler.unscheduleByType("loacontrol");
    }
};

// **************************************************************************************
// Hide LOA "all done" message
// **************************************************************************************
var hideLOAEndMessage = function (_ctx) {
    registry.set("automation.loa.endmessage", "");
};

// **************************************************************************************
// Switch LOA
// **************************************************************************************
var selectLOA = function (_ctx, level) {
    if (level !== registry.get("automation.loa.active")) {
        logging.log("loa:change", _ctx._sender, level);
        var enabledLOA = registry.get("automation.loa.enabled").split(",");
        if (enabledLOA.indexOf(level) >= 0) {
            registry.set("automation.loa.active", level);
            registry.set("automation.loa.dismissed", false);
        }
    }
};

// **************************************************************************************
// Survey results "callback"
// **************************************************************************************
processSurveyReply = function (_ctx) {
    logging.log("survey:answers", _ctx._sender, JSON.stringify(_ctx.answers));
    // Wait for all surveys then resume execution
    var remaining = registry.get("system.survey.expectedreplies") - 1;
    registry.set("system.survey.expectedreplies", remaining);
    if (remaining === 0) {
        execution.resume();
        triggers.deactivateByType("survey");
    }
};

// **************************************************************************************
// Fault and assistance related properties
// **************************************************************************************
registry.set("automation.loa.suggestion.1", "", true);
registry.set("automation.loa.suggestion.2", "", true);
registry.set("automation.loa.suggestion.3", "", true);
registry.set("automation.loa.suggestion.count", 0, true);
registry.set("automation.loa.afterrepair", "", true);
registry.set("automation.loa.afterrepair.delay", -1, true);
registry.set("automation.loa.active", "loa1", true);
registry.set("automation.loa.enabled", "loa1,loa2,loa3,loa4,loa5,loa6", true);
registry.set("automation.loa.dismissed", false, true);
registry.set("automation.loa.message", "", true);
registry.set("automation.loa.endmessage", "", true);
registry.set("automation.loa.notify", -1, true);
registry.set("automation.loa.repeatnotify", false, true);
registry.set("automation.loa.loabeforeforce", "", true);
registry.set("automation.survey.questions", "", true);
registry.set("automation.survey.delay", 0, true);
registry.set("automation.survey.questionsok", "", true);
registry.set("automation.survey.question.heading", "To give your answers please click for each scale on a button that corresponds at best to your current state of feeling <br><br> (1= not at all, 7= completely)", true);
registry.set("automation.survey.question.headingok", "To give your answers please click for each scale on a button that corresponds at best to your current state of feeling <br><br> (1= not at all, 7= completely)", true);

// **************************************************************************************
// LOA selection triggers
// **************************************************************************************
triggers.activate("loa1 select", "selectLOA(_ctx, \"loa1\")");
triggers.activate("loa2 select", "selectLOA(_ctx, \"loa2\")");
triggers.activate("loa3 select", "selectLOA(_ctx, \"loa3\")");
triggers.activate("loa4 select", "selectLOA(_ctx, \"loa4\")");
triggers.activate("loa5 select", "selectLOA(_ctx, \"loa5\")");
triggers.activate("loa6 select", "selectLOA(_ctx, \"loa6\")");

// **************************************************************************************
// Clear LOA suggestions
// **************************************************************************************
var clearSuggestions = function (_ctx) {
    registry.set("automation.loa.suggestion.1", "");
    registry.set("automation.loa.suggestion.2", "");
    registry.set("automation.loa.suggestion.3", "");
    registry.set("automation.loa.suggestion.count", 0);
};

// **************************************************************************************
// Fault include
// **************************************************************************************

scripting.include("cooler_ineffective_fault.js");
scripting.include("cooler_sensor_fault.js");
scripting.include("cooler_stuckon_fault.js");
scripting.include("dehumidifier_ineffective_fault.js");
scripting.include("dehumidifier_stuckon_fault.js");
scripting.include("heater_ineffective_fault.js");
scripting.include("heater_sensor_fault.js");
scripting.include("heater_stuckon_fault.js");
scripting.include("humidity_sensor_fault.js");
scripting.include("mixer_block_fault.js");
scripting.include("nitrogen_block_fault.js");
scripting.include("nitrogen_leak_fault.js");
scripting.include("nitrogen_stuckopen_fault.js");
scripting.include("oxygen_block_fault.js");
scripting.include("oxygen_leak_fault.js");
scripting.include("oxygen_sensor_fault.js");
scripting.include("oxygen_stuckopen_fault.js");
scripting.include("pressure_sensor_fault.js");
scripting.include("scrubber_ineffective_fault.js");
scripting.include("scrubber_sensor_fault.js");
scripting.include("scrubber_stuckon_fault.js");
scripting.include("vent_ineffective_fault.js");
scripting.include("vent_sensor_fault.js");
scripting.include("vent_stuckon_fault.js");

// **************************************************************************************
// Fault aliases
// **************************************************************************************

var ox_valve_leack = OxygenLeakFault;
var ox_valve_leak = OxygenLeakFault;
var ox_valve_block = OxygenBlockFault;
var ox_stuck_open = OxygenStuckOpenFault;
var ox_sensor_upper = OxygenSensorFaultUpper;
var ox_sensor_lower = OxygenSensorFaultLower;
var ox_sensor = OxygenSensorFault;
var ni_valve_leack = NitrogenLeakFault;
var ni_valve_leak = NitrogenLeakFault;
var ni_valve_block = NitrogenBlockFault;
var ni_stuck_open = NitrogenStuckOpenFault;
var ni_sensor_upper = PressureSensorFaultUpper;
var ni_sensor_lower = PressureSensorFaultLower;
var ni_sensor = PressureSensorFault;
var mixer_block = MixerBlockFault;
var dehum_sensor_lower = HumiditySensorFaultLower;
var dehum_sensor_upper = HumiditySensorFaultUpper;
var cool_sensor_lower = CoolerSensorFaultLower;
var heat_sensor_lower = HeaterSensorFaultLower;
var cool_sensor_upper = CoolerSensorFaultUpper;
var heat_sensor_upper = HeaterSensorFaultUpper;
var scrub_sensor_lower = ScrubberSensorFaultLower;
var scrub_sensor_upper = ScrubberSensorFaultUpper;
var vent_sensor_lower = VentSensorFaultLower;
var vent_sensor_upper = VentSensorFaultUpper;
var dehum_ineff = DehumidifierIneffectiveFault;
var scrub_ineff = ScrubberIneffectiveFault;
var vent_ineff = VentIneffectiveFault;
var cool_ineff = CoolerIneffectiveFault;
var heat_ineff = HeaterIneffectiveFault;
var dehum_stuck_on = DehumidifierStuckOnFault;
var scrub_stuck_on = ScrubberStuckOnFault;
var cool_stuck_on = CoolerStuckOnFault;
var heat_stuck_on = HeaterStuckOnFault;
var vent_stuck_on = VentStuckOnFault;
var dehum_sensor = HumiditySensorFault;
var scrub_sensor = ScrubberSensorFault;
var cool_sensor = CoolerSensorFault;
var heat_sensor = HeaterSensorFault;
var vent_sensor = VentSensorFault;
