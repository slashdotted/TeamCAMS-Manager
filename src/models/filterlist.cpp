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
#include "filterlist.h"
#include <QDebug>

struct FilterList::pimpl {
  pimpl(MessageFilter &filter) : m_filter{filter} {}
  MessageFilter &m_filter;
};

FilterList::FilterList(MessageFilter &filter, QObject *parent)
    : QAbstractListModel{parent}, m_pimpl{std::make_unique<pimpl>(filter)} {
  connect(&filter, &MessageFilter::cleared, this, [this]() {
    this->beginResetModel();
    this->endResetModel();
  });
  connect(&filter, &MessageFilter::ruleAdded, this,
          &FilterList::handleFilterAdded);
  connect(&filter, &MessageFilter::ruleRemoved, this,
          &FilterList::handleFilterRemoved);
  connect(&filter, &MessageFilter::ruleUpdated, this,
          &FilterList::handleFilterUpdated);
}

FilterList::~FilterList() = default;

int FilterList::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent)
  return m_pimpl->m_filter.count();
}

int FilterList::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent)
  return 5;
}

QVariant FilterList::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();
  MessageFilter::TrafficDirection d;
  MessageFilter::MessageType t;
  QString key;
  QString user;
  bool feedback;
  m_pimpl->m_filter.getRule(index.row(), d, t, key, user, feedback);
  switch (role) {
  case Qt::EditRole:
  case Qt::DisplayRole:
    switch (index.column()) {
    case 0:
      return MessageFilter::TrafficDirectionName[static_cast<int>(d)];
    case 1:
      return MessageFilter::MessageTypeName[static_cast<int>(t)];
    case 2:
      return key;
    case 3:
      return user;
    case 4:
      return feedback;
    }
    break;
  default:
    break;
  }
  return QVariant();
}

QVariant FilterList::headerData(int section, Qt::Orientation orientation,
                                int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    switch (section) {
    case 0:
      return tr("Direction");
    case 1:
      return tr("Type");
    case 2:
      return tr("Key");
    case 3:
      return tr("Username");
    case 4:
      return tr("Feedback");
    default:
      return QVariant();
    }
  }
  return QVariant();
}

bool FilterList::setData(const QModelIndex &index, const QVariant &value,
                         int role) {
  Q_UNUSED(role);
  if (!index.isValid())
    return false;
  MessageFilter::TrafficDirection d;
  MessageFilter::MessageType t;
  QString key;
  QString user;
  bool feedback;
  m_pimpl->m_filter.getRule(index.row(), d, t, key, user, feedback);
  switch (index.column()) {
  case 0: {
    // Direction
    for (int i{0};
         i <= static_cast<int>(MessageFilter::TrafficDirection::SERVER2CLIENT);
         i++) {
      if (MessageFilter::TrafficDirectionName[i] == value.toString()) {
        m_pimpl->m_filter.updateRule(
            index.row(), static_cast<MessageFilter::TrafficDirection>(i), t,
            key, user, feedback);
        return true;
      }
    }
    return false;
  }
  case 1: {
    // Type
    for (int i{0}; i <= static_cast<int>(MessageFilter::MessageType::CONTROL);
         i++) {
      if (MessageFilter::MessageTypeName[i] == value.toString()) {
        m_pimpl->m_filter.updateRule(index.row(), d,
                                     static_cast<MessageFilter::MessageType>(i),
                                     key, user, feedback);
        return true;
      }
    }
    return false;
  }
  case 2:
    // Key
    m_pimpl->m_filter.updateRule(index.row(), d, t, value.toString().trimmed(),
                                 user, feedback);
    return true;
  case 3:
    // Username
    m_pimpl->m_filter.updateRule(index.row(), d, t, key,
                                 value.toString().trimmed(), feedback);
    return true;
  case 4:
    // Feedback
    m_pimpl->m_filter.updateRule(index.row(), d, t, key, user, value.toBool());
    return true;
  default:
    return false;
  }
}

Qt::ItemFlags FilterList::flags(const QModelIndex &index) const {
  Q_UNUSED(index)
  return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void FilterList::add() {
  auto idx{
      m_pimpl->m_filter.addRule(MessageFilter::TrafficDirection::CLIENT2SERVER,
                                MessageFilter::MessageType::UPDATE,
                                "<key pattern>", "<username pattern>", false)};
  if (idx == rowCount()) {
    beginInsertRows(QModelIndex{}, rowCount(), rowCount());
    endInsertRows();
  }
}

void FilterList::remove(int index) {
  beginRemoveRows(QModelIndex(), index, index);
  m_pimpl->m_filter.remove(index);
  endRemoveRows();
}

void FilterList::handleFilterRemoved(int index) {
  beginRemoveRows(QModelIndex(), index, index);
  endRemoveRows();
}

void FilterList::handleFilterUpdated(int i) {
  if (i >= 0) {
    emit dataChanged(index(i), index(i));
  }
}

void FilterList::handleFilterAdded(int i) {
  beginInsertRows(QModelIndex(), i, i);
  endInsertRows();
}
