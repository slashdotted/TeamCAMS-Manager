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
#include "experimentlist.h"
#ifdef MANAGERGUI
#include <QFont>
#include <QPixmap>
#endif
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTime>

struct ExperimentList::pimpl {
  pimpl(std::shared_ptr<CommunicationManager> cmanager)
      : m_cmanager{cmanager} {}
  std::shared_ptr<CommunicationManager> m_cmanager;
  QList<Experiment *> m_experiments;
};

ExperimentList::ExperimentList(std::shared_ptr<CommunicationManager> cmanager)
    : m_pimpl{std::make_unique<pimpl>(cmanager)} {}

ExperimentList::~ExperimentList() {
  for (int i = 0; i < m_pimpl->m_experiments.size(); i++) {
    delete m_pimpl->m_experiments.at(i);
  }
}

int ExperimentList::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return m_pimpl->m_experiments.size();
}

int ExperimentList::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return 6;
}

QVariant ExperimentList::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();
  Experiment *exp = m_pimpl->m_experiments.at(index.row());
  switch (role) {
  case Qt::DisplayRole:
    switch (index.column()) {
    case 0:
      return exp->name();
    case 1:
      return exp->description();
    case 3:
      return QString{"%1/%2"}
          .arg(exp->operatorsManager()->cards().size())
          .arg(exp->credentialManager()->count());
    case 4:
      return QTime(0, 0)
          .addSecs(exp->executionManager()->timestamp() / 1000)
          .toString("hh:mm:ss");
    }
    break;
  case Qt::DecorationRole:
#ifdef MANAGERGUI
    switch (index.column()) {
    case 2:
      switch (exp->executionManager()->state()) {
      case ExecutionManager::ExecutionState::RUNNING:
        return QPixmap(":/images/media-playback-start.png").scaledToHeight(16);
      case ExecutionManager::ExecutionState::UNINITIALIZED:
        return QPixmap(":/images/media-playback-stop.png").scaledToHeight(16);
      case ExecutionManager::ExecutionState::PAUSED:
        return QPixmap(":/images/media-playback-pause.png").scaledToHeight(16);
      }
      break;
    case 5:
      if (exp->enabled()) {
        return QPixmap(":/images/user-available.png").scaledToHeight(16);
      } else {
        return QPixmap(":/images/list-remove.png").scaledToHeight(16);
      }
    }
    break;
#else
    return QVariant{};
#endif
  case Qt::FontRole:
#ifdef MANAGERGUI
    if (index.column() == 0) {
      QFont font;
      font.setBold(true);
      return font;
    }
    break;
#else
    return QVariant{};
#endif
  case Qt::ToolTipRole:
    switch (index.column()) {
    case 3:
      QString tooltip;
      auto ulist = exp->operatorsManager()->cards();
      for (const auto &u : std::as_const(ulist)) {
        tooltip += u->username();
        if (u != ulist.last()) {
          tooltip += ", ";
        }
      }
      return tooltip;
    }

  default:
    break;
  }
  return QVariant();
}

QVariant ExperimentList::headerData(int section, Qt::Orientation orientation,
                                    int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    switch (section) {
    case 0:
      return tr("Name");
    case 1:
      return tr("Description");
    case 2:
      return tr("Status");
    case 3:
      return tr("Users");
    case 4:
      return tr("Time");
    case 5:
      return tr("Available?");
    default:
      return QVariant();
    }
  }
  return QVariant();
}

AuthToken ExperimentList::parseToken(const QString &token) const {
  for (const auto &e : std::as_const(m_pimpl->m_experiments)) {
    auto attempt{e->credentialManager()->authenticate(token)};
    if (attempt.isValid()) {
      return AuthToken{true, e->name(), attempt.toString()};
    }
  }
  return AuthToken{false};
}

bool ExperimentList::hasExperiment(const QString &id) const {
  for (const auto &e : std::as_const(m_pimpl->m_experiments)) {
    if (e->name() == id) {
      return true;
    }
  }
  return false;
}

Experiment *ExperimentList::getExperiment(const QString &id) const {
  for (const auto &e : std::as_const(m_pimpl->m_experiments)) {
    if (e->name() == id) {
      return e;
    }
  }
  return nullptr;
}

