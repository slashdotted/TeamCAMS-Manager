// TeamCAMS - reborn Cabin Air Management System
// Copyright (C) 2015-2026  Amos Brocco,
//                          Cognitive Ergonomics and Work Psychology Team,
//                          Psychology Department of Fribourg University,
//                          Switzerland / Department of Innovative Technologies
//                          University of Applied Sciences and Arts of Southern
//                          Switzerland, Contact: amos.brocco@supsi.ch
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
#include "xmlconfighandler.h"
#include <QMap>
#include <QStringList>

// TODO: Implement validation using XML Schema

static QString translateControlCommand(const QString &camsCmd);
static bool isValidInterfaceElement(const QString &element);
static bool isValidErrorType(const QString &type);
static bool isValidAdaptiveType(const QString &type);
static bool isValidLoaType(const QString &type);

struct XMLConfigHandler::pimpl {
  bool m_strict_verification{true};
  bool m_inscriptelement{false};
  unsigned int m_startTime{0};
  QString m_strbuilder;
  QStringList m_qids;
};

XMLConfigHandler::XMLConfigHandler(bool strict_verification, QObject *parent)
    : QObject{parent}, m_pimpl{std::make_unique<pimpl>()} {
  m_pimpl->m_strict_verification = strict_verification;
  m_pimpl->m_startTime = 0;
}

XMLConfigHandler::~XMLConfigHandler() = default;

void XMLConfigHandler::parseScriptElement(const QXmlStreamAttributes &atts) {
  bool isAdaptiveEnabled{false};
  QString adaptiveType{""};
  int adaptMaxChange{0};
  double baselineTx{0};
  double baselineTxFault{0};
  double baselineLog{0};
  double baselineLogFault{0};
  int logIntA{0};
  int logIntB{0};
  int logIntC{0};
  int logIntD{0};
  int logIntE{0};
  int txIntA{0};
  int txIntB{0};
  int txIntC{0};
  int txIntD{0};
  int txIntE{0};

  m_pimpl->m_inscriptelement = true;
  for (const auto &attribute : atts) {
    auto attName = attribute.name().toString();
    auto attValue = attribute.value().toString();
    if (attName == "name") {
      // not used
    } else if (attName == "adaptive") {
      if (attValue != "true" && attValue != "false") {
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed script command: invalid adaptive value (should be "
                 "true or false): %1")
                  .arg(attValue)};
      }
      isAdaptiveEnabled = (attValue == "true");
    } else if (attName == "adaptiveparams") {
      QStringList kvpairs{attValue.split(";")};
      for (const auto &kvp : kvpairs) {
        QStringList kv{kvp.split("=")};
        if (kv.length() == 2) {
          if (kv[0].toLower() == "adaptivecontrol") {
            if (isValidAdaptiveType(kv[1])) {
              adaptiveType = kv[1];
            } else {
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Invalid adaptive control type %s").arg(kv[1])};
            }
          } else if (kv[0].toLower() == "adaptivecontrolmaxchange") {
            QString value{kv[1]};
            bool ok;
            int maxchange{value.toInt(&ok)};
            if (maxchange < 0 || !ok)
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed script command: invalid maximum LOA changes: "
                       "%1")
                        .arg(kv[1])};
            adaptMaxChange = maxchange;
          } else if (kv[0].toLower() == "reactionbaselinetransmission") {
            QString value{kv[1]};
            bool ok;
            double ms{value.toDouble(&ok)};
            if (ms < 0 || !ok)
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed script command: invalid reaction time "
                       "baseline: %1")
                        .arg(kv[1])};
            baselineTx = ms;
          } else if (kv[0].toLower() ==
                     "reactionbaselinetransmissionduringfault") {
            QString value{kv[1]};
            bool ok;
            double ms{value.toDouble(&ok)};
            if (ms < 0 || !ok)
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed script command: invalid reaction time "
                       "baseline: %1")
                        .arg(kv[1])};
            baselineTxFault = ms;
          } else if (kv[0].toLower() == "reactionbaselinelogging") {
            QString value{kv[1]};
            bool ok;
            double ms{value.toDouble(&ok)};
            if (ms < 0 || !ok)
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed script command: invalid reaction time "
                       "baseline: %1")
                        .arg(kv[1])};
            baselineLog = ms;
          } else if (kv[0].toLower() == "reactionbaselineloggingduringfault") {
            QString value{kv[1]};
            bool ok;
            double ms{value.toDouble(&ok)};
            if (ms < 0 || !ok)
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed script command: invalid logging time "
                       "baseline: %1")
                        .arg(kv[1])};
            baselineLogFault = ms;
          } else if (kv[0].toLower() ==
                     "reactiontimeloggingpredefinedintervals") {
            QStringList intervals{kv[1].split(",")};
            if (intervals.length() == 5) {
              bool ok;
              int value{intervals[0].toInt(&ok)};
              if (value < 0 || !ok)
                if (m_pimpl->m_strict_verification)
                  throw XMLConfigHandlerException{
                      tr("Malformed script command: invalid logging time "
                         "interval: %1")
                          .arg(intervals[0])};
              logIntA = value;
              value = intervals[1].toInt(&ok);
              if (value < 0 || !ok)
                if (m_pimpl->m_strict_verification)
                  throw XMLConfigHandlerException{
                      tr("Malformed script command: invalid logging time "
                         "interval: %1")
                          .arg(intervals[1])};
              logIntB = value;
              value = intervals[2].toInt(&ok);
              if (value < 0 || !ok)
                if (m_pimpl->m_strict_verification)
                  throw XMLConfigHandlerException{
                      tr("Malformed script command: invalid logging time "
                         "interval: %1")
                          .arg(intervals[2])};
              logIntC = value;
              value = intervals[3].toInt(&ok);
              if (value < 0 || !ok)
                if (m_pimpl->m_strict_verification)
                  throw XMLConfigHandlerException{
                      tr("Malformed script command: invalid logging time "
                         "interval: %1")
                          .arg(intervals[3])};
              logIntD = value;
              value = intervals[4].toInt(&ok);
              if (value < 0 || !ok)
                if (m_pimpl->m_strict_verification)
                  throw XMLConfigHandlerException{
                      tr("Malformed script command: invalid logging time "
                         "interval: %1")
                          .arg(intervals[4])};
              logIntE = value;
              if (!(logIntA <= logIntB && logIntB <= logIntC &&
                    logIntC <= logIntD && logIntD <= logIntE)) {
                if (m_pimpl->m_strict_verification)
                  throw XMLConfigHandlerException{
                      tr("Malformed script command: invalid intervals for "
                         "adaptive control based on logging time: %1")
                          .arg(kv[1])};
              }
            } else {
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed script command: invalid number of intervals "
                       "for adaptive control based on logging: %1")
                        .arg(kv[1])};
            }
          } else if (kv[0].toLower() ==
                     "reactiontimetransmissionpredefinedintervals") {
            QStringList intervals{kv[1].split(",")};
            if (intervals.length() == 5) {
              bool ok;
              int value{intervals[0].toInt(&ok)};
              if (value < 0 || !ok)
                if (m_pimpl->m_strict_verification)
                  throw XMLConfigHandlerException{
                      tr("Malformed script command: invalid reaction time "
                         "interval: %1")
                          .arg(intervals[0])};
              txIntA = value;
              value = intervals[1].toInt(&ok);
              if (value < 0 || !ok)
                if (m_pimpl->m_strict_verification)
                  throw XMLConfigHandlerException{
                      tr("Malformed script command: invalid reaction time "
                         "interval: %1")
                          .arg(intervals[1])};
              txIntB = value;
              value = intervals[2].toInt(&ok);
              if (value < 0 || !ok)
                if (m_pimpl->m_strict_verification)
                  throw XMLConfigHandlerException{
                      tr("Malformed script command: invalid reaction time "
                         "interval: %1")
                          .arg(intervals[2])};
              txIntC = value;
              value = intervals[3].toInt(&ok);
              if (value < 0 || !ok)
                if (m_pimpl->m_strict_verification)
                  throw XMLConfigHandlerException{
                      tr("Malformed script command: invalid reaction time "
                         "interval: %1")
                          .arg(intervals[3])};
              txIntD = value;
              value = intervals[4].toInt(&ok);
              if (value < 0 || !ok)
                if (m_pimpl->m_strict_verification)
                  throw XMLConfigHandlerException{
                      tr("Malformed script command: invalid reaction time "
                         "interval: %1")
                          .arg(intervals[4])};
              txIntE = value;
              if (!(txIntA <= txIntB && txIntB <= txIntC && txIntC <= txIntD &&
                    txIntD <= txIntE)) {
                if (m_pimpl->m_strict_verification)
                  throw XMLConfigHandlerException{
                      tr("Malformed script command: invalid intervals for "
                         "adaptive control based on reaction time: %1")
                          .arg(kv[1])};
              }
            } else {
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed script command: invalid number of intervals "
                       "for adaptive control based on reaction time: %1")
                        .arg(kv[1])};
            }
          } else {
            if (m_pimpl->m_strict_verification)
              throw XMLConfigHandlerException{
                  tr("Malformed script command: invalid configuration key in "
                     "script node: %1")
                      .arg(kv[0])};
          }
        } else {
          if (m_pimpl->m_strict_verification)
            throw XMLConfigHandlerException{
                tr("Malformed script command: invalid attribute declaration: "
                   "%1")
                    .arg(attName)};
        }
      }
    }
  }
  QJsonObject parameters;
  parameters["adaptive"] = isAdaptiveEnabled;
  parameters["adaptiveControl"] = adaptiveType;
  parameters["adaptiveControlMaxChange"] = adaptMaxChange;
  parameters["reactionbaselinetransmission"] = baselineTx;
  parameters["reactionbaselinetransmissionDuringFault"] = baselineTxFault;
  parameters["reactionbaselinelogging"] = baselineLog;
  parameters["reactionbaselineloggingduringfault"] = baselineLogFault;
  parameters["reactiontimeloggingpredefinedintervals_a"] = logIntA;
  parameters["reactiontimeloggingpredefinedintervals_b"] = logIntB;
  parameters["reactiontimeloggingpredefinedintervals_c"] = logIntC;
  parameters["reactiontimeloggingpredefinedintervals_d"] = logIntD;
  parameters["reactiontimeloggingpredefinedintervals_e"] = logIntE;
  parameters["reactiontimetransmissionpredefinedintervals_a"] = txIntA;
  parameters["reactiontimetransmissionpredefinedintervals_b"] = txIntA;
  parameters["reactiontimetransmissionpredefinedintervals_c"] = txIntA;
  parameters["reactiontimetransmissionpredefinedintervals_d"] = txIntA;
  parameters["reactiontimetransmissionpredefinedintervals_e"] = txIntA;
  emit scriptElementRead(parameters);
}

