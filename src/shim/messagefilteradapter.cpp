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
#include "messagefilteradapter.h"

MessageFilterAdapter::MessageFilterAdapter(MessageFilter *parent)
    : QObject{parent} {}

void MessageFilterAdapter::blockIncomingUpdate(const QString &username,
                                               const QString &key,
                                               bool feedback) {
  auto mf{dynamic_cast<MessageFilter *>(parent())};
  mf->addRule(MessageFilter::TrafficDirection::CLIENT2SERVER,
              MessageFilter::MessageType::UPDATE, key, username, feedback);
}

void MessageFilterAdapter::blockOutgoingUpdate(const QString &username,
                                               const QString &key) {
  auto mf{dynamic_cast<MessageFilter *>(parent())};
  mf->addRule(MessageFilter::TrafficDirection::SERVER2CLIENT,
              MessageFilter::MessageType::UPDATE, key, username);
}

void MessageFilterAdapter::blockOutgoingNotify(const QString &username,
                                               const QString &key) {
  auto mf{dynamic_cast<MessageFilter *>(parent())};
  mf->addRule(MessageFilter::TrafficDirection::SERVER2CLIENT,
              MessageFilter::MessageType::NOTIFICATION, key, username);
}

void MessageFilterAdapter::blockIncomingTrigger(const QString &username,
                                                const QString &key) {
  auto mf{dynamic_cast<MessageFilter *>(parent())};
  mf->addRule(MessageFilter::TrafficDirection::CLIENT2SERVER,
              MessageFilter::MessageType::TRIGGER, key, username);
}

void MessageFilterAdapter::unblockIncomingUpdate(const QString &username,
                                                 const QString &key) {
  auto mf{dynamic_cast<MessageFilter *>(parent())};
  mf->remove(mf->indexOf(MessageFilter::TrafficDirection::CLIENT2SERVER,
                         MessageFilter::MessageType::UPDATE, key, username));
}

void MessageFilterAdapter::unblockOutgoingUpdate(const QString &username,
                                                 const QString &key) {
  auto mf{dynamic_cast<MessageFilter *>(parent())};
  mf->remove(mf->indexOf(MessageFilter::TrafficDirection::SERVER2CLIENT,
                         MessageFilter::MessageType::UPDATE, key, username));
}

void MessageFilterAdapter::unblockOutgoingNotify(const QString &username,
                                                 const QString &key) {
  auto mf{dynamic_cast<MessageFilter *>(parent())};
  mf->remove(mf->indexOf(MessageFilter::TrafficDirection::SERVER2CLIENT,
                         MessageFilter::MessageType::NOTIFICATION, key,
                         username));
}

void MessageFilterAdapter::unblockIncomingTrigger(const QString &username,
                                                  const QString &key) {
  auto mf{dynamic_cast<MessageFilter *>(parent())};
  mf->remove(mf->indexOf(MessageFilter::TrafficDirection::CLIENT2SERVER,
                         MessageFilter::MessageType::TRIGGER, key, username));
}
