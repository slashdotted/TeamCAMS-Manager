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
#include "operatorsmanager.h"
#include "experiment/experiment.h"
#include "operator.h"

struct OperatorsManager::pimpl {
  Experiment *m_experiment;
  QMap<UserCardPtr, Operator *> m_connections;
  MessageFilter m_filter;
};

OperatorsManager::OperatorsManager(Experiment *parent)
    : QObject{parent}, m_pimpl{std::make_unique<OperatorsManager::pimpl>()} {
  m_pimpl->m_experiment = parent;
}

void OperatorsManager::add(const UserCardPtr &u, std::shared_ptr<Peer> client) {
  auto cc{new Operator{u, client}};
  m_pimpl->m_connections.insert(u, cc);
  connect(cc, SIGNAL(updateReceived(QString, QJsonValue)), this,
          SLOT(handleUpdate(QString, QJsonValue)));
  connect(cc, SIGNAL(triggerReceived(QString, QJsonValue)), this,
          SLOT(handleTrigger(QString, QJsonValue)));
  connect(cc, SIGNAL(controlReceived(QString, QJsonValue)), this,
          SLOT(handleControl(QString, QJsonValue)));
  connect(cc, SIGNAL(disconnected(const QString &)), this,
          SLOT(handleClientDisconnection(const QString &)));
  connect(cc, SIGNAL(aliasChanged()), this, SLOT(handleAliasChanged()));
  connect(
      &m_pimpl->m_filter, &MessageFilter::touchRequired, this,
      [this](const QString &property) {
        dynamic_cast<Experiment *>(parent())->simulator()->registry()->touch(
            property);
      });
  connect(&m_pimpl->m_filter, &MessageFilter::dumpRequired, this, [this]() {
    dynamic_cast<Experiment *>(parent())->simulator()->registry()->dump();
  });
  emit clientConnected(u);
}

bool OperatorsManager::removeByCard(const UserCardPtr &card) {
  return m_pimpl->m_connections.remove(card);
}

std::shared_ptr<Peer>
OperatorsManager::peerByCard(const UserCardPtr &card) const {
  return m_pimpl->m_connections.value(card)->peer();
}

bool OperatorsManager::containsCard(const UserCardPtr &card) {
  return m_pimpl->m_connections.contains(card);
}

QList<UserCardPtr> OperatorsManager::cards() const {
  return m_pimpl->m_connections.keys();
}

MessageFilter &OperatorsManager::filter() { return m_pimpl->m_filter; }

void OperatorsManager::handleControl(const QString &key,
                                     const QJsonValue &payload) const {
  if (const auto &cc{dynamic_cast<Operator *>(sender())}) {
    const auto card{cc->card()};
    if (card->access() == AccessCard::AccessType::DISABLED) {
      return;
    }
    if (!m_pimpl->m_filter.isBlockedIncoming(
            card->username(), MessageFilter::MessageType::CONTROL, key)) {
      emit controlReceived(key, payload, *cc);
    }
  }
}

void OperatorsManager::handleAliasChanged() const {
  if (const auto &cc{dynamic_cast<Operator *>(sender())}) {
    const auto card{cc->card()};
    emit aliasChanged(card);
  }
}

void OperatorsManager::disconnect(const UserCardPtr &user) {
  const auto &cc{m_pimpl->m_connections.value(user)};
  if (cc) {
    qDebug() << "[OperatorsManager] Disconnecting user" << user->username();
    sendNotify("disconnecting", {}, cc->card());
    cc->peer()->disconnect();
    delete cc;
  }
  m_pimpl->m_connections.remove(user);
  emit clientDisconnected(user);
}

void OperatorsManager::disconnectAll() {
  qDebug() << "[OperatorsManager] Disconnecting all users";
  for (auto u : std::as_const(m_pimpl->m_connections)) {
    sendNotify("disconnecting", {}, u->card());
    emit clientDisconnected(u->card());
    // u->socket()->disconnectClient("ConnectionManager::disconnectAll()");
    u->deleteLater();
  }
  m_pimpl->m_connections.clear();
}

