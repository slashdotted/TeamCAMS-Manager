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

#include "mqttexperimentclient.h"
#include "mqttcrypto.h"
#include "mqtttopic.h"
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>

struct MqttExperimentClient::pimpl {
  pimpl(MqttTransport *mqtt) : m_mqttTransport{mqtt} {}

  MqttTransport *m_mqttTransport;

  QString m_experimentUuid;
  QString m_username;
  QString m_password;

  QString m_clientUuid;
  QString m_challenge;
  QString m_sessionId;

  QByteArray m_sessionKey;

  uint64_t m_txSequence = 0;
  uint64_t m_rxSequence = 0;

  MqttExperimentClient::Status m_status =
      MqttExperimentClient::Status::Disconnected;

  QDateTime m_lastServerHeartbeat;
  QDateTime m_lastClientHeartbeat;
  QTimer m_heartbeatTimer;
};

MqttExperimentClient::~MqttExperimentClient() noexcept = default;

MqttExperimentClient::MqttExperimentClient(MqttTransport *mqtt, QObject *parent)
    : QObject{parent}, m_pimpl{std::make_unique<pimpl>(mqtt)} {
  m_pimpl->m_clientUuid = QUuid::createUuid().toString(QUuid::WithoutBraces);

  qDebug() << "[MqttExperimentClient] New instance for client uuid:"
           << m_pimpl->m_clientUuid;

  connect(m_pimpl->m_mqttTransport, &MqttTransport::connected, this,
          &MqttExperimentClient::onMqttTransportConnected);

  connect(m_pimpl->m_mqttTransport, &MqttTransport::disconnected, this,
          &MqttExperimentClient::onMqttTransportDisconnected);

  connect(m_pimpl->m_mqttTransport, &MqttTransport::messageReceived, this,
          &MqttExperimentClient::onMessageReceived);

  connect(m_pimpl->m_mqttTransport, &MqttTransport::stateChanged, this,
          &MqttExperimentClient::onMqttTransportStateChanged);

  m_pimpl->m_heartbeatTimer.setInterval(5000);

  connect(&m_pimpl->m_heartbeatTimer, &QTimer::timeout, this,
          &MqttExperimentClient::onHeartbeatTimer);
}

void MqttExperimentClient::onHeartbeatTimer() {
  if (status() == Status::Connected) {
    auto now = QDateTime::currentDateTime();
    if (m_pimpl->m_lastClientHeartbeat.secsTo(now) < 5) {
      qDebug() << "[MqttExperimentClient] Skip sending heartbeat to server "
                  "from client"
               << m_pimpl->m_clientUuid << ": less than 5 seconds";
    }
    qDebug() << "[MqttExperimentClient] Sending heartbeat to server from client"
             << m_pimpl->m_clientUuid;
    static QJsonObject heartbeat{{"type", "heartbeat"}};
    m_pimpl->m_lastClientHeartbeat = now;
    send(heartbeat);
  } else {
    qDebug() << "[MqttExperimentClient] Skipping sending heartbeat to server "
                "from client"
             << m_pimpl->m_clientUuid << "because we are not connected";
  }

  if (status() != Status::Disconnected) {
    const auto now = QDateTime::currentDateTime();
    if (m_pimpl->m_lastServerHeartbeat.isValid()) {
      const auto idleTime = m_pimpl->m_lastServerHeartbeat.secsTo(now);

      if (idleTime > 30) {
        qDebug() << "[MqttExperimentClient] Server has been idle for more than"
                 << idleTime << "s, disconnecting";
        onMqttTransportDisconnected(tr("Server did not respond"));
        emit disconnected(tr("Server did not respond"));
        return;
      }
    }
  }
}

void MqttExperimentClient::connectToExperiment(const QString &experimentUuid,
                                               const QString &username,
                                               const QString &password) {

  if ((m_pimpl->m_mqttTransport->state() == QAbstractSocket::ConnectedState) &&
      m_pimpl->m_status == Status::Disconnected) {

    qDebug() << "[MqttExperimentClient] Connecting to experiment:"
             << experimentUuid;

    m_pimpl->m_experimentUuid = experimentUuid;
    m_pimpl->m_username = username;
    m_pimpl->m_password = password;

    m_pimpl->m_sessionId.clear();
    m_pimpl->m_challenge.clear();
    m_pimpl->m_sessionKey.clear();

    m_pimpl->m_txSequence = 0;
    m_pimpl->m_rxSequence = 0;

    handleHeartbeat();

    status(Status::Connecting);

    auto topic = MqttTopic::toClient(m_pimpl->m_mqttTransport->transportId(),
                                     experimentUuid, m_pimpl->m_clientUuid);

    m_pimpl->m_mqttTransport->subscribe(topic);

    sendAnnounce();
  }
}

