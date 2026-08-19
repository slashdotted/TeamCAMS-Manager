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
#ifndef STATE_H
#define STATE_H

#include <QJsonValue>
#include <QMap>
#include <QObject>
#include <QSet>
#include <QString>
#include <memory>

class Registry : public QObject {
  Q_OBJECT
public:
  explicit Registry(QObject *parent = nullptr);
  virtual ~Registry();

public slots:
  // TODO: Enforce .const properties
  // TODO: dump / restore all registry with value and stuck properties
  void set(const QString &name, const QJsonValue &value,
           bool initialize = false, const QString &who = "<system>");
  QJsonValue get(const QString &name, bool ignoreUndefined = false) const;
  void stick(const QString &name, const QJsonValue &value);
  void stick(const QString &name);
  void setAndStick(const QString &name, const QJsonValue &value);
  void unstick(const QString &name);
  bool isStuck(const QString &name) const;
  void dump(const QString &notifyTarget = "") const;
  bool exists(const QString &name) const;
  void touch(const QString &name) const;
  void reset();
  QStringList stuckProperties() const;
  // Methods used for the registry view
  QString name(unsigned int i) const;
  QJsonValue value(unsigned int i) const;
  bool isStuck(unsigned int i) const;
  unsigned int count() const;
  unsigned int index(const QString &name) const;

signals:
  void changed(const QString &name, const QJsonValue &value, const QString &who,
               bool skipLogging = false,
               const QString &notifyTarget = "") const;
  void propertyAdded();
  void beginReset();
  void endReset();
  void sticknessChanged(const QString &name);

private:
  struct pimpl;
  std::unique_ptr<pimpl> m_pimpl;
};

#endif // STATE_H
