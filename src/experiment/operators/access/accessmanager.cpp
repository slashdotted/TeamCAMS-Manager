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
#include "accessmanager.h"
#include <QDebug>
#ifdef MANAGERGUI
#include <QPixmap>
#endif
#include "../../experiment.h"
#include <QJsonArray>
#include <QJsonObject>

struct AccessManager::pimpl {
  Experiment *m_experiment;
  QList<UserCardPtr> m_cards;
  QMap<QString, QString> m_session_tokens;
};

AccessManager::AccessManager(Experiment *ex)
    : m_pimpl{std::make_unique<pimpl>()} {
  m_pimpl->m_experiment = ex;
}

AccessManager::~AccessManager() = default;

void AccessManager::load(const QJsonObject &obj) {
  auto json = obj["credentials"].toArray();
  for (auto v : json) {
    auto u{std::make_shared<AccessCard>("", "", "",
                                        AccessCard::AccessType::DISABLED)};
    u->load(v.toObject());
    if (!containsUsername(u->username())) {
      m_pimpl->m_cards.push_back(u);
    }
  }
}

void AccessManager::save(QJsonObject &obj) const {
  QJsonArray json;
  for (const auto &u : m_pimpl->m_cards) {
    QJsonObject v;
    u->save(v);
    json.push_back(v);
  }
  obj["credentials"] = json;
}

int AccessManager::count() const { return m_pimpl->m_cards.count(); }

bool AccessManager::authenticate(const QString &username,
                                 const QString &password) const {
  const auto &c{std::find_if(
      m_pimpl->m_cards.begin(), m_pimpl->m_cards.end(),
      [&](const UserCardPtr &c) { return c->username() == username; })};
  if (c != m_pimpl->m_cards.end()) {
    if (!(*c)->password().trimmed().isEmpty() && (*c)->password() == password) {
      return true;
    }
  }
  // Try with token based auth
  auto token_user{authenticate(password)};
  return (token_user.isValid() && token_user.toString() == username);
}

QVariant AccessManager::authenticate(const QString &token) const {

  const auto &c{std::find_if(m_pimpl->m_cards.begin(), m_pimpl->m_cards.end(),
                             [&](const UserCardPtr &c) {
                               return c->token(m_pimpl->m_experiment->name()) ==
                                      token;
                             })};
  if (c != m_pimpl->m_cards.end()) {
    return (*c)->username();
  }
  return QVariant{};
}

QString AccessManager::getAuthenticationToken(const QString &username) const {
  const auto &c{std::find_if(
      m_pimpl->m_cards.begin(), m_pimpl->m_cards.end(),
      [&](const UserCardPtr &c) { return c->username() == username; })};
  if (c != m_pimpl->m_cards.end()) {
    return (*c)->token(m_pimpl->m_experiment->name());
  }
  return "";
}

int AccessManager::create() {
  int idx{count()};
  QString username{"user"};
  while (containsUsername(username + QString::number(idx))) {
    idx++;
  }
  QString alias{"alias"};
  while (containsAlias(alias + QString::number(idx))) {
    idx++;
  }
  int retval =
      add(username + QString::number(idx), alias + QString::number(idx),
          QUuid::createUuid().toString(QUuid::WithoutBraces), //"user",
          AccessCard::AccessType::FULL_VIEW);
  emit contentChanged();
  return retval;
}

int AccessManager::add(const QString &username, const QString &alias,
                       const QString &password, AccessCard::AccessType r) {
  int index{0};
  for (const auto &u : m_pimpl->m_cards) {
    if (u->username() == username) {
      return index;
    }
    ++index;
  }
  m_pimpl->m_cards.push_back(
      std::make_shared<AccessCard>(username, alias, password, r));
  emit contentChanged();
  return m_pimpl->m_cards.size() - 1;
}

int AccessManager::indexOf(const UserCardPtr &c) const {
  return m_pimpl->m_cards.indexOf(c);
}

void AccessManager::removeByIndex(int index) {
  m_pimpl->m_cards.removeAt(index);
  emit contentChanged();
}

void AccessManager::removeByUsername(const QString &username) {
  m_pimpl->m_cards.erase(std::remove_if(
      m_pimpl->m_cards.begin(), m_pimpl->m_cards.end(),
      [&](const UserCardPtr &c) { return c->username() == username; }));
  emit contentChanged();
}

void AccessManager::removeByAlias(const QString &alias) {
  m_pimpl->m_cards.erase(std::remove_if(
      m_pimpl->m_cards.begin(), m_pimpl->m_cards.end(),
      [&](const UserCardPtr &c) { return c->alias() == alias; }));
  emit contentChanged();
}

void AccessManager::removeByCard(UserCardPtr card) {
  m_pimpl->m_cards.erase(std::remove_if(
      m_pimpl->m_cards.begin(), m_pimpl->m_cards.end(),
      [&](const UserCardPtr &c) { return c.get() == card.get(); }));
  emit contentChanged();
}

UserCardPtr AccessManager::cardByIndex(int index) const {
  return m_pimpl->m_cards.at(index);
}

UserCardPtr AccessManager::cardByUsername(const QString &username) const {
  const auto &c{std::find_if(
      m_pimpl->m_cards.begin(), m_pimpl->m_cards.end(),
      [&](const UserCardPtr &c) { return c->username() == username; })};
  if (c != m_pimpl->m_cards.end()) {
    return *c;
  }
  return nullptr;
}

UserCardPtr AccessManager::cardByAlias(const QString &alias) const {
  const auto &c{
      std::find_if(m_pimpl->m_cards.begin(), m_pimpl->m_cards.end(),
                   [&](const UserCardPtr &c) { return c->alias() == alias; })};
  if (c != m_pimpl->m_cards.end()) {
    return *c;
  }
  return nullptr;
}

bool AccessManager::containsUsername(const QString &username) const {
  const auto &c{std::find_if(
      m_pimpl->m_cards.begin(), m_pimpl->m_cards.end(),
      [&](const UserCardPtr &c) { return c->username() == username; })};
  return c != m_pimpl->m_cards.end();
}

bool AccessManager::containsAlias(const QString &alias) const {
  const auto &c{
      std::find_if(m_pimpl->m_cards.begin(), m_pimpl->m_cards.end(),
                   [&](const UserCardPtr &c) { return c->alias() == alias; })};
  return c != m_pimpl->m_cards.end();
}
