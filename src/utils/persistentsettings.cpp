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

#include "persistentsettings.h"

PersistentSettings::PersistentSettings(QObject *parent) : QSettings{parent} {}

void PersistentSettings::setDefault(QAnyStringView key, const QVariant &value) {
  setValue(key.toString() + ".default", value);
  if (!contains(key)) {
    setValue(key, value);
  }
}

void PersistentSettings::setUpdatedValue(QAnyStringView key,
                                         const QVariant &value) {
  QString tempKey{key.toString() + ".temp"};
  setValue(tempKey, value);
}

QVariant PersistentSettings::valueOrUpdated(QAnyStringView key) const {
  QString tempKey{key.toString() + ".temp"};
  if (contains(tempKey)) {
    return value(tempKey);
  } else {
    return value(key);
  }
}

QVariant
PersistentSettings::valueOrUpdated(QAnyStringView key,
                                   const QVariant &defaultValue) const {
  QString tempKey{key.toString() + ".temp"};
  if (contains(tempKey)) {
    return value(tempKey);
  } else {
    return value(key, defaultValue);
  }
}

bool PersistentSettings::containsUpdated(QAnyStringView key) const {
  QString tempKey{key.toString() + ".temp"};
  auto value = QSettings::value(key);
  auto tempValue = QSettings::value(tempKey);
  return contains(tempKey) && value != tempValue;
}

void PersistentSettings::remove(QAnyStringView key) {
  QString tempKey{key.toString() + ".temp"};
  if (contains(tempKey)) {
    QSettings::remove(tempKey);
  }
  QSettings::remove(key);
}

void PersistentSettings::applyUpdates() {
  auto keys{allKeys()};
  for (const auto &k : keys) {
    if (k.endsWith(".temp")) {
      auto realKey{k.left(k.length() - 5)};
      setValue(realKey, value(k));
      QSettings::remove(k);
    }
  }
}

void PersistentSettings::clearUpdates() {
  auto keys{allKeys()};
  for (const auto &k : keys) {
    if (k.endsWith(".temp")) {
      QSettings::remove(k);
    }
  }
}

void PersistentSettings::clear(const QStringList &keys) {
  for (const auto &k : keys) {
    remove(k);
  }
}

void PersistentSettings::resetToDefault() {
  auto keys{allKeys()};
  for (const auto &k : keys) {
    if (k.endsWith(".default")) {
      auto realKey{k.left(k.length() - 8)};
      setValue(realKey, value(k));
      QSettings::remove(realKey);
      QSettings::remove(realKey + ".temp");
    }
  }
}

void PersistentSettings::resetUpdatedToDefault() {
  auto keys{allKeys()};
  for (const auto &k : keys) {
    if (k.endsWith(".default")) {
      auto realKey{k.left(k.length() - 8)};
      setUpdatedValue(realKey, value(k));
    }
  }
}
