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
#include "userlist.h"
#ifdef MANAGERGUI
#include <QPixmap>
#endif

struct UserList::pimpl {
  pimpl(AccessManager *credentials, OperatorsManager *connections)
      : m_credentials{credentials}, m_connections{connections} {}
  AccessManager *m_credentials;
  OperatorsManager *m_connections;
};

UserList::UserList(AccessManager *credentials, OperatorsManager *connections,
                   QObject *parent)
    : QAbstractListModel{parent},
      m_pimpl{std::make_unique<pimpl>(credentials, connections)} {
  // Pass this to fix disconnection
  connect(m_pimpl->m_connections, &OperatorsManager::clientConnected, this,
          [this](UserCardPtr) {
            emit dataChanged(index(0, 0), index(rowCount(), 4));
          });
  connect(m_pimpl->m_connections, &OperatorsManager::clientDisconnected, this,
          [this](UserCardPtr) {
            emit dataChanged(index(0, 0), index(rowCount(), 4));
          });
}

UserList::~UserList() = default;

QVariant UserList::data(const QModelIndex &index, int role) const {
  switch (role) {
  case Qt::EditRole:
  case Qt::DisplayRole:
    switch (index.column()) {
    case 0:
      return m_pimpl->m_credentials->cardByIndex(index.row())->username();
    case 1:
      return m_pimpl->m_credentials->cardByIndex(index.row())->alias();
    case 2:
      try {
        return AccessCard::accessTypeName(static_cast<int>(
            m_pimpl->m_credentials->cardByIndex(index.row())->access()));
      } catch (...) {
        return tr("Invalid access value");
      }
    case 3:
      if (role == Qt::EditRole)
        return m_pimpl->m_credentials->cardByIndex(index.row())->password();
      else
        return "***";
    default:
      return QVariant();
    }
  case Qt::TextAlignmentRole:
    return int(Qt::AlignCenter | Qt::AlignBottom);
  case Qt::DecorationRole:
#ifdef MANAGERGUI
    if (index.column() == 4) {
      auto card{m_pimpl->m_credentials->cardByIndex(index.row())};
      if (m_pimpl->m_connections->containsCard(card)) {
        auto c = m_pimpl->m_connections->peerByCard(card);
        return QPixmap(":/images/user-available-web.png").scaledToHeight(16);
      } else {
        return QPixmap(":/images/user-offline.png").scaledToHeight(16);
      }
    } else {
      return QVariant();
    }
#else
    return QVariant{};
#endif
  case Qt::ToolTipRole:
    switch (index.column()) {
    case 0:
      return m_pimpl->m_credentials->cardByIndex(index.row())->username();
    case 1:
      return m_pimpl->m_credentials->cardByIndex(index.row())->alias();
    case 2:
      try {
        return AccessCard::accessTypeName(static_cast<int>(
            m_pimpl->m_credentials->cardByIndex(index.row())->access()));
      } catch (...) {
        return tr("Invalid access value");
      }
    case 4: {
      auto card{m_pimpl->m_credentials->cardByIndex(index.row())};
      if (m_pimpl->m_connections->containsCard(card)) {
        auto peer{m_pimpl->m_connections->peerByCard(card)};
        if (peer->status() == Peer::Status::Ready) {
          return tr("Connected");
        } else {
          return tr("Not connected");
        }
      } else {
        return tr("Not connected");
      }
    }
    default:
      return QVariant();
    }
  default:
    return QVariant();
  }
}

QVariant UserList::headerData(int section, Qt::Orientation orientation,
                              int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    switch (section) {
    case 0:
      return tr("Username");
    case 1:
      return tr("Chat alias");
    case 2:
      return tr("Access");
    case 3:
      return tr("Password");
    case 4:
      return tr("Status");
    default:
      return QVariant();
    }
  }
  return QVariant();
}

bool UserList::setData(const QModelIndex &index, const QVariant &value,
                       int role) {
  Q_UNUSED(role);
  switch (index.column()) {
  case 0: {
    QString uname{value.toString().trimmed()};
    if (uname.isEmpty())
      return false;
    if (m_pimpl->m_credentials->containsUsername(uname))
      return false;
    if (uname.contains(","))
      return false;
    if (uname.contains("\""))
      return false;
    if (uname.contains("/"))
      return false;
    if (uname.contains("\\"))
      return false;
    if (uname.contains("*"))
      return false;
    if (uname.contains(">"))
      return false;
    if (uname.contains("<"))
      return false;
    if (uname.contains("!"))
      return false;
    if (uname.contains("?"))
      return false;
    if (uname.contains(":"))
      return false;
    if (uname.contains("'"))
      return false;
    if (uname.contains("|"))
      return false;
    m_pimpl->m_credentials->cardByIndex(index.row())->username(uname);
    emit dataChanged(index, index);
    return true;
  }
  case 1: {
    QString ualias{value.toString().trimmed()};
    if (m_pimpl->m_credentials->containsAlias(ualias))
      return false;
    if (ualias.contains(","))
      return false;
    if (ualias.contains("\""))
      return false;
    if (ualias.contains("/"))
      return false;
    if (ualias.contains("\\"))
      return false;
    if (ualias.contains("*"))
      return false;
    if (ualias.contains(">"))
      return false;
    if (ualias.contains("<"))
      return false;
    if (ualias.contains("!"))
      return false;
    if (ualias.contains("?"))
      return false;
    if (ualias.contains(":"))
      return false;
    if (ualias.contains("'"))
      return false;
    if (ualias.contains("|"))
      return false;
    m_pimpl->m_credentials->cardByIndex(index.row())->alias(ualias);
    emit dataChanged(index, index);
    return true;
  }
  case 2:
    for (int i{0}; i <= static_cast<int>(AccessCard::AccessType::DISABLED);
         i++) {
      if (AccessCard::accessTypeName(i) == value.toString()) {
        auto card{m_pimpl->m_credentials->cardByIndex(index.row())};
        card->access(static_cast<AccessCard::AccessType>(i));
        emit dataChanged(index, index);
        return true;
      }
    }
    return false;
  case 3:
    if (value.toString().trimmed() == "")
      return false;
    m_pimpl->m_credentials->cardByIndex(index.row())
        ->password(value.toString());
    emit dataChanged(index, index);
    return true;
  default:
    return false;
  }
}

Qt::ItemFlags UserList::flags(const QModelIndex &index) const {
  Q_UNUSED(index);
  return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void UserList::createUser() {
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  m_pimpl->m_credentials->create();
  endInsertRows();
}

void UserList::addUser(const QString &username, const QString &alias,
                       const QString &password, AccessCard::AccessType r) {
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  m_pimpl->m_credentials->add(username, alias, password, r);
  endInsertRows();
}

void UserList::removeUser(int index) {
  beginRemoveRows(QModelIndex(), index, index);
  m_pimpl->m_credentials->removeByIndex(index);
  endRemoveRows();
}

int UserList::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return m_pimpl->m_credentials->count();
}

int UserList::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return 5;
}
