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
#include "connectionadapter.h"

struct ConnectionAdapter::pimpl {
  OperatorsManager *m_conm;
  AccessManager *m_credm;
};

ConnectionAdapter::ConnectionAdapter(OperatorsManager *conm,
                                     AccessManager *credm)
    : QObject{conm}, m_pimpl{std::make_unique<pimpl>()} {
  m_pimpl->m_conm = conm;
  m_pimpl->m_credm = credm;
}

void ConnectionAdapter::disconnect(const QString &username) {
  auto card{m_pimpl->m_credm->cardByUsername(username)};
  if (card) {
    m_pimpl->m_conm->disconnect(card);
  }
}

void ConnectionAdapter::disconnect() { m_pimpl->m_conm->disconnectAll(); }

void ConnectionAdapter::sendUpdate(const QString &key,
                                   const QJsonValue &payload,
                                   const QString &username) const {
  auto card{m_pimpl->m_credm->cardByUsername(username)};
  if (card) {
    m_pimpl->m_conm->sendUpdate(key, payload, card);
  }
}

void ConnectionAdapter::sendNotify(const QString &key,
                                   const QJsonValue &payload,
                                   const QString &username) const {
  auto card{m_pimpl->m_credm->cardByUsername(username)};
  if (card) {
    m_pimpl->m_conm->sendNotify(key, payload, card);
  }
}

void ConnectionAdapter::sendControl(const QString &key,
                                    const QJsonValue &payload,
                                    const QString &username) const {
  auto card{m_pimpl->m_credm->cardByUsername(username)};
  if (card) {
    m_pimpl->m_conm->sendControl(key, payload, card);
  }
}

void ConnectionAdapter::broadcastUpdate(const QString &key,
                                        const QJsonValue &payload) const {
  m_pimpl->m_conm->broadcastUpdate(key, payload);
}

void ConnectionAdapter::broadcastNotify(const QString &key,
                                        const QJsonValue &payload) const {
  m_pimpl->m_conm->broadcastNotify(key, payload);
}

void ConnectionAdapter::broadcastControl(const QString &key,
                                         const QJsonValue &payload) const {
  m_pimpl->m_conm->broadcastControl(key, payload);
}

QJsonArray ConnectionAdapter::connectUsersMatching(const QString &pattern) {
  QJsonArray result;
  QRegularExpression userpattern{QRegularExpression::fromWildcard(
      QRegularExpression::anchoredPattern(pattern), Qt::CaseSensitive)};
  for (const auto &u : m_pimpl->m_conm->cards()) {
    if (userpattern.match(u->username()).hasMatch()) {
      result.push_back(u->username());
    }
  }
  return result;
}

QJsonArray ConnectionAdapter::connectedUsers() const {
  QJsonArray users;
  for (const auto &c : m_pimpl->m_conm->cards()) {
    users.push_back(c->username());
  }
  return users;
}

QJsonArray ConnectionAdapter::connectedUserAliases() const {
  QJsonArray aliases;
  for (const auto &c : m_pimpl->m_conm->cards()) {
    if (!c->alias().trimmed().isEmpty()) {
      aliases.push_back(c->alias());
    }
  }
  return aliases;
}

QString ConnectionAdapter::aliasToUser(const QString &alias) const {
  return m_pimpl->m_credm->cardByAlias(alias)->username();
}

QString ConnectionAdapter::userToAlias(const QString &username) const {
  return m_pimpl->m_credm->cardByUsername(username)->alias();
}

bool ConnectionAdapter::disconnectUser(const QString &username) {
  if (!m_pimpl->m_credm->containsUsername(username)) {
    return false;
  }
  auto card{m_pimpl->m_credm->cardByUsername(username)};
  if (!m_pimpl->m_conm->containsCard(card)) {
    return false;
  }
  disconnect(username);
  return true;
}

void ConnectionAdapter::access(const QString &username,
                               unsigned int accessType) {
  if (!m_pimpl->m_credm->containsUsername(username)) {
    return;
  }
  auto card{m_pimpl->m_credm->cardByUsername(username)};
  if (!m_pimpl->m_conm->containsCard(card)) {
    return;
  }
  card->access(static_cast<AccessCard::AccessType>(accessType));
}

ConnectionAdapter::~ConnectionAdapter() = default;
