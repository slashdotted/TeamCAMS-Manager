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
#include "accesscard.h"
#include "utils/persistentsettings.h"
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QJsonObject>
#include <QUuid>

struct AccessCard::pimpl {
  QString m_username{""};
  QString m_alias{""};
  QString m_password{""};
  AccessType m_access{AccessType::DISABLED};
};

AccessCard::AccessCard(const QString &username, const QString &alias,
                       const QString &password, AccessType r)
    : m_pimpl{std::make_unique<pimpl>()} {
  m_pimpl->m_username = username;
  m_pimpl->m_alias = alias;
  m_pimpl->m_password = password;
  m_pimpl->m_access = r;
}

QString AccessCard::accessTypeName(int type) {
  switch (type) {
  case 0:
    return tr("Full View (autostart)");
  case 1:
    return tr("Full View (autorestart)");
  case 2:
    return tr("Full View");
  case 3:
    return tr("System Overview");
  case 4:
    return tr("Command Panel");
  case 5:
    return tr("Assistance Screen");
  case 6:
    return tr("Assistance Selector");
  case 7:
    return tr("Assistance Panel");
  case 8:
    return tr("Full Command Panel");
  case 9:
    return tr("Graphs");
  case 10:
    return tr("Repair Panel");
  case 11:
    return tr("N2 Logging Panel");
  case 12:
    return tr("Transmission Panel");
  case 13:
    return tr("Disabled");
  }

  return {};
}

AccessCard::~AccessCard() = default;

const QString &AccessCard::username() const { return m_pimpl->m_username; }

const QString &AccessCard::alias() const { return m_pimpl->m_alias; }

const QString &AccessCard::password() const { return m_pimpl->m_password; }

AccessCard::AccessType AccessCard::access() const { return m_pimpl->m_access; }

QString AccessCard::token(const QString &eid) const {
  PersistentSettings settings;
  auto uuid = settings.value("instanceUuid").toString();
  QByteArray token =
      QCryptographicHash::hash((eid + username() + password() + uuid).toUtf8(),
                               QCryptographicHash::Sha256);
  return QLatin1String{token.toHex()};
}

void AccessCard::username(const QString &uname) {
  if (m_pimpl->m_username != uname) {
    m_pimpl->m_username = uname;
    emit usernameUpdated(uname);
  }
}

void AccessCard::alias(const QString &alias) {
  if (m_pimpl->m_alias != alias) {
    m_pimpl->m_alias = alias;
    emit aliasUpdated(alias);
  }
}

void AccessCard::password(const QString &pass) {
  if (m_pimpl->m_password != pass) {
    m_pimpl->m_password = pass;
    emit passwordUpdated(pass);
  }
}

void AccessCard::access(AccessCard::AccessType r) {
  if (m_pimpl->m_access != r) {
    m_pimpl->m_access = r;
    emit accessUpdated(r);
  }
}

void AccessCard::load(const QJsonObject &json) {
  m_pimpl->m_username = json["username"].toString();
  m_pimpl->m_password = json["password"].toString();
  m_pimpl->m_alias = json["alias"].toString();
  m_pimpl->m_access = static_cast<AccessType>(json["role"].toInt());
}

void AccessCard::save(QJsonObject &json) const {
  json["username"] = m_pimpl->m_username;
  json["alias"] = m_pimpl->m_alias;
  json["password"] = m_pimpl->m_password;
  json["role"] = static_cast<int>(m_pimpl->m_access);
}
