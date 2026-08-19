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
#ifndef EXECUTIONADAPTER_H
#define EXECUTIONADAPTER_H
#include "experiment/runtime/executionmanager.h"
#include <QObject>

class ExecutionAdapter : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(ExecutionAdapter)
public:
  ExecutionAdapter(ExecutionManager *em);

public slots:
  /* Simulation control */
  void toggle();
  void resume();
  void pause();
  void stop();
  void step();
  double timestamp() const;
  double speed() const;
  void speed(double factor) const;
};

#endif // EXECUTIONADAPTER_H
