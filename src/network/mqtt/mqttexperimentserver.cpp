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

#include "mqttexperimentserver.h"
#include "experiment/experiment.h"
#include "mqtttopic.h"
#include "network/mqtt/mqttpeer.h"
#include <QTimer>
#include <memory.h>

struct MqttExperimentServer::pimpl {
  pimpl(std::shared_ptr<MqttTransport> mqtt, Experiment *experiment)
      : m_mqttTransport{mqtt}, m_experiment{experiment} {}
  std::shared_ptr<MqttTransport> m_mqttTransport;
  Experiment *m_experiment{nullptr};
  QHash<QString, std::shared_ptr<MqttPeer>> m_activePeers;
  QTimer m_heartbeatTimer;
};

MqttExperimentServer::MqttExperimentServer(std::shared_ptr<MqttTransport> mqtt,
                                           Experiment *experiment,
                                           QObject *parent)
    : ExperimentServer{parent},
      m_pimpl{std::make_unique<pimpl>(mqtt, experiment)} {

  connect(m_pimpl->m_mqttTransport.get(), &MqttTransport::stateChanged, this,
          &MqttExperimentServer::onMqttTransportStateChanged);
  connect(m_pimpl->m_mqttTransport.get(), &MqttTransport::messageReceived, this,
          &MqttExperimentServer::onMessageReceived);
  connect(m_pimpl->m_mqttTransport.get(), &MqttTransport::connected, this,
          &MqttExperimentServer::onMqttTransportConnected);
  connect(m_pimpl->m_mqttTransport.get(), &MqttTransport::disconnected, this,
          &MqttExperimentServer::onMqttTransportDisconnected);
  m_pimpl->m_heartbeatTimer.setInterval(5000);
  connect(&m_pimpl->m_heartbeatTimer, &QTimer::timeout, this,
          &MqttExperimentServer::sendHeartbeat);
  if (m_pimpl->m_mqttTransport->isConnected()) {
      onMqttTransportConnected();
  }
}

MqttExperimentServer::~MqttExperimentServer() = default;

QAbstractSocket::SocketState MqttExperimentServer::state() const {
  return m_pimpl->m_mqttTransport->state();
}

void MqttExperimentServer::onMqttTransportConnected() {
  auto topic = MqttTopic::baseExperimentPrefix(
      m_pimpl->m_mqttTransport->transportId(), m_pimpl->m_experiment->name());
  qDebug() << "[MqttExperimentServer] Subscribing to experiment topic:"
           << topic;
  m_pimpl->m_mqttTransport->subscribe(topic);
  m_pimpl->m_heartbeatTimer.start();
}

void MqttExperimentServer::onMqttTransportDisconnected(const QString &reason) {
  auto peers = m_pimpl->m_activePeers.values();
  m_pimpl->m_heartbeatTimer.stop();

  for (const auto &peer : std::as_const(peers)) {
    peer->disconnect();
  }

  m_pimpl->m_activePeers.clear();
  emit stateChanged(state());
}

void MqttExperimentServer::onMessageReceived(const QByteArray &message,
                                             const QString &topic) {
  auto mqttTopic{MqttTopic::decode(topic)};
  if (!m_pimpl->m_experiment) {
    qDebug() << "[MqttExperimentServer] onMessageReceived without an "
                "experiment, topic "
             << topic;
    return;
  }
  if (!mqttTopic.isValid()) {
    qDebug() << "[MqttExperimentServer] onMessageReceived invalid topic "
             << topic;
    return;
  }
  if (mqttTopic.type() != MqttTopic::Type::TO_EXPERIMENT) {
    qDebug() << "[MqttExperimentServer] onMessageReceived got a message not "
                "targeted to an experiment:"
             << topic;
    return;
  }
  if (mqttTopic.experimentUuid() != m_pimpl->m_experiment->name()) {
    qDebug() << "[MqttExperimentServer] onMessageReceived got a message not "
                "for this experiment:"
             << mqttTopic.experimentUuid() << "vs"
             << m_pimpl->m_experiment->name();
    return;
  }

  if (!m_pimpl->m_experiment->enabled()) {
    qDebug() << "[MqttExperimentServer] onMessageReceived got a message but "
                "experiment"
             << mqttTopic.experimentUuid() << "is disabled";
    return;
  }
  QJsonDocument doc = QJsonDocument::fromJson(message);
  if (!doc.isObject()) {
    qDebug()
        << "[MqttExperimentServer] onMessageReceived got non-object message "
           "on topic:"
        << topic;
    return;
  }
  auto messageObject = doc.object();
  auto clientUuid = mqttTopic.clientUuid();
  auto peer = findPeer(clientUuid);
  auto type = messageObject.value("type");
  if (!peer) {
    // If there is no peer we can only accept announce messages
    if (!type.isString() || type.toString() != "hello") {
      qDebug() << "[MqttExperimentServer] onMessageReceived expected hello "
                  "announcement on topic "
               << topic << "got" << type;
      return;
    }
    handleAnnounceMessage(clientUuid, messageObject);
    return;
  } else if (type.isString() && type.toString() == "challenge_response") {
    if (peer->status() == MqttPeer::Status::Pending) {
      handleChallengeResponseMessage(peer, messageObject);
    }
    return;
  }

  if (peer->status() == MqttPeer::Status::Ready) {
    qDebug() << "[MqttExperimentServer] Feeding message to"
             << peer->clientUuid();
    peer->feed(messageObject);
  }
}

