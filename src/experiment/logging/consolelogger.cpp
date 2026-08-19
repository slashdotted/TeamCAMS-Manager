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

#include "consolelogger.h"
#include "experiment/experiment.h"
#include "simulation/registry.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

struct ConsoleLogger::pimpl {
  pimpl(const Experiment *experiment, const Registry *registry)
      : m_exp{experiment}, m_registry{registry} {}
  bool m_json_format{true};
  const Experiment *m_exp;
  const Registry *m_registry;
  QString m_uuid;
};

ConsoleLogger::ConsoleLogger(const Experiment *exp, const Registry *registry,
                             QObject *parent)
    : QObject{parent}, m_pimpl{std::make_unique<pimpl>(exp, registry)} {
  if (!qgetenv("CONSOLE_LOGGER_PLAIN").isEmpty()) {
    m_pimpl->m_json_format = false;
  }
}

void ConsoleLogger::endSession() {}

void ConsoleLogger::startSession(const QString &name) {
  m_pimpl->m_uuid =
      name + "_" +
      QDateTime::currentDateTime().toString(Qt::ISODate).replace(":", "Q") +
      "_" + QUuid::createUuid().toString().mid(1, 36);
}

static QTextStream &qStdOut() {
  static QTextStream tstream{stdout};
  return tstream;
}

void ConsoleLogger::log(double timestamp, const QString &tag,
                        const QString &sender, const QString &text) {
  static long runningNumber{0};
  if (m_pimpl->m_json_format) {
    QJsonObject data;
    data["idx"] = int(runningNumber++);
    data["eid"] = m_pimpl->m_exp->name();
    data["uuid"] = m_pimpl->m_uuid;
    data["ms"] = timestamp;
    data["s"] = timestamp / 1000.0;
    data["tag"] = tag;
    data["sender"] = sender;
    QJsonParseError err;
    QJsonDocument payload = QJsonDocument::fromJson(text.toUtf8(), &err);
    if (err.error == QJsonParseError::NoError) {
      if (payload.isObject()) {
        data["payload"] = payload.object();
      } else if (payload.isArray()) {
        data["payload"] = payload.array();
      }

    } else {
      data["text"] = text;
    }
    data["ms_abs"] = QDateTime::currentMSecsSinceEpoch();
    data["components.cabin.o2.relvalue"] =
        m_pimpl->m_registry->get("components.cabin.o2.relvalue", true)
            .toDouble();
    data["components.cabin.pressure"] =
        m_pimpl->m_registry->get("components.cabin.pressure", true).toDouble();
    data["components.cabin.temperature"] =
        m_pimpl->m_registry->get("components.cabin.temperature", true)
            .toDouble();
    data["components.cabin.co2.relvalue"] =
        m_pimpl->m_registry->get("components.cabin.co2.relvalue", true)
            .toDouble();
    data["components.cabin.humidity"] =
        m_pimpl->m_registry->get("components.cabin.humidity", true).toDouble();
    data["components.o2tank.volume"] =
        m_pimpl->m_registry->get("components.o2tank.volume", true).toDouble();
    data["components.n2tank.volume"] =
        m_pimpl->m_registry->get("components.n2tank.volume", true).toDouble();
    data["system.error"] =
        m_pimpl->m_registry->get("system.error", true).toString();
    QJsonDocument doc{data};
    qStdOut() << doc.toJson(QJsonDocument::Compact) << '\n';
    qStdOut().flush();
  } else {
    QString escapedText{text};
    escapedText.replace('"', "\\\"");
    QString cleanTag{'"' + tag + '"'};
    QString cleanText{'"' + escapedText + '"'};
    qStdOut()
        << timestamp / 1000.0 << ";" << QDateTime::currentMSecsSinceEpoch()
        << ";"
        << m_pimpl->m_registry->get("components.cabin.o2.relvalue", true)
               .toDouble()
        << ";"
        << m_pimpl->m_registry->get("components.cabin.pressure", true)
               .toDouble()
        << ";"
        << m_pimpl->m_registry->get("components.cabin.temperature", true)
               .toDouble()
        << ";"
        << m_pimpl->m_registry->get("components.cabin.co2.relvalue", true)
               .toDouble()
        << ";"
        << m_pimpl->m_registry->get("components.cabin.humidity", true)
               .toDouble()
        << ";"
        << m_pimpl->m_registry->get("components.o2tank.volume", true).toDouble()
        << ";"
        << m_pimpl->m_registry->get("components.n2tank.volume", true).toDouble()
        << ";" << cleanTag << ";" << cleanText << ";"
        << m_pimpl->m_registry->get("system.error", true).toString() << ";"
        << runningNumber++ << ";" << sender << '\n';
    qStdOut().flush();
  }
}
