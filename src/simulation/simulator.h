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

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "components/cabin.h"
#include "components/cooler.h"
#include "components/crew.h"
#include "components/dehumidifier.h"
#include "components/heater.h"
#include "components/mixer.h"
#include "components/pipe.h"
#include "components/scrubber.h"
#include "components/streamingcomponent.h"
#include "components/tank.h"
#include "components/valve.h"
#include "components/vent.h"
#include "control/control.h"
#include <QObject>

class Simulator : public QObject {
  Q_OBJECT

public:
  explicit Simulator(QObject *parent = nullptr);
  virtual ~Simulator();
  Registry *registry();

public slots:
  void initialize();
  void reset();
  void update(unsigned int timestamp);

private:
  struct pimpl;
  std::unique_ptr<pimpl> m_pimpl;
};

#endif // SIMULATOR_H
