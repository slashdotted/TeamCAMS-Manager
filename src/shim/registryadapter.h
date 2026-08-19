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
#ifndef REGISTRYADAPTER_H
#define REGISTRYADAPTER_H
#include "experiment/experiment.h"
#include "simulation/registry.h"
#include <QObject>

class RegistryAdapter : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(RegistryAdapter)
public:
  explicit RegistryAdapter(Registry *reg);

public slots:
  void set(const QString &name, const QJsonValue &value,
           bool initialize = false);
  QJsonValue get(const QString &name, bool ignoreUndefined = false);
  void stick(const QString &name, const QJsonValue &value);
  void stick(const QString &name);
  void setAndStick(const QString &name, const QJsonValue &value);
  void unstick(const QString &name);
  bool isStuck(const QString &name);
  void dump();
  bool exists(const QString &name);
  void touch(const QString &name);
  void reset();
  QStringList stuckProperties();
};

#endif // REGISTRYADAPTER_H
