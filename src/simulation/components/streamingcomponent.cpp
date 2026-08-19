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
#include "streamingcomponent.h"

StreamingComponent::~StreamingComponent() {}

StreamingComponent *StreamingComponent::connectSink(StreamingComponent *d,
                                                    const QString &flow) {
  m_sinks[flow].insert(d);
  d->m_sources[flow].insert(this);
  return d;
}

void StreamingComponent::connectSource(StreamingComponent *d,
                                       const QString &flow) {
  d->m_sinks[flow].insert(this);
  m_sources[flow].insert(d);
}

void StreamingComponent::pushToSinks(Registry &s, double amount,
                                     const QString &flow, bool proportional) {
  double effective{proportional ? amount : amount / m_sinks[flow].size()};
  for (auto d : m_sinks[flow]) {
    d->push(s, effective, flow);
  }
}

void StreamingComponent::pullFromSources(Registry &s, double amount,
                                         const QString &flow,
                                         bool proportional) {
  double effective{proportional ? amount : amount / m_sources[flow].size()};
  for (auto d : m_sources[flow]) {
    d->pull(s, effective, flow);
  }
}