void MqttExperimentClient::sendAnnounce() {
  qDebug() << "[MqttExperimentClient] sendAnnounce";

  QJsonObject msg;
  msg["type"] = "hello";
  msg["username"] = m_pimpl->m_username;

  auto topic =
      MqttTopic::toExperiment(m_pimpl->m_mqttTransport->transportId(),
                              m_pimpl->m_experimentUuid, m_pimpl->m_clientUuid);

  QJsonDocument doc(msg);
  m_pimpl->m_mqttTransport->publish(topic, doc.toJson(QJsonDocument::Compact));

  status(Status::WaitingChallenge);
}

void MqttExperimentClient::onMqttTransportConnected() {
  m_pimpl->m_heartbeatTimer.start(0);
}

void MqttExperimentClient::disconnectFromExperiment() {
  onMqttTransportDisconnected(tr("Forced disconnection from experiment"));
}

MqttExperimentClient::Status MqttExperimentClient::status() const {
  return m_pimpl->m_status;
}

void MqttExperimentClient::send(const QJsonObject &message) {
  if (status() != Status::Connected) {
    return;
  }

  auto encrypted =
      MqttCrypto::encrypt(message, m_pimpl->m_sessionId, m_pimpl->m_sessionKey,
                          ++m_pimpl->m_txSequence);

  auto topic =
      MqttTopic::toExperiment(m_pimpl->m_mqttTransport->transportId(),
                              m_pimpl->m_experimentUuid, m_pimpl->m_clientUuid);

  QJsonDocument doc(encrypted);

  m_pimpl->m_mqttTransport->publish(topic, doc.toJson(QJsonDocument::Compact));
}

void MqttExperimentClient::onMqttTransportDisconnected(const QString &reason) {
  m_pimpl->m_heartbeatTimer.stop();
  status(Status::Disconnected);

  m_pimpl->m_challenge.clear();
  m_pimpl->m_sessionId.clear();
  m_pimpl->m_sessionKey.clear();

  m_pimpl->m_txSequence = 0;
  m_pimpl->m_rxSequence = 0;

  m_pimpl->m_lastServerHeartbeat = {};
}

void MqttExperimentClient::onMessageReceived(const QByteArray &message,
                                             const QString &topic) {
  qDebug() << "[MqttExperimentClient] onMessageReceived on topic:" << topic;
  auto mqttTopic = MqttTopic::decode(topic);

  if (!mqttTopic.isValid()) {
    qDebug() << "[MqttExperimentClient] onMessageReceived, invalid topic:"
             << topic;
    return;
  }
  if (mqttTopic.type() != MqttTopic::Type::TO_CLIENT) {
    qDebug()
        << "[MqttExperimentClient] onMessageReceived, message not for client";
    return;
  }
  if (mqttTopic.serverUuid() != m_pimpl->m_mqttTransport->transportId()) {
    qDebug() << "[MqttExperimentClient] onMessageReceived, message not for "
                "this transport uuid";
    return;
  }
  if (mqttTopic.experimentUuid() != m_pimpl->m_experimentUuid) {
    qDebug() << "[MqttExperimentClient] onMessageReceived, message not for "
                "this experiment";
    return;
  }

  QJsonDocument doc = QJsonDocument::fromJson(message);

  if (!doc.isObject()) {
    return;
  }

  auto obj = doc.object();
  auto type = obj.value("type");
  if (type.isString()) {
    const auto typeName = type.toString();
    if (typeName == "challenge") {
      if (status() == Status::WaitingChallenge) {
        handleChallenge(obj);
      }
      return;
    }
    if (typeName == "challenge_ack") {
      if (status() == Status::WaitingChallengeAck) {
        handleChallengeAck(obj);
      }
      return;
    }
  }

  if (status() != Status::Connected) {
    qDebug()
        << "[MqttExperimentClient] onMessageReceived not in connected status";
    return;
  }

  quint64 receivedSequence = 0;

  const auto plain =
      MqttCrypto::decrypt(obj, m_pimpl->m_sessionId, m_pimpl->m_sessionKey,
                          m_pimpl->m_rxSequence, &receivedSequence);

  if (plain.isEmpty()) {
    qDebug() << "[MqttPeer] Failed to decrypt message";
    return;
  }

  m_pimpl->m_rxSequence = receivedSequence;

  auto ptype = plain.value("type");
  if (ptype.isString() && ptype.toString() == "heartbeat") {
    handleHeartbeat();
    return;
  }
  qDebug() << "[MqttExperimentClient] onMessageReceived, forwarding message to "
              "upper layer"
           << plain;
  emit messageReceived(plain);
}

