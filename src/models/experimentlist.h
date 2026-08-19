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
#ifndef EXPERIMENTLIST_H
#define EXPERIMENTLIST_H

#include "experiment/experiment.h"
#include <QAbstractTableModel>
#include <QList>
#include <memory>

struct AuthToken {
  bool valid{false};
  QString eid;
  QString username;
};

class ExperimentList : public QAbstractTableModel {
  Q_OBJECT
  Q_DISABLE_COPY(ExperimentList)
public:
  ExperimentList(std::shared_ptr<CommunicationManager> cmanager);
  ~ExperimentList() override;

  int rowCount(const QModelIndex &parent) const override;
  int columnCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role) const override;
  AuthToken parseToken(const QString &token) const;
  bool hasExperiment(const QString &id) const;
  Experiment *getExperiment(const QString &id) const;
  Experiment *getExperiment(int index) const;
  bool removeExperiment(const QString &id);
  bool removeExperiment(int index);
  bool addExperiment(const QString &id);
  void addExperiment(Experiment *e);

  int count() const;
  QStringList names() const;

  QJsonDocument toJson() const;
  void fromJson(const QJsonDocument &doc);

  void load(const QJsonObject &json);
  void save(QJsonObject &json) const;

  void cleanup();
public slots:
  void updateExperimentList();
  void updateExperimentTime();

private slots:
  void onContentChanged();

signals:
  void contentChanged();

private:
  struct pimpl;
  std::unique_ptr<pimpl> m_pimpl;
};

#endif // EXPERIMENTLIST_H