void XMLConfigHandler::parseQuestionElement(const QXmlStreamAttributes &atts) {
  if (!m_pimpl->m_inscriptelement)
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{tr("Malformed XML: element 'question' "
                                         "should be within a script element")};
  QString id;
  QString text;
  bool isIdSet{false};
  bool isTextSet{false};
  for (const auto &attribute : atts) {
    auto attName = attribute.name().toString();
    auto attValue = attribute.value().toString();
    if (attName == "id") {
      id = attValue;
      isIdSet = true;
    } else if (attName == "text") {
      text = attValue;
      isTextSet = true;
    } else {
      if (m_pimpl->m_strict_verification)
        throw XMLConfigHandlerException{
            tr("Malformed question command: invalid question attribute: %1")
                .arg(attName)};
    }
  }
  if (!isIdSet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed question command: missing question id")};
  }
  if (!isTextSet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed question command: missing question text")};
  }
  if (m_pimpl->m_qids.contains(id)) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed question command: duplicate question id %1").arg(id)};
  } else {
    m_pimpl->m_qids.append(id);
    emit questionElementRead(id, text);
  }
}

void XMLConfigHandler::parseDurationElement(const QXmlStreamAttributes &atts) {
  if (!m_pimpl->m_inscriptelement)
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{tr("Malformed XML: element 'duration' "
                                         "should be within a script element")};
  unsigned int endTime{0};
  bool isSecondsSet{false};
  for (const auto &attribute : atts) {
    auto attName = attribute.name().toString();
    auto attValue = attribute.value().toString();
    if (attName == "seconds") {
      bool ok;
      endTime = attValue.toUInt(&ok);
      if (endTime == 0 || !ok)
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed duration command: invalid duration: %1")
                  .arg(attValue)};
      isSecondsSet = true;
    } else {
      if (m_pimpl->m_strict_verification)
        throw XMLConfigHandlerException{
            tr("Malformed duration command: invalid duration attribute: %1")
                .arg(attName)};
    }
  }
  if (!isSecondsSet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed duration command: missing seconds attribute in "
             "duration element")};
  }
  emit durationElementRead(endTime);
}