void OperatorsManager::sendUpdate(const QString &key, const QJsonValue &payload,
                                  const UserCardPtr &card) const {
  if (!m_pimpl->m_experiment->enabled())
    return;
  const auto &cc{m_pimpl->m_connections.value(card)};
  if (!m_pimpl->m_filter.isBlockedOutgoing(
          card->username(), MessageFilter::MessageType::UPDATE, key)) {
    cc->sendUpdate(key, payload);
  }
}

void OperatorsManager::sendNotify(const QString &key, const QJsonValue &payload,
                                  const UserCardPtr &card) const {
  if (!m_pimpl->m_experiment->enabled())
    return;
  const auto &cc{m_pimpl->m_connections.value(card)};
  if (!m_pimpl->m_filter.isBlockedOutgoing(
          card->username(), MessageFilter::MessageType::NOTIFICATION, key)) {
    cc->sendNotify(key, payload);
  }
}

void OperatorsManager::sendControl(const QString &key,
                                   const QJsonValue &payload,
                                   const UserCardPtr &card) const {
  if (!m_pimpl->m_experiment->enabled())
    return;
  const auto &cc{m_pimpl->m_connections.value(card)};
  if (!m_pimpl->m_filter.isBlockedOutgoing(
          card->username(), MessageFilter::MessageType::CONTROL, key)) {
    cc->sendControl(key, payload);
  }
}

void OperatorsManager::broadcastUpdate(const QString &key,
                                       const QJsonValue &payload) const {
  if (!m_pimpl->m_experiment->enabled())
    return;
  for (const auto &cc : std::as_const(m_pimpl->m_connections)) {
    cc->broadcastUpdate(key, payload);
    break;
  }
}

void OperatorsManager::broadcastNotify(const QString &key,
                                       const QJsonValue &payload) const {
  if (!m_pimpl->m_experiment->enabled())
    return;
  for (const auto &cc : std::as_const(m_pimpl->m_connections)) {
    cc->broadcastNotify(key, payload);
    break;
  }
}

void OperatorsManager::broadcastControl(const QString &key,
                                        const QJsonValue &payload) const {
  if (!m_pimpl->m_experiment->enabled())
    return;
  for (const auto &cc : std::as_const(m_pimpl->m_connections)) {
    cc->broadcastControl(key, payload);
    return;
  }
}

void OperatorsManager::load(const QJsonObject &json) {
  Q_UNUSED(json)
  /* m_pimpl->m_filter.load(json["filters"].toObject()); */
}

void OperatorsManager::save(QJsonObject &json) const {
  Q_UNUSED(json)
  /*QJsonObject fobj;
  m_pimpl->m_filter.save(fobj);
  json["filters"] = fobj;*/
}

void OperatorsManager::reset() { m_pimpl->m_filter.clear(); }

void OperatorsManager::handleTrigger(const QString &key,
                                     const QJsonValue &payload) const {
  if (const auto &cc{dynamic_cast<Operator *>(sender())}) {
    const auto card{cc->card()};
    if (card->access() == AccessCard::AccessType::DISABLED) {
      return;
    }
    if (!m_pimpl->m_filter.isBlockedIncoming(
            card->username(), MessageFilter::MessageType::TRIGGER, key)) {
      emit triggerReceived(key, payload, *cc);
    }
  }
}

void OperatorsManager::handleUpdate(const QString &key,
                                    const QJsonValue &payload) const {
  if (const auto &cc{dynamic_cast<Operator *>(sender())}) {
    const auto card{cc->card()};
    if (cc->card()->access() == AccessCard::AccessType::DISABLED) {
      return;
    }
    if (!m_pimpl->m_filter.isBlockedIncoming(
            card->username(), MessageFilter::MessageType::UPDATE, key)) {
      emit updateReceived(key, payload, *cc);
    }
  }
}

OperatorsManager::~OperatorsManager() {
  for (const auto &cc : std::as_const(m_pimpl->m_connections)) {
    cc->disconnect();
    delete cc;
  }
  m_pimpl->m_connections.clear();
}

void OperatorsManager::handleClientDisconnection(const QString &reason) const {
  if (const auto &cc{dynamic_cast<Operator *>(sender())}) {
    const auto card{cc->card()};
    m_pimpl->m_connections.remove(card);
    emit clientDisconnected(card);
  }
}
