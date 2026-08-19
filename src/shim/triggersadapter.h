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
#ifndef TRIGGERSADAPTER_H
#define TRIGGERSADAPTER_H

#include "events/triggers.h"
#include <QObject>
#include <QString>

class TriggersAdapter : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(TriggersAdapter)
public:
  explicit TriggersAdapter(Triggers *triggers);

public slots:
  /* Schedules a new trigger */
  QString activate(const QString &msgPattern, const QString &code,
                   const QString &reference = "", const QString &type = "");
  /* Unschedules the trigger and all other events which refer to it */
  void deactivateByUuid(const QString &event);
  /* Unschedules the trigger and all other events which refer to it */
  void deactivateByType(const QString &type);
  /* Check if the event exists */
  bool exists(const QString &event) const;
  /* Process trigger */
  void processTriggersWithBindings(const QString &trigger,
                                   const QString &bindings);
  void processTriggers(const QString &trigger);
  void reset();
};

#endif // TRIGGERSADAPTER_H