void XMLConfigHandler::parseAssistanceElement(
    const QXmlStreamAttributes &atts) {
  if (!m_pimpl->m_inscriptelement)
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{tr("Malformed XML: element 'assistance' "
                                         "should be within a script element")};
  unsigned int startTime{0};
  bool isStartTimeSet{false};
  bool isTypeSet{false};
  QJsonObject parameters;
  parameters["notify"] = "false";
  for (const auto &attribute : atts) {
    auto attName = attribute.name().toString();
    auto attValue = attribute.value().toString();
    if (attName == "type" || attName == "typ") {
      parameters["type"] = attValue;
      if (!isValidLoaType(attValue)) {
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed assistance command: invalid type: %1")
                  .arg(attValue)};
      }
      isTypeSet = true;
    } else if (attName == "start") {
      bool ok;
      startTime = attValue.toUInt(&ok);
      if (!ok)
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed assistance command: invalid start time: %1")
                  .arg(attValue)};
      isStartTimeSet = true;
    } else if (attName == "notify") {
      if (attValue != "true" && attValue != "false")
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed assistance command: invalid notify value: %1")
                  .arg(attValue)};
      parameters["notify"] = attValue;
    } else {
      if (m_pimpl->m_strict_verification)
        throw XMLConfigHandlerException{
            tr("Malformed assistance command: invalid assistance attribute: %1")
                .arg(attName)};
    }
  }
  if (!isStartTimeSet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{tr(
          "Malformed assistance command: missing assistance start attribute")};
  }
  if (!isTypeSet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed assistance command: missing type attribute")};
  }
  emit assistanceElementRead(startTime, parameters);
}

void XMLConfigHandler::parsePopupElement(const QXmlStreamAttributes &atts) {
  if (!m_pimpl->m_inscriptelement)
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{tr(
          "Malformed XML: element 'popup' should be within a script element")};
  unsigned int startTime{0};
  bool isStartTimeSet{false};
  bool isUrlSet{false};
  bool isTitleSet{false};
  QJsonObject parameters;
  for (const auto &attribute : atts) {
    auto attName = attribute.name().toString();
    auto attValue = attribute.value().toString();
    if (attName == "url") {
        parameters["url"] = attValue;
        isUrlSet = true;
    } else if (attName == "title") {
        parameters["title"] = attValue;
        isTitleSet = true;
    } else if (attName == "start") {
        bool ok;
        startTime = attValue.toUInt(&ok);
        if (!ok)
            if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed popup command: invalid start time: %1").arg(attValue)};
        isStartTimeSet = true;
    } else {
        if (m_pimpl->m_strict_verification)
            throw XMLConfigHandlerException{
                tr("Malformed popup command: invalid popup attribute: %1").arg(attName)};
    }
  }
  if (!isStartTimeSet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed popup command: missing popup start attribute")};
  }
  if (!isUrlSet) {
      if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{tr("Malformed popup command: missing url attribute")};
  }
  if (!isTitleSet) {
      if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{tr("Malformed popup command: missing title attribute")};
  }
  emit popupElementRead(startTime, parameters);
}

void XMLConfigHandler::parseInterfaceElement(const QXmlStreamAttributes &atts) {
  if (!m_pimpl->m_inscriptelement)
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{tr("Malformed XML: element 'interface' "
                                         "should be within a script element")};
  QString modifier;
  QString value;
  QString display;
  QString users;
  bool isStartTimeSet{false};
  bool isModifierSet{false};
  bool isDisplaySet{false};
  unsigned int startTime{0};
  for (const auto &attribute : atts) {
    auto attName = attribute.name().toString();
    auto attValue = attribute.value().toString();
    if (attName == "display") {
      display = attValue;
      if (!isValidInterfaceElement(display)) {
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed interface command: invalid interface element: %1")
                  .arg(display)};
      }
      isDisplaySet = true;
    } else if (attName == "visible") {
      if (isModifierSet) {
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed interface command: duplicate modifier (should be "
                 "either visible or enabled): %1")
                  .arg(attName)};
      }
      modifier = "visible";
      if (attValue != "true" && attValue != "false") {
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed interface command: invalid boolean attribute value "
                 "(should be true or false): %1")
                  .arg(attValue)};
      }
      value = attValue;
      isModifierSet = true;
    } else if (attName == "users") {
      users = attValue;
    } else if (attName == "enabled") {
      if (isModifierSet) {
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed interface command: duplicate modifier (should be "
                 "either visible or enabled): %1")
                  .arg(attName)};
      }
      modifier = "enabled";
      if (attValue != "true" && attValue != "false") {
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed interface command: invalid boolean attribute value "
                 "(should be true or false): %1")
                  .arg(attValue)};
      }
      value = attValue;
      isModifierSet = true;
    } else if (attName == "start") {
      bool ok;
      startTime = attValue.toUInt(&ok);
      if (!ok)
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed interface command: invalid start time: %1")
                  .arg(attValue)};
      isStartTimeSet = true;
    } else {
      if (m_pimpl->m_strict_verification)
        throw XMLConfigHandlerException{
            tr("Malformed interface command: invalid attribute: %1")
                .arg(attName)};
    }
  }
  if (!isDisplaySet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed interface command: missing display attribute")};
  }
  if (!isModifierSet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed interface command: missing modifier (visible or "
             "enabled) attribute")};
  }
  if (!isStartTimeSet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed interface command: missing start time")};
  }
  emit interfaceElementRead(startTime, display, modifier, value, users);
}

void XMLConfigHandler::parseControlElement(const QXmlStreamAttributes &atts) {
  if (!m_pimpl->m_inscriptelement)
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{tr("Malformed XML: element 'control' "
                                         "should be within a script element")};
  QString cmd;
  unsigned int startTime{0};
  bool isStartTimeSet{false};
  bool isCommandSet{false};
  for (const auto &attribute : atts) {
    auto attName = attribute.name().toString();
    auto attValue = attribute.value().toString();
    if (attName == "command") {
      cmd = translateControlCommand(attValue);
      if (cmd == "") {
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed control command: invalid command: %1")
                  .arg(attValue)};
      }
      isCommandSet = true;
    } else if (attName == "start") {
      bool ok;
      startTime = attValue.toUInt(&ok);
      if (!ok)
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed control command: invalid start time: %1")
                  .arg(attValue)};
      isStartTimeSet = true;
    } else {
      if (m_pimpl->m_strict_verification)
        throw XMLConfigHandlerException{
            tr("Malformed control command: invalid control attribute: %1")
                .arg(attName)};
    }
  }
  if (!isCommandSet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed control command: missing command")};
  }
  if (!isStartTimeSet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed control command: missing start time")};
  }
  if (cmd == "")
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Invalid control command element: %1").arg(cmd)};
  emit controlElementRead(startTime, cmd);
}

