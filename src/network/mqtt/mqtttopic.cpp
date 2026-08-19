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

#include "mqtttopic.h"
#include <QDebug>
#include <QStringList>

MqttTopic MqttTopic::toExperiment(const QString &serverUuid,
                                  const QString &experimentUuid,
                                  const QString &clientUuid) {
  return MqttTopic{serverUuid, experimentUuid, clientUuid, Type::TO_EXPERIMENT};
}

MqttTopic MqttTopic::toClient(const QString &serverUuid,
                              const QString &experimentUuid,
                              const QString &clientUuid) {
  return MqttTopic{serverUuid, experimentUuid, clientUuid, Type::TO_CLIENT};
}

MqttTopic MqttTopic::decode(const QString &topic) {
  MqttTopic t;
  auto splitted{topic.split('/')};
  if (splitted.size() == 5) {
    if (splitted[0] == "teamcams") {
      auto serverUuid{splitted[1]};
      auto experimentUuid{splitted[2]};
      auto clientUuid{splitted[4]};
      t.m_serverUuid = serverUuid;
      t.m_experimentUuid = experimentUuid;
      t.m_clientUuid = clientUuid;
      if (splitted[3] == "client") {
        t.m_type = Type::TO_CLIENT;
        t.m_valid = true;
      } else if (splitted[3] == "server") {
        t.m_type = Type::TO_EXPERIMENT;
        t.m_valid = true;
      }
    }
  }
  return t;
}

bool MqttTopic::isValid() const { return m_valid; }

MqttTopic::operator QString() const {
  if (!m_valid)
    return QString{};
  switch (m_type) {
  case Type::TO_CLIENT:
    return QString{"teamcams/%1/%2/client/%3"}.arg(
        m_serverUuid, m_experimentUuid, m_clientUuid);
  case Type::TO_EXPERIMENT:
    return QString{"teamcams/%1/%2/server/%3"}.arg(
        m_serverUuid, m_experimentUuid, m_clientUuid);
  }
  qDebug() << "Houston we have a problem";
  Q_UNREACHABLE_RETURN(QString{});
}

QString MqttTopic::baseServerPrefix(const QString &serverUuid) {
  return QString{"teamcams/%1/#"}.arg(serverUuid);
}

QString MqttTopic::baseExperimentPrefix(const QString &serverUuid,
                                        const QString &experimentUuid) {
  return QString{"teamcams/%1/%2/server/#"}.arg(serverUuid, experimentUuid);
}

QString MqttTopic::baseClientPrefix(const QString &serverUuid,
                                    const QString &experimentUuid) {
  return QString{"teamcams/%1/%2/client/#"}.arg(serverUuid, experimentUuid);
}

MqttTopic::MqttTopic(const QString &serverUuid, const QString &experimentUuid,
                     const QString &clientUuid, Type type)
    : m_serverUuid{serverUuid}, m_experimentUuid{experimentUuid},
      m_clientUuid{clientUuid}, m_type{type}, m_valid{true} {}

MqttTopic::MqttTopic() : m_valid{false} {}

const QString &MqttTopic::serverUuid() const { return m_serverUuid; }

const QString &MqttTopic::experimentUuid() const { return m_experimentUuid; }

const QString &MqttTopic::clientUuid() const { return m_clientUuid; }

MqttTopic::Type MqttTopic::type() const { return m_type; }
