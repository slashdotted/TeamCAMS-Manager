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
#include "operator.h"
#include <QJsonDocument>
#include <QJsonObject>

struct Operator::pimpl {
  UserCardPtr m_card;
  std::shared_ptr<Peer> m_peer;
  ~pimpl() {
    if (m_peer.get()) {
      m_peer->disconnect();
    }
  }
};

Operator::Operator(const UserCardPtr &card, std::shared_ptr<Peer> peer)
    : m_pimpl{std::make_unique<pimpl>()} {
  m_pimpl->m_card = card;
  m_pimpl->m_peer = peer;
  connect(m_pimpl->m_peer.get(), &Peer::messageReceived, this,
          &Operator::processMessage);
  connect(m_pimpl->m_peer.get(), &Peer::disconnected, this,
          &Operator::forwardDisconnection);
  connect(card.get(), &AccessCard::accessUpdated, this,
          &Operator::sendAccessUpdate);
  connect(card.get(), &AccessCard::aliasUpdated, this,
          &Operator::sendAliasUpdate);
}

Operator::~Operator() {
  qDebug() << "[Operator] Removing operator" << m_pimpl->m_card->username();
};

UserCardPtr Operator::card() const { return m_pimpl->m_card; }

std::shared_ptr<Peer> Operator::peer() const { return m_pimpl->m_peer; }

void Operator::sendUpdate(const QString &key, const QJsonValue &payload) const {
  QJsonObject data{{"type", "update"}, {"key", key}, {"payload", payload}};
  m_pimpl->m_peer->send(data);
}

void Operator::sendNotify(const QString &key, const QJsonValue &payload) const {
  QJsonObject data{{"type", "notify"}, {"key", key}, {"payload", payload}};
  m_pimpl->m_peer->send(data);
}

void Operator::sendControl(const QString &key,
                           const QJsonValue &payload) const {
  QJsonObject data{{"type", "control"}, {"key", key}, {"payload", payload}};
  m_pimpl->m_peer->send(data);
}

void Operator::broadcastUpdate(const QString &key,
                               const QJsonValue &payload) const {
  QJsonObject data{{"type", "update"}, {"key", key}, {"payload", payload}};
  m_pimpl->m_peer->broadcast(data);
}

void Operator::broadcastNotify(const QString &key,
                               const QJsonValue &payload) const {
  QJsonObject data{{"type", "notify"}, {"key", key}, {"payload", payload}};
  m_pimpl->m_peer->broadcastMessage(data);
}

void Operator::broadcastControl(const QString &key,
                                const QJsonValue &payload) const {
  QJsonObject data{{"type", "control"}, {"key", key}, {"payload", payload}};
  m_pimpl->m_peer->broadcastMessage(data);
}

void Operator::sendAccessUpdate(const AccessCard::AccessType &t) {
  QJsonObject data{
      {"type", "control"}, {"key", "access"}, {"payload", static_cast<int>(t)}};
  m_pimpl->m_peer->send(data);
}

void Operator::sendAliasUpdate(const QString &alias) {
  QJsonObject data{{"type", "control"}, {"key", "alias"}, {"payload", alias}};
  m_pimpl->m_peer->send(data);
  emit aliasChanged();
}

void Operator::processMessage(const QJsonObject &jsonObject) {
  if (!jsonObject.contains("type") || !jsonObject.contains("key") ||
      !jsonObject.contains("payload")) {
    qDebug() << "[Operator] Invalid message" << jsonObject;
    return;
  }
  QString type{jsonObject["type"].toString()};
  if (type == "update") {
      qDebug() << "[Operator] Got update" << jsonObject;
      emit updateReceived(jsonObject["key"].toString(), jsonObject["payload"]);
  } else if (type == "trigger") {
      qDebug() << "[Operator] Got trigger" << jsonObject;
      emit triggerReceived(jsonObject["key"].toString(), jsonObject["payload"]);
  } else if (type == "control") {
      qDebug() << "[Operator] Got control" << jsonObject;
      emit controlReceived(jsonObject["key"].toString(), jsonObject["payload"]);
  } else {
    return;
  }
}

void Operator::forwardDisconnection(const QString &reason) {
  emit disconnected(reason);
}