void XMLConfigHandler::parseAdaptiveElement(const QXmlStreamAttributes &atts) {
  if (!m_pimpl->m_inscriptelement)
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{tr("Malformed XML: element 'adaptive' "
                                         "should be within a script element")};
  QJsonObject parameters;
  unsigned int startTime{0};
  bool isCommandSet{false};
  bool isStartTimeSet{false};
  for (const auto &attribute : atts) {
    auto attName = attribute.name().toString();
    auto attValue = attribute.value().toString();
    if (attName == "command") {
      isCommandSet = true;
      QStringList atts{attValue.split(";")};
      for (QString &param : atts) {
        if (param.trimmed().isEmpty()) {
          // ok
        } else if (param.startsWith("onPeriod")) {
          QStringList kv{param.split(" ")};
          if (kv.length() == 3) {
            bool ok;
            parameters[kv[0] + "From"] = kv[1].toInt(&ok);
            if (!ok)
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed adaptive command: invalid onPeriod start "
                       "time: %1")
                        .arg(kv[1])};
            parameters[kv[0] + "To"] = kv[2].toInt(&ok);
            if (!ok)
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed adaptive command: invalid onPeriod end time: "
                       "%1")
                        .arg(kv[2])};
          } else {
            if (m_pimpl->m_strict_verification)
              throw XMLConfigHandlerException{
                  tr("Malformed adaptive command: invalid onPeriod directive: "
                     "%1")
                      .arg(attValue)};
          }
        } else if (param.startsWith("onSamples")) {
          QStringList kv{param.split(" ")};
          if (kv.length() == 2) {
            bool ok;
            int value{kv[1].toInt(&ok)};
            parameters[kv[0]] = value;
            if (value <= 0 || !ok)
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed adaptive command: invalid onSamples value: "
                       "%1")
                        .arg(kv[1])};
          } else {
            if (m_pimpl->m_strict_verification)
              throw XMLConfigHandlerException{
                  tr("Malformed adaptive command: invalid onSamples directive: "
                     "%1")
                      .arg(attValue)};
          }
        } else if (param == "onPast") {
          QStringList kv{param.split(" ")};
          if (kv.length() == 2) {
            bool ok;
            int value{kv[1].toInt(&ok)};
            parameters[kv[0]] = value;
            if (value <= 0 || !ok)
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed adaptive command: invalid onPast value: %1")
                        .arg(kv[1])};
          } else {
            if (m_pimpl->m_strict_verification)
              throw XMLConfigHandlerException{
                  tr("Malformed adaptive command: invalid onPast directive: %1")
                      .arg(attValue)};
          }
        } else if (param.startsWith("adaptiveControl")) {
          QStringList kv{param.split("=")};
          if (kv.length() == 2) {
            parameters[kv[0]] = kv[1];
          } else {
            if (m_pimpl->m_strict_verification)
              throw XMLConfigHandlerException{
                  tr("Malformed adaptive command: invalid adaptiveControl "
                     "directive: %1")
                      .arg(attValue)};
          }
        } else if (param.startsWith("reactionbaselinetransmission")) {
          QStringList kv{param.split("=")};
          if (kv.length() == 2) {
            QString value{kv[1]};
            bool ok;
            double ms{value.toDouble(&ok)};
            if (ms <= 0 || !ok)
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed adaptive command: invalid "
                       "reactionbaselinetransmission value: %1")
                        .arg(value)};
            parameters[kv[0]] = ms;
          } else {
            if (m_pimpl->m_strict_verification)
              throw XMLConfigHandlerException{
                  tr("Malformed adaptive command: invalid "
                     "reactionbaselinetransmission directive: %1")
                      .arg(attValue)};
          }
        } else if (param.startsWith(
                       "reactionbaselinetransmissionDuringFault")) {
          QStringList kv{param.split("=")};
          if (kv.length() == 2) {
            QString value{kv[1]};
            bool ok;
            double ms{value.toDouble(&ok)};
            if (ms <= 0 || !ok)
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed adaptive command: invalid "
                       "reactionbaselinetransmissionDuringFault value: %1")
                        .arg(value)};
            parameters[kv[0]] = ms;
          } else {
            if (m_pimpl->m_strict_verification)
              throw XMLConfigHandlerException{
                  tr("Malformed adaptive command: invalid "
                     "reactionbaselinetransmissionDuringFault directive: %1")
                      .arg(attValue)};
          }
        } else if (param == "reactionbaselinelogging") {
          QStringList kv{param.split("=")};
          if (kv.length() == 2) {
            QString value{kv[1]};
            bool ok;
            double ms{value.toDouble(&ok)};
            if (ms <= 0 || !ok)
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed adaptive command: invalid "
                       "reactionbaselinelogging value: %1")
                        .arg(value)};
            parameters[kv[0]] = ms;
          } else {
            if (m_pimpl->m_strict_verification)
              throw XMLConfigHandlerException{
                  tr("Malformed adaptive command: invalid "
                     "reactionbaselinelogging directive: %1")
                      .arg(attValue)};
          }
        } else if (param == "reactionbaselineloggingduringfault") {
          QStringList kv{param.split("=")};
          if (kv.length() == 2) {
            QString value{kv[1]};
            bool ok;
            double ms{value.toDouble(&ok)};
            if (ms <= 0 || !ok)
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed adaptive command: invalid "
                       "reactionbaselineloggingduringfault value: %1")
                        .arg(value)};
            parameters[kv[0]] = ms;
          } else {
            if (m_pimpl->m_strict_verification)
              throw XMLConfigHandlerException{
                  tr("Malformed adaptive command: invalid "
                     "reactionbaselineloggingduringfault directive: %1")
                      .arg(attValue)};
          }
        } else if (param == "reactionTimeLoggingPredefinedIntervals") {
          QStringList kv{param.split("=")};
          if (kv.length() == 2) {
            QStringList intervals{kv[1].split(",")};
            if (intervals.length() == 5) {
              bool ok;
              parameters["reactionTimeLoggingPredefinedIntervalsA"] =
                  intervals[0].toInt(&ok);
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed adaptive command: invalid "
                       "reactionTimeLoggingPredefinedIntervals value: %1")
                        .arg(intervals[0])};
              parameters["reactionTimeLoggingPredefinedIntervalsB"] =
                  intervals[1].toInt(&ok);
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed adaptive command: invalid "
                       "reactionTimeLoggingPredefinedIntervals value: %1")
                        .arg(intervals[1])};
              parameters["reactionTimeLoggingPredefinedIntervalsC"] =
                  intervals[2].toInt(&ok);
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed adaptive command: invalid "
                       "reactionTimeLoggingPredefinedIntervals value: %1")
                        .arg(intervals[2])};
              parameters["reactionTimeLoggingPredefinedIntervalsD"] =
                  intervals[3].toInt(&ok);
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed adaptive command: invalid "
                       "reactionTimeLoggingPredefinedIntervals value: %1")
                        .arg(intervals[3])};
              parameters["reactionTimeLoggingPredefinedIntervalsE"] =
                  intervals[4].toInt(&ok);
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed adaptive command: invalid "
                       "reactionTimeLoggingPredefinedIntervals value: %1")
                        .arg(intervals[4])};
            } else {
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed adaptive command: invalid "
                       "reactionTimeLoggingPredefinedIntervals directive: %1")
                        .arg(attValue)};
            }
          } else {
            if (m_pimpl->m_strict_verification)
              throw XMLConfigHandlerException{
                  tr("Malformed adaptive command: invalid "
                     "reactionTimeLoggingPredefinedIntervals directive: %1")
                      .arg(attValue)};
          }
        } else if (param == "reactionTimeTransmissionPredefinedIntervals") {
          QStringList kv{param.split("=")};
          if (kv.length() == 2) {
            QStringList intervals{kv[1].split(",")};
            if (intervals.length() == 5) {
              bool ok;
              parameters["reactionTimeTransmissionPredefinedIntervalsA"] =
                  intervals[0].toInt(&ok);
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed adaptive command: invalid "
                       "reactionTimeTransmissionPredefinedIntervals value: %1")
                        .arg(intervals[0])};
              parameters["reactionTimeTransmissionPredefinedIntervalsB"] =
                  intervals[1].toInt(&ok);
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed adaptive command: invalid "
                       "reactionTimeTransmissionPredefinedIntervals value: %1")
                        .arg(intervals[1])};
              parameters["reactionTimeTransmissionPredefinedIntervalsC"] =
                  intervals[2].toInt(&ok);
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed adaptive command: invalid "
                       "reactionTimeTransmissionPredefinedIntervals value: %1")
                        .arg(intervals[2])};
              parameters["reactionTimeTransmissionPredefinedIntervalsD"] =
                  intervals[3].toInt(&ok);
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed adaptive command: invalid "
                       "reactionTimeTransmissionPredefinedIntervals value: %1")
                        .arg(intervals[3])};
              parameters["reactionTimeTransmissionPredefinedIntervalsE"] =
                  intervals[4].toInt(&ok);
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed adaptive command: invalid "
                       "reactionTimeTransmissionPredefinedIntervals value: %1")
                        .arg(intervals[4])};
            } else {
              if (m_pimpl->m_strict_verification)
                throw XMLConfigHandlerException{
                    tr("Malformed adaptive command: invalid "
                       "reactionTimeTransmissionPredefinedIntervals directive: "
                       "%1")
                        .arg(attValue)};
            }
          } else {
            if (m_pimpl->m_strict_verification)
              throw XMLConfigHandlerException{
                  tr("Malformed adaptive command: invalid "
                     "reactionTimeTransmissionPredefinedIntervals directive: "
                     "%1")
                      .arg(attValue)};
          }
        } else {
          if (m_pimpl->m_strict_verification)
            throw XMLConfigHandlerException{
                tr("Malformed adaptive command: invalid directive: %1")
                    .arg(param)};
        }
      }
    } else if (attName == "start") {
      bool ok;
      startTime = attValue.toUInt(&ok);
      if (!ok)
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed adaptive command: invalid start time: %1")
                  .arg(attValue)};
      isStartTimeSet = true;
    } else {
      if (m_pimpl->m_strict_verification)
        throw XMLConfigHandlerException{
            tr("Malformed adaptive command: invalid attribute: %1")
                .arg(attName)};
    }
  }
  if (!isStartTimeSet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed adaptive command: missing start time")};
  }
  if (!isCommandSet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed adaptive command: missing command attribute")};
  }
  emit adaptiveElementRead(startTime, parameters);
}

