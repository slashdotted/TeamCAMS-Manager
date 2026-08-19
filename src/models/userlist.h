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
#ifndef USERLIST_H
#define USERLIST_H

#include "experiment/operators/access/accessmanager.h"
#include "experiment/operators/operatorsmanager.h"
#include <QAbstractListModel>
#include <memory>

/**
 * @brief The UserListModel class
 */
class UserList : public QAbstractListModel {
  Q_OBJECT
  Q_DISABLE_COPY(UserList)
public:
  explicit UserList(AccessManager *credentials, OperatorsManager *connections,
                    QObject *parent = nullptr);
  virtual ~UserList();
  // QAbstractListModel implementation
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role);
  Qt::ItemFlags flags(const QModelIndex &index) const;

public slots:
  void createUser();
  void addUser(const QString &username, const QString &alias,
               const QString &password, AccessCard::AccessType r);
  void removeUser(int index);

private:
  struct pimpl;
  std::unique_ptr<pimpl> m_pimpl;
};

#endif // USERLISTMODEL_H
