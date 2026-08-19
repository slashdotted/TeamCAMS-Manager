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
#ifndef MESSAGEFILTER_H
#define MESSAGEFILTER_H
#include <QJsonObject>
#include <QList>
#include <QMap>
#include <QObject>
#include <QString>
#include <memory>

class MessageFilter : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(MessageFilter)
public:
  enum class MessageType { UPDATE, NOTIFICATION, TRIGGER, CONTROL };
  enum class TrafficDirection { CLIENT2SERVER, SERVER2CLIENT };

  static QString MessageTypeName[];
  static QString TrafficDirectionName[];

  explicit MessageFilter(QObject *parent = nullptr);
  ~MessageFilter() override;

  int addRule(TrafficDirection d, MessageType t, const QString &key,
              const QString &user, bool feedback = false);
  void updateRule(int i, TrafficDirection d, MessageType t, const QString &key,
                  const QString &user, bool feedback);
  void getRule(int i, TrafficDirection &d, MessageType &t, QString &key,
               QString &user, bool &feedback) const;
  int indexOf(const TrafficDirection &d, const MessageType &t,
              const QString &key, const QString &user) const;
  int count() const;
  void clear();
  void remove(int i);
  void load(const QJsonObject &d);
  void save(QJsonObject &json) const;
  bool isBlockedIncoming(const QString &user, const MessageType &mt,
                         const QString &key) const;
  bool isBlockedOutgoing(const QString &user, const MessageType &mt,
                         const QString &key) const;

signals:
  void touchRequired(const QString &property) const;
  void dumpRequired() const;
  void cleared();
  void ruleAdded(int index);
  void ruleRemoved(int index);
  void ruleUpdated(int index);
  void log(const QString &tag, const QString &text);

private:
  struct pimpl;
  std::unique_ptr<pimpl> m_pimpl;
};

#endif // MESSAGEFILTER_H