void XMLConfigHandler::parseCodeElement(const QXmlStreamAttributes &atts) {

  if (!m_pimpl->m_inscriptelement)
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{tr(
          "Malformed XML: element 'code' should be within a script element")};
  bool isStartTimeSet{false};
  for (const auto &attribute : atts) {
    auto attName = attribute.name().toString();
    auto attValue = attribute.value().toString();
    if (attName == "start") {
      bool ok;
      m_pimpl->m_startTime = attValue.toUInt(&ok);
      if (!ok)
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed code command: invalid start time: %1")
                  .arg(attValue)};
      isStartTimeSet = true;
    }
  }
  if (!isStartTimeSet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed code command: missing start time")};
  }
}

void XMLConfigHandler::parseErrorElement(const QXmlStreamAttributes &atts) {
  if (!m_pimpl->m_inscriptelement)
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{tr(
          "Malformed XML: element 'error' should be within a script element")};
  QString errorType;
  unsigned int startTime{0};
  QJsonObject parameters;
  for (const auto &attribute : atts) {
    auto attName = attribute.name().toString();
    auto attValue = attribute.value().toString();
    if (attName == "typ" || attName == "type") {
      attName = "type";
      parameters[attName] = attValue;
    } else if (attName == "notify") {
      bool ok;
      int value{attValue.toInt(&ok)};
      parameters[attName] = value;
      if (value < 0 || !ok)
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed error command: invalid notify delay: %1")
                  .arg(attValue)};
    } else if (attName == "diagnostics") {
      parameters[attName] = attValue;
      QStringList validValues{"true",
                              "miss",
                              "ox_valve_leak",
                              "ox_valve_block",
                              "ox_stuck_open",
                              "ox_sensor_upper",
                              "ox_sensor_lower",
                              "ox_sensor",
                              "ni_valve_leak",
                              "ni_valve_block",
                              "ni_stuck_open",
                              "ni_sensor_upper",
                              "ni_sensor_lower",
                              "ni_sensor",
                              "mixer_block",
                              "dehum_sensor_lower",
                              "dehum_sensor_upper",
                              "cool_sensor_lower",
                              "heat_sensor_lower",
                              "cool_sensor_upper",
                              "heat_sensor_upper",
                              "scrub_sensor_lower",
                              "scrub_sensor_upper",
                              "vent_sensor_lower",
                              "vent_sensor_upper",
                              "dehum_ineff",
                              "scrub_ineff",
                              "vent_ineff",
                              "cool_ineff",
                              "heat_ineff",
                              "dehum_stuck_on",
                              "scrub_stuck_on",
                              "cool_stuck_on",
                              "heat_stuck_on",
                              "vent_stuck_on",
                              "dehum_sensor",
                              "scrub_sensor",
                              "cool_sensor",
                              "heat_sensor",
                              "vent_sensor"};
      if (!validValues.contains(attValue)) {
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed error command: invalid diagnostics type: %1")
                  .arg(attValue)};
      }
    } else if (attName == "fake") {
      bool ok;
      int value{attValue.toInt(&ok)};
      parameters[attName] = value;
      if (value <= 0 || value > 3 || !ok)
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed error command: invalid fake value: %1")
                  .arg(attValue)};
    } else if (attName == "start") {
      bool ok;
      parameters[attName] = attValue.toInt(&ok);
      if (!ok)
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed error command: invalid start time: %1")
                  .arg(attValue)};
    } else if (attName == "efficiency") {
      bool ok;
      double value{attValue.toDouble(&ok)};
      parameters[attName] = value;
      if (value < 0 || !ok)
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed error command: invalid efficiency: %1")
                  .arg(attValue)};
    } else if (attName == "clear") {
      bool ok;
      parameters[attName] = attValue.toInt(&ok);
      if (!ok)
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed error command: invalid clear delay: %1")
                  .arg(attValue)};
    } else if (attName == "survey") {
      QStringList qids{attValue.split(",")};
      for (const auto &qid : qids) {
        if (!qid.isEmpty() && !m_pimpl->m_qids.contains(qid)) {
          if (m_pimpl->m_strict_verification)
            throw XMLConfigHandlerException{
                tr("Malformed error command: invalid survey question id %1")
                    .arg(qid)};
        }
      }
      parameters[attName] = attValue;
    } else if (attName == "surveyOk") {
      QStringList qids{attValue.split(",")};
      for (const auto &qid : qids) {
        if (!qid.isEmpty() && !m_pimpl->m_qids.contains(qid)) {
          if (m_pimpl->m_strict_verification)
            throw XMLConfigHandlerException{
                tr("Malformed error command: invalid survey question id %1")
                    .arg(qid)};
        }
      }
      parameters[attName] = attValue;
    } else if (attName == "surveyDelay") {
      bool ok;
      int value{attValue.toInt(&ok)};
      parameters[attName] = value;
      if (value < 0 || !ok)
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed error command: invalid surveyDelay value: %1")
                  .arg(attValue)};
    } else if (attName == "allowLateRepair") {
      parameters[attName] = attValue;
      if (attValue != "true" && attValue != "false") {
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed error command: invalid allowLateRepair value: %1")
                  .arg(attValue)};
      }
    } else if (attName == "repeatNotify") {
      parameters[attName] = attValue;
      if (attValue != "true" && attValue != "false") {
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed error command: invalid repeatNotify value: %1")
                  .arg(attValue)};
      }
    } else if (attName == "loa") {
      parameters[attName] = attValue;
      if (!isValidLoaType(attValue)) {
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed error command: invalid loa value: %1")
                  .arg(attValue)};
      }
    } else if (attName == "loadelay") {
      bool ok;
      int value{attValue.toInt(&ok)};
      parameters[attName] = value;
      if (value < 0 || !ok)
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed error command: invalid loadelay value: %1")
                  .arg(attValue)};
    } else {
      if (m_pimpl->m_strict_verification)
        throw XMLConfigHandlerException{
            tr("Malformed error command: unknown attribute %1 set to %2")
                .arg(attName, attValue)};
    }
  }
  if (parameters.contains("type")) {
    errorType = parameters["type"].toString();
  } else {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed error command: missing error type")};
  }
  if (parameters.contains("start")) {
    startTime = static_cast<unsigned int>(
        parameters["start"].toInt()); // value is already validated
  } else {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed error command: missing start time")};
  }
  if (!isValidErrorType(errorType)) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed error command: invalid error type %1").arg(errorType)};
  }
  emit errorElementRead(startTime, errorType, parameters);
}

