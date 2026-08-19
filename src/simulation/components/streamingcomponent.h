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
#ifndef DEVICE_H
#define DEVICE_H
#include "simulation/registry.h"
#include <QList>
#include <QMap>
#include <QSet>
#include <QString>

// Different amounts must be computed in the pull phase
// for example:
// - if a valve blocks X% of the flow:
//          - pull reduces the requested amount forwarded to pullFromSources
//            to 1-X% of the request
//          - push forwards everything to pushToSinks
// - if a valve leaks Y% of the flow:
//          - pull increases the requested amount by Y%
//          - push reduces the received amount by Y% (the rest is leaked)
class StreamingComponent {
public:
  virtual ~StreamingComponent();
  virtual void push(Registry &s, double amount, const QString &flow) = 0;
  virtual void pull(Registry &s, double amount, const QString &flow) = 0;
  StreamingComponent *connectSink(StreamingComponent *d,
                                  const QString &flow = "default");
  void connectSource(StreamingComponent *d, const QString &flow = "default");

protected:
  void pushToSinks(Registry &s, double amount, const QString &flow = "default",
                   bool proportional = false);
  void pullFromSources(Registry &s, double amount,
                       const QString &flow = "default",
                       bool proportional = false);

private:
  QMap<QString, QSet<StreamingComponent *>> m_sinks;
  QMap<QString, QSet<StreamingComponent *>> m_sources;
};

#endif // DEVICE_H
