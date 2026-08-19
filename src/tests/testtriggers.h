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
#ifndef TESTTRIGGERS_H
#define TESTTRIGGERS_H

#include <QObject>

class TestTriggers : public QObject {
  Q_OBJECT
public:
  explicit TestTriggers(QObject *parent = nullptr);

public slots:
  void onEvaluateRequest(const QString &code, const QVariantMap &bindings);

private slots:
  void testActivate();
  void testDeactivate();
  void testProcessTriggers();

private:
  void clearSignalData();
  QString m_code;
};

#endif // TESTTRIGGERS_H
