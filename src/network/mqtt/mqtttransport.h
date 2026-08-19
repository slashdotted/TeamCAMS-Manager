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

#ifndef MqttTransport_H
#define MqttTransport_H

#include "network/messagetransport.h"
#include <QAbstractSocket>
#include <QByteArray>
#include <QObject>
#include <QString>
#include <QUrl>
#include <QUuid>

#if defined(Q_OS_WASM)
#include <QWebSocket>
#else
#include <QSslError>
#include <QSslSocket>
#include <QTcpSocket>
#endif

enum class MqttSecurityMode { Unsecure, Secure };

class MqttTransport : public MessageTransport {
  Q_OBJECT
public:
  explicit MqttTransport(const QString &managerUuid, QObject *parent = nullptr);
  ~MqttTransport() override;

  void connectToBroker(const QString &host, quint16 port, MqttSecurityMode mode,
                       bool requiresAuth = false, const QString &username = {},
                       const QString &password = {});
  void subscribe(const QString &topic);
  void unsubscribe(const QString &topic);
  void publish(const QString &topic, const QByteArray &message);

  bool isConnected() const;
  QAbstractSocket::SocketState state() const override;

  QString transportId() const override;

signals:
  void subscribed();
  void unsubscribed();

private slots:
  void onConnected();
  void onDisconnected();
  void reconnect() override;

private:
  void writeRawData(const QByteArray &data);
  void processBuffer();
  QByteArray encodeRemainingLength(int length);
  void sendPingReq();
  void checkKeepAlive();

  struct pimpl;
  std::unique_ptr<pimpl> m_pimpl;
};

#endif
