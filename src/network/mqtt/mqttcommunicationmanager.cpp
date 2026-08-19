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

#include "mqttcommunicationmanager.h"
#include "mqttexperimentserver.h"
#include "mqtttransport.h"

struct MqttCommunicationManager::pimpl {
  pimpl(const QString &managerUuid)
      : m_transport{std::make_shared<MqttTransport>(managerUuid)} {}

  std::shared_ptr<MqttTransport> m_transport;
};

MqttCommunicationManager::MqttCommunicationManager(const QString &managerUuid)
    : m_pimpl{std::make_unique<pimpl>(managerUuid)} {}

MqttTransport &MqttCommunicationManager::mqttTransport() {
  return *m_pimpl->m_transport.get();
}

MqttCommunicationManager::~MqttCommunicationManager() = default;

std::shared_ptr<MessageTransport> MqttCommunicationManager::transport() {
  return m_pimpl->m_transport;
}

std::shared_ptr<ExperimentServer>
MqttCommunicationManager::newExperimentServer(Experiment *experiment) {
  return std::shared_ptr<ExperimentServer>{
      new MqttExperimentServer{m_pimpl->m_transport, experiment}};
}
