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
#ifndef OPERATORSMANAGER_H
#define OPERATORSMANAGER_H

#include "access/accesscard.h"
#include "messagefilter.h"
#include "network/peer.h"
#include "operator.h"
#include <QMap>
#include <QObject>
#include <memory>

class Experiment;

class OperatorsManager : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(OperatorsManager)
public:
  explicit OperatorsManager(Experiment *parent);
  ~OperatorsManager() override;

  void add(const UserCardPtr &u, std::shared_ptr<Peer> client);
  bool removeByCard(const UserCardPtr &card);
  std::shared_ptr<Peer> peerByCard(const UserCardPtr &card) const;
  bool containsCard(const UserCardPtr &card);
  QList<UserCardPtr> cards() const;
  MessageFilter &filter();

signals:
  // Client events
  void updateReceived(const QString &key, const QJsonValue &payload,
                      const Operator &cc) const;
  void triggerReceived(const QString &key, const QJsonValue &payload,
                       const Operator &cc) const;
  void controlReceived(const QString &key, const QJsonValue &payload,
                       const Operator &cc) const;
  void clientDisconnected(UserCardPtr connection) const;
  void clientConnected(UserCardPtr connection) const;
  void aliasChanged(UserCardPtr connection) const;

public slots:
  // Handle incoming signals from the outside world
  void disconnect(const UserCardPtr &user);
  void disconnectAll();
  void sendUpdate(const QString &key, const QJsonValue &payload,
                  const UserCardPtr &card) const;
  void sendNotify(const QString &key, const QJsonValue &payload,
                  const UserCardPtr &card) const;
  void sendControl(const QString &key, const QJsonValue &payload,
                   const UserCardPtr &card) const;
  void broadcastUpdate(const QString &key, const QJsonValue &payload) const;
  void broadcastNotify(const QString &key, const QJsonValue &payload) const;
  void broadcastControl(const QString &key, const QJsonValue &payload) const;
  // Serialization
  void load(const QJsonObject &json);
  void save(QJsonObject &json) const;
  void reset();

private slots:
  // Handle incoming signals from ClientConnections
  void handleClientDisconnection(const QString &reason) const;
  void handleUpdate(const QString &key, const QJsonValue &payload) const;
  void handleTrigger(const QString &key, const QJsonValue &payload) const;
  void handleControl(const QString &key, const QJsonValue &payload) const;
  void handleAliasChanged() const;

private:
  struct pimpl;
  std::unique_ptr<pimpl> m_pimpl;
};

#endif // OPERATORSMANAGER_H
