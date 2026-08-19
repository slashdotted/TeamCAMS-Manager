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

#ifndef MQTTEXPERIMENTSERVER_H
#define MQTTEXPERIMENTSERVER_H

#include "experiment/operators/access/accesscard.h"
#include "network/experimentserver.h"
#include "network/mqtt/mqttpeer.h"
#include "network/mqtt/mqtttransport.h"
#include <QAbstractSocket>
#include <QObject>
#include <memory>

class Experiment;

class MqttExperimentServer : public ExperimentServer {
  Q_OBJECT
public:
  explicit MqttExperimentServer(std::shared_ptr<MqttTransport> mqtt,
                                Experiment *experiment,
                                QObject *parent = nullptr);
  ~MqttExperimentServer();
  QAbstractSocket::SocketState state() const;

private slots:
  void onMqttTransportConnected();
  void onMqttTransportDisconnected(const QString &reason);
  void onMessageReceived(const QByteArray &message, const QString &topic);
  void onPeerSendMessageRequest(MqttPeer *peer, const QJsonObject &message);
  void onMqttTransportStateChanged(QAbstractSocket::SocketState s);
  void sendHeartbeat();
  void broadcastMessage(const QJsonObject &message);

private:
  std::shared_ptr<MqttPeer> createPeer(const QString &clientUuid,
                                       UserCardPtr card);
  std::shared_ptr<MqttPeer> findPeer(const QString &clientUuid);
  struct pimpl;
  std::unique_ptr<pimpl> m_pimpl;
  void handleAnnounceMessage(const QString &clientUuid,
                             const QJsonObject &message);
  void sendChallenge(std::shared_ptr<MqttPeer> peer);
  bool validateChallengeResponse(std::shared_ptr<MqttPeer> peer,
                                 const QString &proof) const;
  void handleChallengeResponseMessage(std::shared_ptr<MqttPeer> peer,
                                      const QJsonObject &message);
  void sendChallengeAck(std::shared_ptr<MqttPeer>);

  void sendMessage(MqttPeer *peer, const QJsonObject &message);
  void sendEncryptedMessage(MqttPeer *peer, const QJsonObject &message);
};

#endif // MQTTEXPERIMENTSERVER_H
