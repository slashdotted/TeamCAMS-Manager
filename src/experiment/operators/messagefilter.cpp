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
#include "messagefilter.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>

QString MessageFilter::MessageTypeName[] = {
    QT_TR_NOOP("Update"), QT_TR_NOOP("Notification"), QT_TR_NOOP("Trigger"),
    QT_TR_NOOP("Control")};

QString MessageFilter::TrafficDirectionName[] = {QT_TR_NOOP("To Server"),
                                                 QT_TR_NOOP("To Client")};

struct MessageFilter::pimpl {
  struct MessageRule {
    MessageRule(TrafficDirection dir, MessageType type, const QString &key,
                const QString &user, bool feedback)
        : m_dir{dir}, m_type{type}, m_key{key}, m_user{user},
          m_key_regex{QRegularExpression::fromWildcard(
              QRegularExpression::anchoredPattern(key), Qt::CaseSensitive)},
          m_user_regex{QRegularExpression::fromWildcard(
              QRegularExpression::anchoredPattern(user), Qt::CaseSensitive)},
          m_feedback{feedback} {
      // Special inverse filter handling (rules starting with ! negate the
      // filter)
      if (key.startsWith("!")) {
        m_key_regex = QRegularExpression::fromWildcard(
            QRegularExpression::anchoredPattern(key.right(key.size() - 1)),
            Qt::CaseSensitive);
        m_inverse_key = true;
      }
      if (user.startsWith("!")) {
        m_user_regex = QRegularExpression::fromWildcard(
            QRegularExpression::anchoredPattern(user.right(user.size() - 1)),
            Qt::CaseSensitive);
        m_inverse_user = true;
      }
    }
    TrafficDirection m_dir;
    MessageType m_type;
    QString m_key;
    QString m_user;
    QRegularExpression m_key_regex;
    QRegularExpression m_user_regex;
    bool m_feedback;
    bool m_inverse_key{false};
    bool m_inverse_user{false};
  };

  QList<MessageFilter::pimpl::MessageRule> m_rules;
};

MessageFilter::MessageFilter(QObject *parent)
    : QObject{parent}, m_pimpl{std::make_unique<pimpl>()} {}

MessageFilter::~MessageFilter() = default;

int MessageFilter::addRule(MessageFilter::TrafficDirection d,
                           MessageFilter::MessageType t, const QString &key,
                           const QString &user, bool feedback) {
  auto idx{indexOf(d, t, key, user)};
  if (idx >= 0) {
    return idx;
  }
  MessageFilter::pimpl::MessageRule r{d, t, key, user, feedback};
  m_pimpl->m_rules.append(r);
  emit ruleAdded(m_pimpl->m_rules.size() - 1);
  QJsonObject logObj;
  logObj["direction"] = TrafficDirectionName[static_cast<int>(d)];
  logObj["type"] = MessageTypeName[static_cast<int>(t)];
  logObj["key"] = key;
  logObj["user"] = user;
  logObj["feedback"] = feedback;
  emit log("filter:enable",
           QJsonDocument{logObj}.toJson(QJsonDocument::Compact));
  return m_pimpl->m_rules.size() - 1;
}

void MessageFilter::updateRule(int i, MessageFilter::TrafficDirection d,
                               MessageFilter::MessageType t, const QString &key,
                               const QString &user, bool feedback) {
  if (i >= 0 && i < count()) {
    MessageFilter::pimpl::MessageRule r{d, t, key, user, feedback};
    m_pimpl->m_rules.replace(i, r);
    emit ruleUpdated(i);
    QJsonObject logObj;
    logObj["direction"] = TrafficDirectionName[static_cast<int>(d)];
    logObj["type"] = MessageTypeName[static_cast<int>(t)];
    logObj["key"] = key;
    logObj["user"] = user;
    logObj["feedback"] = feedback;
    emit log("filter:update",
             QJsonDocument{logObj}.toJson(QJsonDocument::Compact));
    emit dumpRequired(); // Dump to update previously blocked clients
  }
}

void MessageFilter::getRule(int i, TrafficDirection &d, MessageType &t,
                            QString &key, QString &user, bool &feedback) const {
  if (i >= 0 && i < count()) {
    MessageFilter::pimpl::MessageRule m{m_pimpl->m_rules.at(i)};
    d = m.m_dir;
    t = m.m_type;
    key = m.m_key;
    user = m.m_user;
    feedback = m.m_feedback;
  }
}

int MessageFilter::indexOf(const TrafficDirection &d, const MessageType &t,
                           const QString &key, const QString &user) const {
  auto idx{0};
  for (const auto &r : m_pimpl->m_rules) {
    if (r.m_dir == d && r.m_type == t && r.m_user == user && r.m_key == key) {
      return idx;
    }
    ++idx;
  }
  return -1;
}