void XMLConfigHandler::parseFilterElement(const QXmlStreamAttributes &atts) {
  Q_UNUSED(atts)
  unsigned int startTime{0};
  bool filterEnabled{false};
  QString mtype;
  QString key;
  QString user;
  bool feedback{false};
  bool isStartTimeSet{false};
  bool isFilterEnabledSet{false};
  bool isTypeSet{false};
  bool isUserSet{false};
  bool isKeySet{false};
  if (!m_pimpl->m_inscriptelement)
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{tr(
          "Malformed XML: element 'filter' should be within a script element")};
  for (const auto &attribute : atts) {
    auto attName = attribute.name().toString();
    auto attValue = attribute.value().toString();
    if (attName == "start") {
      bool ok;
      startTime = attValue.toUInt(&ok);
      if (!ok)
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed error command: invalid start time: %1")
                  .arg(attValue)};
      isStartTimeSet = true;
    } else if (attName == "type") {
      if (attValue != "client_update" && attValue != "server_update" &&
          attValue != "client_trigger" && attValue != "server_notify") {
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed error command: invalid repeatNotify value: %1")
                  .arg(attValue)};
      }
      mtype = attValue;
      isTypeSet = true;
    } else if (attName == "enabled") {
      if (attValue != "true" && attValue != "false") {
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed error command: invalid enabled value: %1")
                  .arg(attValue)};
      }
      filterEnabled = (attValue == "true");
      isFilterEnabledSet = true;
    } else if (attName == "feedback") {
      if (attValue != "true" && attValue != "false") {
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed error command: invalid feedback value: %1")
                  .arg(attValue)};
      }
      feedback = (attValue == "true");
    } else if (attName == "key") {
      key = attValue.replace('\\', "\\\\").replace("\"", "\\");
      isKeySet = true;
    } else if (attName == "user") {
      user = attValue.replace('\\', "\\\\").replace("\"", "\\");
      ;
      isUserSet = true;
    } else {
      if (m_pimpl->m_strict_verification)
        throw XMLConfigHandlerException{
            tr("Malformed error command: unknown attribute %1 set to %2")
                .arg(attName, attValue)};
    }
  }
  if (!isStartTimeSet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed filter command: missing start time")};
  }
  if (!isFilterEnabledSet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed filter command: missing enabled parameter")};
  }
  if (!isTypeSet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed filter command: missing type parameter")};
  }
  if (!isUserSet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed filter command: missing user parameter")};
  }
  if (!isKeySet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed filter command: missing key parameter")};
  }
  emit filterElementRead(startTime, filterEnabled, mtype, key, user, feedback);
}

