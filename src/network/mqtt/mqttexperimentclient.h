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

#ifndef MQTTEXPERIMENTCLIENT_H
#define MQTTEXPERIMENTCLIENT_H

#include "network/mqtt/mqtttransport.h"
#include <QObject>

class MqttExperimentClient : public QObject {
  Q_OBJECT
public:
  enum class Status {
    Disconnected,
    Connecting,
    WaitingChallenge,
    WaitingChallengeAck,
    Connected
  };

  explicit MqttExperimentClient(MqttTransport *mqtt, QObject *parent = nullptr);
  ~MqttExperimentClient() noexcept;

  void connectToExperiment(const QString &experimentUuid,
                           const QString &username, const QString &password);
  void disconnectFromExperiment();
  Status status() const;
  void send(const QJsonObject &message);

signals:
  void connected();
  void disconnected(const QString &reason);
  void statusChanged(Status);
  void messageReceived(const QJsonObject &message);

private slots:
  void onMqttTransportConnected();
  void onMqttTransportDisconnected(const QString &reason);
  void onMessageReceived(const QByteArray &message, const QString &topic);
  void onMqttTransportStateChanged(QAbstractSocket::SocketState s);

  void handleChallenge(const QJsonObject &msg);
  void handleChallengeAck(const QJsonObject &msg);
  void handleHeartbeat();

  void onHeartbeatTimer();

private:
  QByteArray buildNonce(quint64 seq);
  void status(Status s);
  void sendAnnounce();

  struct pimpl;
  std::unique_ptr<pimpl> m_pimpl;
};

#endif // MQTTEXPERIMENTCLIENT_H
