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

#include "mqttpeer.h"
#include "mqttcrypto.h"
#include <qcryptographichash.h>

struct MqttPeer::pimpl {
  pimpl(const QString &clientUuid, UserCardPtr card, Status status)
      : m_clientUuid{clientUuid}, m_card{card}, m_status{status},
        m_challenge{QUuid::createUuid().toString(QUuid::WithoutBraces)},
        m_sessionId{QUuid::createUuid().toString(QUuid::WithoutBraces)} {}

  QString m_clientUuid;
  UserCardPtr m_card;
  Status m_status;
  QString m_challenge;
  QString m_sessionId;
  QByteArray m_sessionKey;
  uint64_t m_txSequence{0};
  uint64_t m_rxSequence{0};
  QDateTime m_lastHeartbeat;
};

MqttPeer::MqttPeer(const QString &clientUuid, UserCardPtr card, Status status,
                   QObject *parent)
    : m_pimpl{std::make_unique<pimpl>(clientUuid, card, status)} {
  m_pimpl->m_lastHeartbeat = QDateTime::currentDateTime();
}

void MqttPeer::feed(const QJsonObject &envelope) {
  qDebug() << "[MqttPeer] Feeding message" << envelope;

  if (m_pimpl->m_status != Status::Ready) {
    qDebug() << "[MqttPeer] Peer not ready to decrypt message";
    return;
  }

  quint64 receivedSequence = 0;

  const auto plain =
      MqttCrypto::decrypt(envelope, m_pimpl->m_sessionId, m_pimpl->m_sessionKey,
                          m_pimpl->m_rxSequence, &receivedSequence);

  if (plain.isEmpty()) {
    qDebug() << "[MqttPeer] Failed to decrypt message";
    return;
  }

  m_pimpl->m_rxSequence = receivedSequence;

  touch();

  auto type = plain.value("type");

  if (type.isString() && type.toString() == "heartbeat") {

    qDebug() << "[MqttPeer] Got heartbeat for client" << m_pimpl->m_clientUuid;

    return;
  }
  qDebug() << "[MqttPeer] Valid message received" << plain;
  emit messageReceived(plain);
}

void MqttPeer::broadcast(const QJsonObject &message) {
  emit broadcastMessage(message);
}

void MqttPeer::send(const QJsonObject &message) {
  if (m_pimpl->m_status != Status::Ready) {
    qDebug() << "[MqttPeer] Cannot send message from peer, status not ready";
    return;
  }
  auto encrypted =
      MqttCrypto::encrypt(message, m_pimpl->m_sessionId, m_pimpl->m_sessionKey,
                          ++m_pimpl->m_txSequence);
  qDebug() << "[MqttPeer] Requesting send message from peer"
           << m_pimpl->m_clientUuid;
  emit sendMessage(this, encrypted);
}

void MqttPeer::status(Status s) { m_pimpl->m_status = s; }

UserCardPtr MqttPeer::card() const { return m_pimpl->m_card; }

const QString &MqttPeer::challenge() const { return m_pimpl->m_challenge; }

const QString &MqttPeer::sessionId() const { return m_pimpl->m_sessionId; }

const QByteArray &MqttPeer::sessionKey() const { return m_pimpl->m_sessionKey; }

void MqttPeer::sessionKey(const QByteArray &key) {
  m_pimpl->m_sessionKey = key;
}

MqttPeer::Status MqttPeer::status() const { return m_pimpl->m_status; }

void MqttPeer::disconnect() {
  if (m_pimpl->m_status != Status::Disconnecting) {
    m_pimpl->m_status = Status::Disconnecting;
    emit disconnected(tr("Forced peer disconnection"));
  }
}

QString MqttPeer::clientUuid() const { return m_pimpl->m_clientUuid; }

const QDateTime &MqttPeer::lastActivity() const {
  return m_pimpl->m_lastHeartbeat;
}

uint64_t MqttPeer::txSequence() const { return m_pimpl->m_txSequence; }

uint64_t MqttPeer::rxSequence() const { return m_pimpl->m_rxSequence; }

void MqttPeer::txSequence(uint64_t v) { m_pimpl->m_txSequence = v; }

void MqttPeer::rxSequence(uint64_t v) { m_pimpl->m_rxSequence = v; }

MqttPeer::~MqttPeer() noexcept {
  qDebug() << "[MqttPeer] Destroying peer" << m_pimpl->m_clientUuid;
}

void MqttPeer::touch() {
  if (m_pimpl->m_status == Status::Ready) {
    qDebug() << "[MqttPeer] Got something to keep us alive"
             << m_pimpl->m_clientUuid;
    m_pimpl->m_lastHeartbeat = QDateTime::currentDateTime();
  }
}