void MqttExperimentServer::onPeerSendMessageRequest(
    MqttPeer *peer, const QJsonObject &message) {
  if (!peer) {
    qDebug() << "[MqttExperimentServer] onPeerSendMessageRequest on null peer";
    return;
  }
  if (peer->status() == MqttPeer::Status::Ready) {
    qDebug() << "[MqttExperimentServer] sending message on behalf of peer"
             << peer->clientUuid();
    sendMessage(peer, message);
  }
}

void MqttExperimentServer::onMqttTransportStateChanged(
    QAbstractSocket::SocketState s) {
  emit stateChanged(s);
}

void MqttExperimentServer::sendHeartbeat() {
  if (m_pimpl->m_activePeers.isEmpty())
    return;
  qDebug() << "[MqttExperimentServer] sendHeartbeat";
  QJsonObject hb;
  hb["type"] = "heartbeat";
  broadcastMessage(hb);

  auto now = QDateTime::currentDateTime();
  QList<std::shared_ptr<MqttPeer>> scheduledForDisconnection;
  for (const auto &[clientUuid, peer] :
       m_pimpl->m_activePeers.asKeyValueRange()) {
    const auto idleTime = peer->lastActivity().secsTo(now);
    if (idleTime > 30) {
      qDebug() << "[MqttExperimentServer] Client" << clientUuid << "idle for"
               << idleTime << "s, disconnecting";
      scheduledForDisconnection.push_back(peer);
    } else {
      qDebug() << "[MqttExperimentServer] Client" << clientUuid
               << "still alive, last rx" << idleTime << "s";
    }
  }
  for (const auto &peer : scheduledForDisconnection) {
    peer->disconnect();
  }
}

void MqttExperimentServer::broadcastMessage(const QJsonObject &message) {
  for (const auto &peer : m_pimpl->m_activePeers) {
    if (peer->status() == MqttPeer::Status::Ready) {
      qDebug() << "[MqttExperimentServer] send" << message << "to"
               << peer->clientUuid();
      peer->send(message);
    } else {
      qDebug() << "[MqttExperimentServer] cannot send" << message << "to"
               << peer->clientUuid() << "peer not ready";
    }
  }
}

std::shared_ptr<MqttPeer>
MqttExperimentServer::createPeer(const QString &clientUuid, UserCardPtr card) {
  auto peer{std::shared_ptr<MqttPeer>{
      new MqttPeer{clientUuid, card, MqttPeer::Status::Pending, this},
      [](MqttPeer *obj) {
        if (obj) {
          qDebug() << "[MqttPeer] deleteLater of peer" << obj->clientUuid();
          obj->deleteLater();
        }
      }}};
  qDebug() << "[MqttPeer] Creating peer" << clientUuid;
  std::weak_ptr<Peer> weakPeer = peer;
  connect(peer.get(), &MqttPeer::disconnected, this,
          [this, weakPeer, clientUuid]() {
            auto peer = weakPeer.lock();
            if (!peer) {
              return;
            }
            emit peerDisconnected(peer);
            qDebug() << "[MqttPeer] Peer" << clientUuid
                     << "disconnected, removing from active peers";
            m_pimpl->m_activePeers.remove(clientUuid);
          });
  connect(peer.get(), &MqttPeer::broadcastMessage, this,
          [this](const QJsonObject &message) { broadcastMessage(message); });
  connect(peer.get(), &MqttPeer::sendMessage, this,
          &MqttExperimentServer::onPeerSendMessageRequest);
  qDebug() << "[MqttPeer] Adding peer" << clientUuid << "to active peers";
  m_pimpl->m_activePeers.insert(clientUuid, peer);
  return peer;
}

std::shared_ptr<MqttPeer>
MqttExperimentServer::findPeer(const QString &clientUuid) {
  return m_pimpl->m_activePeers.value(clientUuid, nullptr);
}