void XMLConfigHandler::parseMessageElement(const QXmlStreamAttributes &atts) {

  if (!m_pimpl->m_inscriptelement)
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{tr("Malformed XML: element 'message' "
                                         "should be within a script element")};
  bool isStartTimeSet{false};
  bool isTextSet{false};
  bool isSenderSet{false};
  bool isRecipientSet{false};
  unsigned int startTime{0};
  QString sender;
  QString recipient;
  QString text;
  for (const auto &attribute : atts) {
    auto attName = attribute.name().toString();
    auto attValue = attribute.value().toString();
    if (attName == "start") {
      bool ok;
      startTime = attValue.toUInt(&ok);
      if (!ok)
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed code command: invalid start time: %1")
                  .arg(attValue)};
      isStartTimeSet = true;
    } else if (attName == "from") {
      sender = attValue;
      isSenderSet = true;
    } else if (attName == "to") {
      recipient = attValue;
      isRecipientSet = true;
    } else if (attName == "text") {
      text = attValue;
      isTextSet = true;
    }
  }
  if (!isStartTimeSet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed message command: missing start time")};
  }
  if (!isTextSet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed message command: missing text")};
  }
  if (!isSenderSet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed message command: missing sender")};
  }
  if (!isRecipientSet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed message command: missing recipient")};
  }
  emit messageElementRead(startTime, sender, recipient, text);
}

void XMLConfigHandler::parseAccessElement(const QXmlStreamAttributes &atts) {

  if (!m_pimpl->m_inscriptelement)
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{tr(
          "Malformed XML: element 'access' should be within a script element")};
  bool isStartTimeSet{false};
  bool isUserSet{false};
  bool isAccessTypeSet{false};
  unsigned int startTime{0};
  QString username;
  int accessType{0};
  for (const auto &attribute : atts) {
    auto attName = attribute.name().toString();
    auto attValue = attribute.value().toString();
    if (attName == "start") {
      bool ok;
      startTime = attValue.toUInt(&ok);
      if (!ok)
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed code command: invalid start time: %1")
                  .arg(attValue)};
      isStartTimeSet = true;
    } else if (attName == "username") {
      username = attValue;
      isUserSet = true;
    } else if (attName == "type") {
      bool ok;
      accessType = attValue.toInt(&ok);
      if (!ok)
        if (m_pimpl->m_strict_verification)
          throw XMLConfigHandlerException{
              tr("Malformed code command: invalid access type: %1")
                  .arg(attValue)};
      if (accessType < 0 || accessType > 12) {
        throw XMLConfigHandlerException{
            tr("Malformed code command: invalid access type: %1")
                .arg(attValue)};
      }
      isAccessTypeSet = true;
    }
  }
  if (!isStartTimeSet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed message command: missing start time")};
  }
  if (!isUserSet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed message command: missing username")};
  }
  if (!isAccessTypeSet) {
    if (m_pimpl->m_strict_verification)
      throw XMLConfigHandlerException{
          tr("Malformed message command: missing access type")};
  }
  emit accessElementRead(startTime, username, accessType);
}

bool XMLConfigHandler::startElement(QStringView localName,
                                    const QXmlStreamAttributes &atts) {
  m_pimpl->m_strbuilder.clear();
  if (localName == QString{"script"}) {
    parseScriptElement(atts);
  } else if (localName == QString{"question"}) {
    parseQuestionElement(atts);
  } else if (localName == QString{"duration"}) {
    parseDurationElement(atts);
  } else if (localName == QString{"assistance"} ||
             localName == QString{"assitance"}) {
    parseAssistanceElement(atts);
  } else if (localName == QString{"interface"}) {
    parseInterfaceElement(atts);
  } else if (localName == QString{"control"}) {
    parseControlElement(atts);
  } else if (localName == QString{"adaptive"}) {
    parseAdaptiveElement(atts);
  } else if (localName == QString{"code"}) {
    parseCodeElement(atts);
  } else if (localName == QString{"error"}) {
    parseErrorElement(atts);
  } else if (localName == QString{"filter"}) {
    parseFilterElement(atts);
  } else if (localName == QString{"message"}) {
    parseMessageElement(atts);
  } else if (localName == QString{"access"}) {
    parseAccessElement(atts);
  } else if (localName == QString{"popup"}) {
    parsePopupElement(atts);
  }
  return true;
}

bool XMLConfigHandler::characters(QStringView ch) {
  m_pimpl->m_strbuilder += ch;
  return true;
}

bool XMLConfigHandler::endElement(QStringView localName) {

  if (localName == QString{"code"}) {
    emit codeElementRead(m_pimpl->m_startTime, m_pimpl->m_strbuilder);
  } else if (localName == QString{"script"}) {
    m_pimpl->m_inscriptelement = false;
  }
  m_pimpl->m_strbuilder.clear();
  return true;
}

