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
#ifndef FILTERLIST_H
#define FILTERLIST_H

#include "experiment/operators/messagefilter.h"
#include <QAbstractListModel>
#include <QObject>
#include <memory>

class FilterList : public QAbstractListModel {
  Q_OBJECT
public:
  explicit FilterList(MessageFilter &filter, QObject *parent = nullptr);
  virtual ~FilterList();
  FilterList(const FilterList &) = delete;
  FilterList &operator=(const FilterList &) = delete;
  FilterList(const FilterList &&) = delete;
  FilterList &operator=(const FilterList &&) = delete;

  int rowCount(const QModelIndex &parent = QModelIndex{}) const;
  int columnCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role);
  Qt::ItemFlags flags(const QModelIndex &index) const;
  void add();
  void remove(int index);

private slots:
  void handleFilterAdded(int i);
  void handleFilterRemoved(int i);
  void handleFilterUpdated(int i);

private:
  struct pimpl;
  std::unique_ptr<pimpl> m_pimpl;
};

#endif // FILTERLIST_H
