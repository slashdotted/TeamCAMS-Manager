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

#ifndef MQTTPEER_H
#define MQTTPEER_H

#include "experiment/operators/access/accesscard.h"
#include "network/peer.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QTimer>

class MqttPeer : public Peer {
  Q_OBJECT
public:
  MqttPeer(const QString &clientUuid, UserCardPtr card,
           Status status = Status::Pending, QObject *parent = nullptr);
  void feed(const QJsonObject &message);
  void send(const QJsonObject &message) override;
  void broadcast(const QJsonObject &message) override;
  Status status() const override;
  void disconnect() override;
  UserCardPtr card() const override;

  void status(Status s);
  const QString &challenge() const;
  const QString &sessionId() const;
  const QByteArray &sessionKey() const;
  void sessionKey(const QByteArray &key);
  QString clientUuid() const;
  const QDateTime &lastActivity() const;
  uint64_t txSequence() const;
  uint64_t rxSequence() const;
  void txSequence(uint64_t v);
  void rxSequence(uint64_t v);
  ~MqttPeer() noexcept;

public slots:
  void touch();

signals:
  void sendMessage(MqttPeer *peer, const QJsonObject &message);

private:
  struct pimpl;
  std::unique_ptr<pimpl> m_pimpl;
};
#endif
