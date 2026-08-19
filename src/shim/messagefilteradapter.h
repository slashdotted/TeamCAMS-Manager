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
#ifndef MESSAGEFILTERADAPTER_H
#define MESSAGEFILTERADAPTER_H

#include "experiment/operators/messagefilter.h"
#include <QObject>

class MessageFilterAdapter : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(MessageFilterAdapter)
public:
  explicit MessageFilterAdapter(MessageFilter *parent = nullptr);

public slots:
  void blockIncomingUpdate(const QString &username, const QString &key,
                           bool feedback);
  void blockOutgoingUpdate(const QString &username, const QString &key);
  void blockOutgoingNotify(const QString &username, const QString &key);
  void blockIncomingTrigger(const QString &username, const QString &key);

  void unblockIncomingUpdate(const QString &username, const QString &key);
  void unblockOutgoingUpdate(const QString &username, const QString &key);
  void unblockOutgoingNotify(const QString &username, const QString &key);
  void unblockIncomingTrigger(const QString &username, const QString &key);
};

#endif // MESSAGEFILTERADAPTER_H
