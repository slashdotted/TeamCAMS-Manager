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
#include "executionmanager.h"
#include <QDebug>
#include <QTime>
#include <QTimer>
#ifdef MANAGERGUI
#include <QApplication>
#include <QMessageBox>
#endif

struct ExecutionManager::pimpl {
  QTimer m_timer;
  unsigned int m_resolution{10};
  double m_speed_factor{1.0};
  ExecutionState m_state{ExecutionState::UNINITIALIZED};
  unsigned int m_ticks{0};
};

ExecutionManager::ExecutionManager(QObject *parent)
    : QObject{parent}, m_pimpl{std::make_unique<pimpl>()} {
  connect(&m_pimpl->m_timer, &QTimer::timeout, this, &ExecutionManager::step);
}

ExecutionManager::~ExecutionManager() = default;

void ExecutionManager::resolution(unsigned int ms) {
  if (ms > 0) {
    m_pimpl->m_resolution = ms;
  }
}

unsigned int ExecutionManager::resolution() const {
  return m_pimpl->m_resolution;
}

double ExecutionManager::timestamp() const {
  return m_pimpl->m_ticks * resolution();
}

void ExecutionManager::speed(double factor) {
  if (factor > 0) {
    m_pimpl->m_speed_factor = factor;
    m_pimpl->m_timer.setInterval(
        static_cast<int>(resolution() / m_pimpl->m_speed_factor));
  }
}

double ExecutionManager::speed() const { return m_pimpl->m_speed_factor; }

void ExecutionManager::restart() {
  auto previous{m_pimpl->m_state};
  m_pimpl->m_timer.stop();
  m_pimpl->m_ticks = 0;
  resolution(resolution());
  speed(speed());
  initialize();
  emit stateChanged(m_pimpl->m_state = ExecutionState::PAUSED,
                    ExecutionState::UNINITIALIZED);
}

ExecutionManager::ExecutionState ExecutionManager::state() const {
  return m_pimpl->m_state;
}

void ExecutionManager::load(const QJsonObject &obj) {
  if (obj.contains("resolution")) {
    resolution(static_cast<unsigned int>(obj["resolution"].toInt()));
  }
  if (obj.contains("speedFactor")) {
    speed(obj["speedFactor"].toDouble());
  }
}

void ExecutionManager::save(QJsonObject &obj) const {
  obj["resolution"] = static_cast<int>(resolution());
  obj["speedFactor"] = speed();
}

void ExecutionManager::initialize() {
  m_pimpl->m_timer.stop();
  m_pimpl->m_ticks = 0;
  m_pimpl->m_state = ExecutionState::PAUSED;
}

void ExecutionManager::toggle() {
  if (m_pimpl->m_state != ExecutionState::RUNNING) {
    resume();
  } else {
    pause();
  }
}

void ExecutionManager::resume() {
  if (m_pimpl->m_state == ExecutionState::UNINITIALIZED) {
    try {
      reset();
      initialize();
      emit stateChanged(m_pimpl->m_state = ExecutionState::PAUSED,
                        ExecutionState::UNINITIALIZED);
    } catch (const std::exception &e) {
#ifdef MANAGERGUI
      QMessageBox *msgBox = new QMessageBox(QApplication::activeWindow());
      msgBox->setIcon(QMessageBox::Critical);
      msgBox->setWindowTitle(tr("Critical"));
      msgBox->setText(QString::fromUtf8(e.what()));
      msgBox->setStandardButtons(QMessageBox::Ok);
      msgBox->setAttribute(Qt::WA_DeleteOnClose);
      msgBox->open();
#else
      qError() << e.what();
#endif
      reset();
      return;
    }
  }
  if (m_pimpl->m_state == ExecutionState::PAUSED) {
    m_pimpl->m_timer.start();
    emit stateChanged(m_pimpl->m_state = ExecutionState::RUNNING,
                      ExecutionState::PAUSED);
  }
}

void ExecutionManager::pause() {
  if (m_pimpl->m_state == ExecutionState::RUNNING) {
    m_pimpl->m_timer.stop();
    emit stateChanged(m_pimpl->m_state = ExecutionState::PAUSED,
                      ExecutionState::RUNNING);
  }
}

void ExecutionManager::reset() {
  auto previous{m_pimpl->m_state};
  m_pimpl->m_timer.stop();
  m_pimpl->m_ticks = 0;
  resolution(resolution());
  speed(speed());
  emit stateChanged(m_pimpl->m_state = ExecutionState::UNINITIALIZED, previous);
}

void ExecutionManager::step() {
  auto current{++m_pimpl->m_ticks};
  auto interval{1000 / resolution()};
  if (current % interval == 0) {
    emit tick(current * resolution() / 1000);
  }
}