int MessageFilter::count() const { return m_pimpl->m_rules.count(); }

void MessageFilter::clear() {
  m_pimpl->m_rules.clear();
  emit cleared();
  emit dumpRequired(); // Dump to update previously blocked clients
}

void MessageFilter::remove(int i) {
  if (i >= 0 && i < count()) {
    QJsonObject logObj;
    auto &rule{m_pimpl->m_rules.at(i)};
    logObj["direction"] = TrafficDirectionName[static_cast<int>(rule.m_dir)];
    logObj["type"] = MessageTypeName[static_cast<int>(rule.m_type)];
    logObj["key"] = rule.m_key;
    logObj["user"] = rule.m_user;
    logObj["feedback"] = rule.m_feedback;
    emit log("filter:disable",
             QJsonDocument{logObj}.toJson(QJsonDocument::Compact));
    m_pimpl->m_rules.removeAt(i);
    emit ruleRemoved(i);
    emit dumpRequired(); // Dump to update previously blocked clients
  }
}

void MessageFilter::load(const QJsonObject &d) {
  if (d.contains("rules") && d["rules"].isArray()) {
    QJsonArray rules = d["rules"].toArray();
    for (auto r : rules) {
      QJsonObject o{r.toObject()};
      TrafficDirection td{o["direction"].toString() == "in"
                              ? TrafficDirection::CLIENT2SERVER
                              : TrafficDirection::SERVER2CLIENT};
      MessageType mt;
      QString mts{o["type"].toString()};
      if (mts == "notify") {
        mt = MessageType::NOTIFICATION;
      } else if (mts == "update") {
        mt = MessageType::UPDATE;
      } else if (mts == "trigger") {
        mt = MessageType::TRIGGER;
      } else if (mts == "control") {
        mt = MessageType::CONTROL;
      } else {
        mt = MessageType::UPDATE; // FIXME: Throw exception?
      }
      bool feedback{false};
      if (o.contains("feedback")) {
        feedback = o["feedback"].toBool();
      }
      QString key{o["key"].toString()};
      QString user{o["user"].toString()};
      MessageFilter::pimpl::MessageRule mr{td, mt, key, user, feedback};
      m_pimpl->m_rules.append(mr);
    }
  }
}

void MessageFilter::save(QJsonObject &json) const {
  QJsonArray o;
  for (const auto &mr : m_pimpl->m_rules) {
    QJsonObject r;
    r["direction"] = mr.m_dir == TrafficDirection::CLIENT2SERVER ? "in" : "out";
    switch (mr.m_type) {
    case MessageType::NOTIFICATION:
      r["type"] = "notify";
      break;
    case MessageType::UPDATE:
      r["type"] = "update";
      break;
    case MessageType::TRIGGER:
      r["type"] = "trigger";
      break;
    case MessageType::CONTROL:
      r["type"] = "control";
      break;
    }
    r["feedback"] = mr.m_feedback;
    r["key"] = mr.m_key;
    r["user"] = mr.m_user;
    o.append(r);
  }
  json["rules"] = o;
}

bool MessageFilter::isBlockedIncoming(const QString &user,
                                      const MessageType &mt,
                                      const QString &key) const {
  if (mt == MessageType::NOTIFICATION) {
    return true; // Invalid incoming message
  }
  for (const auto &r : m_pimpl->m_rules) {
    if (r.m_dir == TrafficDirection::CLIENT2SERVER) {
      if (mt != r.m_type) {
        continue;
      }
      auto matchesUser{r.m_user_regex.match(user).hasMatch()};
      auto matchesKey{r.m_key_regex.match(key).hasMatch()};
      if (matchesUser && r.m_inverse_user) {
        continue;
      } else if (matchesUser) {
        return false;
      }
      if (matchesKey && r.m_inverse_key) {
        continue;
      } else if (matchesKey) {
        return false;
      }
      if (mt == MessageType::UPDATE && r.m_feedback) {
        emit touchRequired(key);
      }
      return true;
    }
  }
  return false;
}

bool MessageFilter::isBlockedOutgoing(const QString &user,
                                      const MessageType &mt,
                                      const QString &key) const {
  for (const auto &r : m_pimpl->m_rules) {
    if (r.m_dir == TrafficDirection::SERVER2CLIENT) {
      if (mt != r.m_type) {
        continue;
      }
      auto matchesUser{r.m_user_regex.match(user).hasMatch()};
      auto matchesKey{r.m_key_regex.match(key).hasMatch()};
      if (matchesUser && r.m_inverse_user) {
        continue;
      }
      if (matchesKey && r.m_inverse_key) {
        continue;
      }
      return true;
    }
  }
  return false;
}
