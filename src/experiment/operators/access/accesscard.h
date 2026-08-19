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
#ifndef ACCESSCARD_H
#define ACCESSCARD_H

#include <QObject>
#include <memory>

/**
 * @brief The AccessCard class contains the credentials for an operator
 */
class AccessCard : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(AccessCard)
public:
  enum class AccessType {
    FULL_VIEW_AUTOSTART = 0,
    FULL_VIEW_AUTORESTART,
    FULL_VIEW,
    SYSTEM_OVERVIEW,
    COMMAND_PANEL,
    ASSISTANCE_SCREEN,
    ASSISTANCE_SELECTOR,
    ASSISTANCE_PANEL,
    FULL_COMMAND_PANEL,
    GRAPHS,
    REPAIR_PANEL,
    N2_LOGGING_PANEL,
    TRANSMISSION_PANEL,
    DISABLED
  };

  AccessCard(const QString &username, const QString &alias,
             const QString &password, AccessType r);
  ~AccessCard() override;

  // Getters
  const QString &username() const;
  const QString &alias() const;
  const QString &password() const;
  AccessType access() const;
  QString token(const QString &eid) const;

  // Setters
  void username(const QString &uname);
  void alias(const QString &alias);
  void password(const QString &pass);
  void access(AccessType r);

  // Serialization/Deserialization
  void load(const QJsonObject &json);
  void save(QJsonObject &json) const;

  static QString accessTypeName(int type);
signals:
  // Emitted when access type changes
  void accessUpdated(const AccessType &type);
  void aliasUpdated(const QString &alias);
  void usernameUpdated(const QString &name);
  void passwordUpdated(const QString &passwd);

private:
  struct pimpl;
  std::unique_ptr<pimpl> m_pimpl;
};

using UserCardPtr = std::shared_ptr<AccessCard>;

#endif // ACCESSCARD_H