Experiment *ExperimentList::getExperiment(int index) const {
  return m_pimpl->m_experiments.at(index);
}

bool ExperimentList::removeExperiment(const QString &id) {
  int index{0};
  for (const auto &e : std::as_const(m_pimpl->m_experiments)) {
    if (e->name() == id) {
      beginRemoveRows(QModelIndex(), index, index);
      m_pimpl->m_experiments.removeAt(index);
      endRemoveRows();
      emit contentChanged();
      return true;
    }
    index++;
  }
  return false;
}

bool ExperimentList::removeExperiment(int index) {
  beginRemoveRows(QModelIndex(), index, index);
  m_pimpl->m_experiments.removeAt(index);
  endRemoveRows();
  emit contentChanged();
  return true;
}

bool ExperimentList::addExperiment(const QString &id) {
  if (!hasExperiment(id)) {
    Experiment *e{new Experiment(id, m_pimpl->m_cmanager, this)};
    addExperiment(e);
    return true;
  } else {
    return false;
  }
}

void ExperimentList::addExperiment(Experiment *e) {
  beginInsertRows(QModelIndex(), m_pimpl->m_experiments.size(),
                  m_pimpl->m_experiments.size());
  m_pimpl->m_experiments.push_back(e);
  connect(e->executionManager(), &ExecutionManager::stateChanged, this,
          &ExperimentList::updateExperimentList);
  connect(e->operatorsManager(), &OperatorsManager::clientConnected, this,
          &ExperimentList::updateExperimentList);
  connect(e->operatorsManager(), &OperatorsManager::clientDisconnected, this,
          &ExperimentList::updateExperimentList);
  connect(e->executionManager(), &ExecutionManager::tick, this,
          &ExperimentList::updateExperimentTime);
  connect(e, &Experiment::enabledStateChanged, this,
          &ExperimentList::updateExperimentList);
  connect(e, &Experiment::contentChanged, this,
          &ExperimentList::onContentChanged);
  endInsertRows();
}

int ExperimentList::count() const { return m_pimpl->m_experiments.count(); }

QStringList ExperimentList::names() const {
  QStringList nlist;
  for (const auto &e : std::as_const(m_pimpl->m_experiments)) {
    if (e->enabled()) {
      nlist.append(e->name());
    }
  }
  return nlist;
}

QJsonDocument ExperimentList::toJson() const {
  QJsonObject saveObject;
  save(saveObject);
  QJsonDocument saveDoc(saveObject);
  return saveDoc;
}

void ExperimentList::fromJson(const QJsonDocument &doc) { load(doc.object()); }

void ExperimentList::load(const QJsonObject &json) {
  for (auto a : json["experiments"].toArray()) {
    Experiment *e = new Experiment("", m_pimpl->m_cmanager, this);
    e->load(a.toObject(), false);
    if (!hasExperiment(e->name())) {
      addExperiment(e);
    } else {
      delete e;
    }
  }
}

void ExperimentList::save(QJsonObject &json) const {
  QJsonArray elist;
  for (const auto &exp : std::as_const(m_pimpl->m_experiments)) {
    QJsonObject e;
    exp->save(e);
    elist.push_back(e);
  }
  json["experiments"] = elist;
}

void ExperimentList::updateExperimentList() {
  if (m_pimpl->m_experiments.size() > 0) {
    emit dataChanged(createIndex(0, 0),
                     createIndex(m_pimpl->m_experiments.size() - 1, 4));
  }
}

void ExperimentList::updateExperimentTime() {
  if (m_pimpl->m_experiments.size() > 0) {
    emit dataChanged(createIndex(0, 4),
                     createIndex(m_pimpl->m_experiments.size() - 1, 4));
  }
}

void ExperimentList::onContentChanged() { emit contentChanged(); }

void ExperimentList::cleanup() {
  for (const auto &exp : std::as_const(m_pimpl->m_experiments)) {
    exp->executionManager()->reset();
    exp->cleanup();
  }
  beginResetModel();
  m_pimpl->m_experiments.clear();
  endResetModel();
  emit contentChanged();
}