static QString translateControlCommand(const QString &camsCmd) {
  static QMap<QString, QString> controlTranslations{
      {"sauerstoff.general.high",
       R"(registry.set("components.o2valve.strength", "high"))"},
      {"sauerstoff.general.medium",
       R"(registry.set("components.o2valve.strength", "medium"))"},
      {"sauerstoff.general.standard",
       R"(registry.set("components.o2valve.strength", "standard"))"},
      {"sauerstoff.general.auto on",
       R"(registry.set("control.o2.state", "auto"))"},
      {"sauerstoff.general.flow on",
       R"(registry.set("control.o2.state", "on"))"},
      {"sauerstoff.general.flow off",
       R"(registry.set("control.o2.state", "off"))"},
      {"kohlendioxid.general.high",
       R"(registry.set("components.scrubber.strength", "high"))"},
      {"kohlendioxid.general.medium",
       R"(registry.set("components.scrubber.strength", "medium"))"},
      {"kohlendioxid.general.standard",
       R"(registry.set("components.scrubber.strength", "standard"))"},
      {"kohlendioxid.general.auto on",
       R"(registry.set("control.scrubber.state", "auto"" },
        { "kohlendioxid.general.scrub on", R"(registry.set("control.scrubber.state", "on"" },
        { "kohlendioxid.general.scrub off", R"(registry.set("control.scrubber.state", "off"" },
        { "feuchtigkeit.general.high", R"(registry.set("components.dehumidifier.strength", "high"))"},
      {"feuchtigkeit.general.medium",
       R"(registry.set("components.dehumidifier.strength", "medium"))"},
      {"feuchtigkeit.general.standard",
       R"(registry.set("components.dehumidifier.strength", "standard"))"},
      {"feuchtigkeit.general.auto on",
       R"(registry.set("control.humidity.state", "auto"" },
        { "feuchtigkeit.general.dehumidify on", R"(registry.set("control.humidity.state", "on"" },
        { "feuchtigkeit.general.dehumidify off", R"(registry.set("control.humidity.state", "off"" },
        { "stickstoff.general.auto on", R"(registry.set("control.pressure.state", "auto"" },
        { "stickstoff.general.flow on", R"(registry.set("control.pressure.state", "on"" },
        { "stickstoff.general.flow off", R"(registry.set("control.pressure.state", "off"" },
        { "stickstoff.vent.high", R"(registry.set("components.vent.strength", "high"))"},
      {"stickstoff.vent.medium",
       R"(registry.set("components.vent.strength", "medium"))"},
      {"stickstoff.vent.standard",
       R"(registry.set("components.vent.strength", "standard"))"},
      {"stickstoff.flow.high",
       R"(registry.set("components.n2valve.strength", "high"))"},
      {"stickstoff.flow.medium",
       R"(registry.set("components.n2valve.strength", "medium"))"},
      {"stickstoff.flow.standard",
       R"(registry.set("components.n2valve.strength", "standard"))"},
      {"temperature.general.auto on",
       R"(registry.set("control.temperature.state", "auto"" },
        { "temperature.general.heat on", R"(registry.set("control.temperature.state", "heater"" },
        { "temperature.general.off", R"(registry.set("control.temperature.state", "off"" },
        { "temperature.general.cool on", R"(registry.set("control.temperature.state", "cooler"" },
        { "temperature.heater.high", R"(registry.set("components.heater.strength", "high"))"},
      {"temperature.heater.medium",
       R"(registry.set("components.heater.strength", "medium"))"},
      {"temperature.heater.standard",
       R"(registry.set("components.heater.strength", "standard"))"},
      {"temperature.cooler.high",
       R"(registry.set("components.cooler.strength", "high"))"},
      {"temperature.cooler.medium",
       R"(registry.set("components.cooler.strength", "medium"))"},
      {"temperature.cooler.standard",
       R"(registry.set("components.cooler.strength", "standard"))"}};

  if (controlTranslations.contains(camsCmd)) {
    return controlTranslations[camsCmd];
  }
  return "";
}

static bool isValidInterfaceElement(const QString &element) {
  static QStringList el{{"oxygenscope",
                         "pressurescope",
                         "carbonscope",
                         "tempscope",
                         "humidityscope",
                         "systemmonitor",
                         "oxtankdisplay",
                         "oxseconddisplay",
                         "nitankdisplay",
                         "niseconddisplay",
                         "mixerdisplay",
                         "cofilterlabel",
                         "coolingmachinelabel",
                         "heatingmachinelabel",
                         "dehumidlabel",
                         "ventlabel",
                         "oxygenpanel",
                         "carbonpanel",
                         "humiditypanel",
                         "pressurepanel",
                         "temppanel",
                         "masteralarm",
                         "assistanceselector",
                         "repairbutton",
                         "oxygenpanelgeneral",
                         "oxygenpanelflow",
                         "carbonpanelgeneral",
                         "carbonpanelscrubber",
                         "humiditypanelgeneral",
                         "humiditypanellevel",
                         "pressurepanelgeneral",
                         "pressurepanelflow",
                         "temppanelgeneral",
                         "temppanelheater",
                         "temppanelcooler",
                         "loa_1.button",
                         "loa_2.button",
                         "loa_3.button",
                         "loa_4.button",
                         "loa_5.button",
                         "loa_6.button",
                         "assistancepanel",
                         "chatbox",
                         "alternatemessaging",
                         "controldivider",
                         "strengthtitle",
                         "systemoverview",
                         "repairtask",
                         "transmissioncontrol",
                         "n2loggingtask",
                         "devicecontrol",
                         "assistancebox",
                         "survey",
                         "automationnotification",
                         "tempcontrolaligncenter",
                         "keepcontrolsvisible",
                         "transmissionchecksound",
                         "buttonsound",
                         "togglesound",
                         "alarmsound",
                         "loggingsound",
                         "messagesound",
                         "messaginginfront",
                         "fullscreen"}};
  return el.contains(element.toLower());
}

static bool isValidErrorType(const QString &type) {
  static QStringList el{
      {"ox_valve_leack",     "ox_valve_leak",      "ox_valve_block",
       "ox_stuck_open",      "ox_sensor_upper",    "ox_sensor_lower",
       "ox_sensor",          "ni_valve_leack",     "ni_valve_leak",
       "ni_valve_block",     "ni_stuck_open",      "ni_sensor_upper",
       "ni_sensor_lower",    "ni_sensor",          "mixer_block",
       "dehum_sensor_lower", "dehum_sensor_upper", "cool_sensor_lower",
       "heat_sensor_lower",  "cool_sensor_upper",  "heat_sensor_upper",
       "scrub_sensor_lower", "scrub_sensor_upper", "vent_sensor_lower",
       "vent_sensor_upper",  "dehum_ineff",        "scrub_ineff",
       "vent_ineff",         "cool_ineff",         "heat_ineff",
       "dehum_stuck_on",     "scrub_stuck_on",     "cool_stuck_on",
       "heat_stuck_on",      "vent_stuck_on",      "dehum_sensor",
       "scrub_sensor",       "cool_sensor",        "heat_sensor",
       "vent_sensor"}};
  return el.contains(type.toLower());
}

static bool isValidAdaptiveType(const QString &type) {
  static QStringList el{{"transmission", "logging", "combined",
                         "predefinedtransmission", "predefinedlogging",
                         "none"}};
  return el.contains(type.toLower());
}

static bool isValidLoaType(const QString &type) {
  static QStringList el{
      {"loa1", "loa2", "loa3", "loa4", "loa5", "loa6", "ask", "force", "user"}};
  return el.contains(type);
}
