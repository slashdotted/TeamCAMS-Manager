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
#ifndef OPERATOR_H
#define OPERATOR_H

#include "access/accesscard.h"
#include "network/peer.h"
#include <QJsonValue>
#include <QList>
#include <QObject>
#include <QSslError>
#include <memory>

class Operator : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(Operator)
public:
  explicit Operator(const UserCardPtr &card, std::shared_ptr<Peer> peer);
  ~Operator() override;
  UserCardPtr card() const;
  std::shared_ptr<Peer> peer() const;

signals:
  void updateReceived(const QString &key, const QJsonValue &payload);
  void triggerReceived(const QString &key, const QJsonValue &payload);
  void controlReceived(const QString &key, const QJsonValue &payload);
  void disconnected(const QString &reason);
  void aliasChanged();

public slots:
  void sendUpdate(const QString &key, const QJsonValue &payload) const;
  void sendNotify(const QString &key, const QJsonValue &payload) const;
  void sendControl(const QString &key, const QJsonValue &payload) const;

  void broadcastUpdate(const QString &key, const QJsonValue &payload) const;
  void broadcastNotify(const QString &key, const QJsonValue &payload) const;
  void broadcastControl(const QString &key, const QJsonValue &payload) const;

private slots:
  void sendAccessUpdate(const AccessCard::AccessType &t);
  void sendAliasUpdate(const QString &alias);
  void processMessage(const QJsonObject &msg);
  void forwardDisconnection(const QString &reason);

private:
  struct pimpl;
  std::unique_ptr<pimpl> m_pimpl;
};

#endif // OPERATOR_H
