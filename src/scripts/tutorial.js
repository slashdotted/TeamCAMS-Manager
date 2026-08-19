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
registry.set("tutorial.trigger.generic", "", true);
registry.set("tutorial.trigger.update", "", true);
registry.set("tutorial.trigger.log", "", true);

// TODO: Consider multiple users? i.e resume only when all users have completed the action
// But then the user will not get an immediate feedback and might think that he's doing wrong

// TODO: deprecate this
var displayWebOverlay = function (url, showCloseButton, width, height) {
    notify("showWebOverlay", {
        "url": url,
        "showCloseButton": showCloseButton,
        "width": width,
        "height": height
    });
    execution.pause();
    registry.set("tutorial.trigger.generic", triggers.activate("webOverlayCompleted", "resumeAfterWebTutorial(_ctx)"));
};

var displayPopup = function (contentUrl, contentTitle) {
    notify("showPopup", {
        "url": contentUrl,
        "title": contentTitle
    });
    execution.pause();
    registry.set("tutorial.trigger.generic", triggers.activate("popupCompleted", "resumeAfterPopup(_ctx)"));
};

var expectUpdateToValue = function (propertyName, propertyValue) {
    filter.blockIncomingUpdate("*", "!" + propertyName, true);
    execution.pause();
    registry.set("tutorial.trigger.update", triggers.activate("<update>", "evaluateUpdate(_ctx, \"" + propertyName + "\"," + propertyValue + ")"));
};

var expectUpdate = function (propertyName) {
    filter.blockIncomingUpdate("*", "!" + propertyName, true);
    execution.pause();
    registry.set("tutorial.trigger.update", triggers.activate("<update>", "unexpectUpdate(_ctx, \"" + propertyName + "\")"));
};

var expectLog = function (tag) {
    filter.blockIncomingTrigger("*", "!" + tag, true);
    execution.pause();
    registry.set("tutorial.trigger.log", triggers.activate("<log>", "unexpectLog(_ctx, \"" + tag + "\")"));
};

var expectLogText = function (tag, text) {
    filter.blockIncomingTrigger("*", "!" + tag, true);
    execution.pause();
    registry.set("tutorial.trigger.log", triggers.activate("<log>", "unexpectLogText(_ctx, \"" + tag + "\", \"" + text + "\")"));
};

var expectLogKeys = function (tag, keys) {
    filter.blockIncomingTrigger("*", "!" + tag, true);
    execution.pause();
    registry.set("tutorial.trigger.log", triggers.activate("<log>", "unexpectLogKeys(_ctx, \"" + tag + "\", " + JSON.stringify(keys) + ")"));
};

var expectLogKeyValues = function (tag, keyvalues) {
    filter.blockIncomingTrigger("*", "!" + tag, true);
    execution.pause();
    registry.set("tutorial.trigger.log", triggers.activate("<log>", "unexpectLogKeyValues(_ctx, \"" + tag + "\", " + JSON.stringify(keyvalues) + ")"));
};

var expectLogKeyAnyValue = function (tag, keyvalues) {
    filter.blockIncomingTrigger("*", "!" + tag, true);
    execution.pause();
    registry.set("tutorial.trigger.log", triggers.activate("<log>", "unexpectLogKeyAnyValue(_ctx, \"" + tag + "\", " + JSON.stringify(keyvalues) + ")"));
};

var expectLogKeyNotValues = function (tag, keyvalues) {
    filter.blockIncomingTrigger("*", "!" + tag, true);
    execution.pause();
    registry.set("tutorial.trigger.log", triggers.activate("<log>", "unexpectLogKeyNotValues(_ctx, \"" + tag + "\", " + JSON.stringify(keyvalues) + ")"));
};

var resumeAfterImageTutorial = function (_ctx) {
    triggers.deactivateByUuid(registry.get("tutorial.trigger.generic"));
    execution.resume();
    notify("hideImageOverlay", {});
};

