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

#ifndef PERSISTENTSETTINGS_H
#define PERSISTENTSETTINGS_H

#include <QObject>
#include <QSettings>

class PersistentSettings : public QSettings {
  Q_OBJECT
public:
  explicit PersistentSettings(QObject *parent = nullptr);

  void setDefault(QAnyStringView key, const QVariant &value);
  void setUpdatedValue(QAnyStringView key, const QVariant &value);
  QVariant valueOrUpdated(QAnyStringView key) const;
  QVariant valueOrUpdated(QAnyStringView key,
                          const QVariant &defaultValue) const;
  bool containsUpdated(QAnyStringView key) const;
  void remove(QAnyStringView key);
  void applyUpdates();
  void clearUpdates();
  using QSettings::clear;
  void clear(const QStringList &keys);
  void resetToDefault();
  void resetUpdatedToDefault();
};

#endif // PERSISTENTSETTINGS_H
