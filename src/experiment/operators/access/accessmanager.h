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
#ifndef ACCESSMANAGER_H
#define ACCESSMANAGER_H

#include "accesscard.h"
#include <QAbstractListModel>
#include <QJsonArray>
#include <QObject>
#include <memory>

class Experiment;

/**
 * @brief The AccessManager class manages user cards
 */
class AccessManager : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(AccessManager)
public:
  explicit AccessManager(Experiment *ex);
  ~AccessManager() override;

  // Serialization/Deserialization
  void load(const QJsonObject &obj);
  void save(QJsonObject &obj) const;

  // Public methods
  int count() const;
  bool authenticate(const QString &username, const QString &password) const;
  QVariant authenticate(const QString &token) const;
  QString getAuthenticationToken(const QString &username) const;
  int indexOf(const UserCardPtr &c) const;
  UserCardPtr cardByIndex(int index) const;
  UserCardPtr cardByUsername(const QString &username) const;
  UserCardPtr cardByAlias(const QString &alias) const;
  bool containsUsername(const QString &username) const;
  bool containsAlias(const QString &alias) const;

public slots:
  int create();
  int add(const QString &username, const QString &alias,
          const QString &password, AccessCard::AccessType r);
  void removeByIndex(int index);
  void removeByUsername(const QString &username);
  void removeByAlias(const QString &alias);
  void removeByCard(UserCardPtr card);

signals:
  void contentChanged();

private:
  struct pimpl;
  std::unique_ptr<pimpl> m_pimpl;
};

#endif // ACCESSMANAGER_H
