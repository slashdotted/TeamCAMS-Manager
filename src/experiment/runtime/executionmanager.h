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
#ifndef EXECUTIONMANAGER_H
#define EXECUTIONMANAGER_H
#include "experiment/operators/access/accesscard.h"
#include <QJsonObject>
#include <QObject>
#include <memory>

// TODO: manage timestamp as double
//       manage timestamp as delta from start of experiment
//       allow for asking timestamp as double (ms precision)

class ExecutionManager : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(ExecutionManager)
public:
  enum class ExecutionState { UNINITIALIZED, RUNNING, PAUSED };
  explicit ExecutionManager(QObject *parent = nullptr);
  ~ExecutionManager() override;
  ExecutionState state() const;

  // Serialization/Deserialization
  void load(const QJsonObject &obj);
  void save(QJsonObject &obj) const;
public slots:
  // Public / Scripting interface
  void resolution(unsigned int ms);
  unsigned int resolution() const;
  double timestamp() const;
  void speed(double factor);
  double speed() const;
  void restart();
  void reset();
  void initialize();
  void toggle();
  void resume();
  void pause();
  void step();
signals:
  void tick(int timestamp);
  void stateChanged(const ExecutionManager::ExecutionState &state,
                    const ExecutionManager::ExecutionState &previous);

private:
  struct pimpl;
  std::unique_ptr<pimpl> m_pimpl;
};

#endif // EXECUTIONMANAGER_H