void MqttExperimentClient::onMqttTransportStateChanged(
    QAbstractSocket::SocketState s) {
  if (s == QAbstractSocket::UnconnectedState) {
    onMqttTransportDisconnected(tr("Socket disconnected"));
  }
}

void MqttExperimentClient::handleChallenge(const QJsonObject &msg) {
  qDebug() << "[MqttExperimentClient] handleChallenge";
  auto challengeValue = msg.value("challenge");

  if (!challengeValue.isString()) {
    return;
  }

  m_pimpl->m_challenge = challengeValue.toString();

  auto masterKey = QCryptographicHash::hash(
      (m_pimpl->m_mqttTransport->transportId() + m_pimpl->m_experimentUuid +
       m_pimpl->m_username + m_pimpl->m_password)
          .toUtf8(),
      QCryptographicHash::Sha256);

  QByteArray proofData;
  proofData.append(masterKey);
  proofData.append(m_pimpl->m_challenge.toUtf8());

  auto proof = QCryptographicHash::hash(proofData, QCryptographicHash::Sha256);

  QJsonObject response;
  response["type"] = "challenge_response";
  response["proof"] = QString::fromUtf8(proof.toHex());

  auto topic =
      MqttTopic::toExperiment(m_pimpl->m_mqttTransport->transportId(),
                              m_pimpl->m_experimentUuid, m_pimpl->m_clientUuid);

  QJsonDocument doc(response);

  m_pimpl->m_mqttTransport->publish(topic, doc.toJson(QJsonDocument::Compact));

  status(Status::WaitingChallengeAck);
}

void MqttExperimentClient::handleChallengeAck(const QJsonObject &msg) {
  auto sessionIdValue = msg.value("sessionid");
  qDebug() << "[MqttExperimentClient] handleChallengeAck, received session id:"
           << sessionIdValue;

  if (!sessionIdValue.isString()) {
    qDebug() << "[MqttExperimentClient] handleChallengeAck, invalid session id";
    return;
  }

  m_pimpl->m_sessionId = sessionIdValue.toString();

  auto masterKey = QCryptographicHash::hash(
      (m_pimpl->m_mqttTransport->transportId() + m_pimpl->m_experimentUuid +
       m_pimpl->m_username + m_pimpl->m_password)
          .toUtf8(),
      QCryptographicHash::Sha256);

  QByteArray sessionKeyMaterial;

  sessionKeyMaterial.append(masterKey);
  sessionKeyMaterial.append(m_pimpl->m_challenge.toUtf8());
  sessionKeyMaterial.append(m_pimpl->m_sessionId.toUtf8());

  m_pimpl->m_sessionKey =
      QCryptographicHash::hash(sessionKeyMaterial, QCryptographicHash::Sha256);

  m_pimpl->m_txSequence = 0;
  m_pimpl->m_rxSequence = 0;

  m_pimpl->m_lastServerHeartbeat = QDateTime::currentDateTime();

  status(Status::Connected);
  m_pimpl->m_challenge.clear();
}

void MqttExperimentClient::handleHeartbeat() {
  qDebug() << "[MqttExperimentClient] handleHeartbeat (from server)";
  m_pimpl->m_lastServerHeartbeat = QDateTime::currentDateTime();
  if (m_pimpl->m_lastClientHeartbeat.isValid() &&
      m_pimpl->m_lastClientHeartbeat.secsTo(m_pimpl->m_lastServerHeartbeat) >
          5) {
    onHeartbeatTimer();
  }
}

void MqttExperimentClient::status(Status s) {
  if (m_pimpl->m_status == s) {
    return;
  }

  auto oldStatus = m_pimpl->m_status;

  m_pimpl->m_status = s;

  emit statusChanged(s);

  if (s == Status::Connected) {
    emit connected();
  }

  if (oldStatus == Status::Connected && s == Status::Disconnected) {
    emit disconnected(tr("Forced disconnected status"));
  }
}
