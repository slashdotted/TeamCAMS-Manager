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
#include "propertylist.h"
#ifdef MANAGERGUI
#include <QPixmap>
#endif

struct PropertyList::pimpl {
  pimpl(Registry *registry) : m_registry{registry} {}
  Registry *m_registry;
};

PropertyList::PropertyList(Registry *registry, QObject *parent)
    : QAbstractListModel{parent}, m_pimpl{std::make_unique<pimpl>(registry)} {
  connect(m_pimpl->m_registry, &Registry::propertyAdded, this, [this]() {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    endInsertRows();
  });
  connect(m_pimpl->m_registry, &Registry::beginReset, this,
          [this]() { beginResetModel(); });
  connect(m_pimpl->m_registry, &Registry::endReset, this,
          [this]() { endResetModel(); });
  connect(m_pimpl->m_registry, &Registry::changed, this, [this]() {
    const int count = m_pimpl->m_registry->count();
    if (count > 0) {
      emit dataChanged(index(0), index(count - 1));
    }
  });
  connect(m_pimpl->m_registry, &Registry::sticknessChanged, this,
          [this](const QString &name) {
            auto idx{m_pimpl->m_registry->index(name)};
            emit dataChanged(index(idx), index(idx));
          });
}

PropertyList::~PropertyList() = default;

QVariant PropertyList::data(const QModelIndex &index, int role) const {
  switch (role) {
  case Qt::DisplayRole:
    switch (index.column()) {
    case 0:
      return m_pimpl->m_registry->name(index.row());
    case 1:
      if (m_pimpl->m_registry->isStuck(index.row())) {
        return m_pimpl->m_registry->value(index.row()).toString().toUpper();
      } else {
        return m_pimpl->m_registry->value(index.row());
      }
    default:
      return QVariant();
    }
  case Qt::TextAlignmentRole:
    return int(Qt::AlignLeft);
  case Qt::DecorationRole:
    return QVariant{};
  default:
    return QVariant();
  }
}

QVariant PropertyList::headerData(int section, Qt::Orientation orientation,
                                  int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    switch (section) {
    case 0:
      return tr("Name");
    case 1:
      return tr("Value");
    default:
      return QVariant();
    }
  }
  return QVariant();
}

Qt::ItemFlags PropertyList::flags(const QModelIndex &index) const {
  Q_UNUSED(index);
  return Qt::ItemIsEnabled;
}

int PropertyList::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return m_pimpl->m_registry->count();
}

int PropertyList::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return 2;
}