void MqttExperimentServer::handleAnnounceMessage(const QString &clientUuid,
                                                 const QJsonObject &message) {
  qDebug() << "[MqttExperimentServer] handleAnnounceMessage from" << clientUuid;
  auto usernameValue = message.value("username");
  if (!usernameValue.isString()) {
    qDebug() << "[MqttExperimentServer] handleAnnounceMessage error: missing "
                "username";
    return;
  }
  auto username = usernameValue.toString();
  auto card =
      m_pimpl->m_experiment->credentialManager()->cardByUsername(username);
  if (!card) {
    qDebug() << "[MqttExperimentServer] handleAnnounceMessage error: missing "
                "card for username"
             << username;
    return;
  }
  if (m_pimpl->m_experiment->operatorsManager()->containsCard(card)) {
    qDebug() << "[MqttExperimentServer] Operator already connected" << username;
    return;
  }
  auto peer{createPeer(clientUuid,
                       card)}; // the challenge is an uuid generate by mqttpeer
  sendChallenge(peer);
}

void MqttExperimentServer::sendChallenge(std::shared_ptr<MqttPeer> peer) {
  qDebug() << "[MqttExperimentServer] Sending challenge to peer"
           << peer->clientUuid();
  QJsonObject c;
  c["type"] = "challenge";
  c["challenge"] = peer->challenge();
  sendMessage(peer.get(), c);
}

void MqttExperimentServer::handleChallengeResponseMessage(
    std::shared_ptr<MqttPeer> peer, const QJsonObject &message) {
  qDebug() << "[MqttExperimentServer] handleChallengeResponseMessage for peer"
           << peer->clientUuid();
  auto proof = message.value("proof");
  if (proof.isString()) {
    if (validateChallengeResponse(peer, proof.toString())) {
      peer->status(MqttPeer::Status::Ready);
      peer->touch();
      emit peerConnected(peer);
      qDebug() << "[MqttExperimentServer] peerConnected" << peer->clientUuid();
      sendChallengeAck(peer);
    }
  }
}

bool MqttExperimentServer::validateChallengeResponse(
    std::shared_ptr<MqttPeer> peer, const QString &proof) const {
  qDebug() << "[MqttExperimentServer] validateChallengeResponse for peer"
           << peer->clientUuid();
  auto masterKey = QCryptographicHash::hash(
      (m_pimpl->m_mqttTransport->transportId() + m_pimpl->m_experiment->name() +
       peer->card()->username() + peer->card()->password())
          .toUtf8(),
      QCryptographicHash::Sha256);
  QByteArray data;
  data.append(masterKey);
  data.append(peer->challenge().toUtf8());
  auto expected = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
  if (expected.toHex() == proof.toUtf8()) {
    QByteArray sessionKey;
    sessionKey.append(masterKey);
    sessionKey.append(peer->challenge().toUtf8());
    sessionKey.append(peer->sessionId().toUtf8());
    peer->sessionKey(
        QCryptographicHash::hash(sessionKey, QCryptographicHash::Sha256));
    qDebug() << "[MqttExperimentServer] validateChallengeResponse for peer"
             << peer->clientUuid() << "successful";
    return true;
  }
  qDebug() << "[MqttExperimentServer] validateChallengeResponse for peer"
           << peer->clientUuid() << "failed";
  return false;
}

void MqttExperimentServer::sendChallengeAck(std::shared_ptr<MqttPeer> peer) {
  qDebug() << "[MqttExperimentServer] sendChallengeAck for peer"
           << peer->clientUuid();
  QJsonObject c;
  c["type"] = "challenge_ack";
  c["sessionid"] = peer->sessionId();
  sendMessage(peer.get(), c);
}

void MqttExperimentServer::sendMessage(MqttPeer *peer,
                                       const QJsonObject &message) {
  auto clientTopic =
      MqttTopic::toClient(m_pimpl->m_mqttTransport->transportId(),
                          m_pimpl->m_experiment->name(), peer->clientUuid());
  QJsonDocument doc{message};
  qDebug() << "[MqttExperimentServer] sendMessage to" << clientTopic;
  m_pimpl->m_mqttTransport->publish(clientTopic,
                                    doc.toJson(QJsonDocument::Compact));
}

void MqttExperimentServer::sendEncryptedMessage(MqttPeer *peer,
                                                const QJsonObject &message) {
  // Peer only sends encrypted messages
  if (!peer) {
    qDebug() << "[MqttExperimentServer] sendEncryptedMessage on null peer";
    return;
  }
  peer->send(message);
}
