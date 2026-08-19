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
#ifndef MQTTTOPIC_H
#define MQTTTOPIC_H

#include <QString>

class MqttTopic {
public:
  // Prefix listening
  // Base prefix is teamcams/<serveruuid>/#
  // Server message is
  // teamcams/<serveruuid>/<experimentuuid>/client/<clientuuid> Client message
  // is teamcams/<serveruuid>/<experimentuuid>/server/<clientuuid>

  enum class Type { TO_CLIENT, TO_EXPERIMENT };

  static MqttTopic toExperiment(const QString &serverUuid,
                                const QString &experimentUuid,
                                const QString &clientUuid);

  static MqttTopic toClient(const QString &serverUuid,
                            const QString &experimentUuid,
                            const QString &clientUuid);

  static MqttTopic decode(const QString &topic);

  bool isValid() const;

  operator QString() const;

  static QString baseServerPrefix(const QString &serverUuid);

  static QString baseExperimentPrefix(const QString &serverUuid,
                                      const QString &experimentUuid);

  static QString baseClientPrefix(const QString &serverUuid,
                                  const QString &experimentUuid);

  const QString &serverUuid() const;
  const QString &experimentUuid() const;
  const QString &clientUuid() const;
  Type type() const;

private:
  MqttTopic(const QString &serverUuid, const QString &experimentUuid,
            const QString &clientUuid, Type type);

  MqttTopic();

  QString m_serverUuid;
  QString m_experimentUuid;
  QString m_clientUuid;
  Type m_type;
  bool m_valid;
};

#endif // MQTTTOPIC_H