var resumeAfterWebTutorial = function (_ctx) {
    triggers.deactivateByUuid(registry.get("tutorial.trigger.generic"));
    execution.resume();
    notify("hideWebOverlay", {});
};

var resumeAfterPopup = function (_ctx) {
    triggers.deactivateByUuid(registry.get("tutorial.trigger.generic"));
    execution.resume();
    notify("hidePopup", {});
};

var unexpectLog = function (_ctx, tag) {
    if (_ctx.tag === tag) { // tag can be, for example userui
        triggers.deactivateByUuid(registry.get("tutorial.trigger.log"));
        filter.unblockIncomingTrigger("*", "!" + tag, true);
        execution.resume();
        notify("hideImageOverlay", {});
    }
};

var unexpectLogText = function (_ctx, tag, text) {
    if (_ctx.tag === tag && _ctx.text === text) {
        notify("hideImageOverlay", {});
        triggers.deactivateByUuid(registry.get("tutorial.trigger.log"));
        filter.unblockIncomingTrigger("*", "!" + tag, true);
        execution.resume();
        notify("hideImageOverlay", {});
    }
};

var unexpectLogKeys = function (_ctx, tag, keys) {
    var data = JSON.parse(_ctx.text);
    if (_ctx.tag === tag && keys.includes(data.key)) {
        notify("hideImageOverlay", {});
        triggers.deactivateByUuid(registry.get("tutorial.trigger.log"));
        filter.unblockIncomingTrigger("*", "!" + tag, true);
        execution.resume();
        notify("hideImageOverlay", {});
    }
};

var unexpectLogKeyValues = function (_ctx, tag, keyvalues) {
    var data = JSON.parse(_ctx.text);
    if ((_ctx.tag === tag) && (data.key in keyvalues) && (keyvalues[data.key] === data.value)) {
        notify("hideImageOverlay", {});
        triggers.deactivateByUuid(registry.get("tutorial.trigger.log"));
        filter.unblockIncomingTrigger("*", "!" + tag, true);
        execution.resume();
        notify("hideImageOverlay", {});
    }
};

var unexpectLogKeyNotValues = function (_ctx, tag, keyvalues) {
    var data = JSON.parse(_ctx.text);
    if ((_ctx.tag === tag) && (data.key in keyvalues) && (keyvalues[data.key] !== data.value)) {
        notify("hideImageOverlay", {});
        triggers.deactivateByUuid(registry.get("tutorial.trigger.log"));
        filter.unblockIncomingTrigger("*", "!" + tag, true);
        execution.resume();
        notify("hideImageOverlay", {});
    }
};

var unexpectLogKeyAnyValue = function (_ctx, tag, keyvalues) {
    var data = JSON.parse(_ctx.text);
    if ((_ctx.tag === tag) && (data.key in keyvalues)) {
        notify("hideImageOverlay", {});
        triggers.deactivateByUuid(registry.get("tutorial.trigger.log"));
        filter.unblockIncomingTrigger("*", "!" + tag, true);
        execution.resume();
        notify("hideImageOverlay", {});
    }
};

var unexpectUpdate = function (_ctx, propertyName) {
    if (_ctx.key === propertyName && _ctx.sender !== "<system>") {
        triggers.deactivateByUuid(registry.get("tutorial.trigger.update"));
        filter.unblockIncomingUpdate("*", "!" + propertyName, true);
        execution.resume();
        notify("hideImageOverlay", {});
    }
};

var evaluateUpdate = function (_ctx, propertyName, propertyValue) {
    if (_ctx.value === propertyValue && _ctx.sender !== "<system>") {
        triggers.deactivateByUuid(registry.get("tutorial.trigger.update"));
        filter.unblockIncomingUpdate("*", "!" + propertyName, true);
        execution.resume();
        notify("hideImageOverlay", {});
    } else if (_ctx.sender !== "<system>") {
        registry.touch(propertyName);
    }
};
