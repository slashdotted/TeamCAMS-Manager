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
#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "content/contentlibrary.h"
#include "events/scheduler.h"
#include "events/triggers.h"
#include "logging/filelogger.h"
#include "logging/loggingmanager.h"
#include "network/communicationmanager.h"
#include "operators/access/accessmanager.h"
#include "operators/operatorsmanager.h"
#include "runtime/executionmanager.h"
#include "scripting/scriptingmanager.h"
#include "simulation/simulator.h"
#include <QObject>
#include <QString>
#include <memory>

class Experiment : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(Experiment)
public:
  // Ctor / Dtor
  explicit Experiment(
      const QString &name,
      std::shared_ptr<CommunicationManager> communicationManager = nullptr,
      QObject *parent = nullptr);
  ~Experiment() override;

  // Serialization
  void load(const QJsonObject &json, bool ignore_name = true);
  void save(QJsonObject &json) const;
  void enabled(bool value);
  bool enabled();

  // Experiment components
  AccessManager *credentialManager() const;
  OperatorsManager *operatorsManager() const;
  Simulator *simulator() const;
  Scheduler *scheduler() const;
  Triggers *triggers() const;
  ExecutionManager *executionManager() const;
  ScriptingManager *scriptingManager() const;
  LoggingManager *loggingManager() const;
  ContentLibrary *contentLibrary() const;
  ExperimentServer *experimentServer() const;

signals:
  void enabledStateChanged(bool enabled);
  void contentChanged();

public slots:
  // Basic information
  const QString &name() const;
  void name(const QString &name);
  const QString &description() const;
  void description(const QString &desc);
  void script(const QString &scriptname);
  const QString &script() const;
  void onContentChanged();
  void cleanup();

  void initialize();
  void reset();

private:
  struct pimpl;
  std::unique_ptr<pimpl> m_pimpl;
};

#endif // EXPERIMENT_H
