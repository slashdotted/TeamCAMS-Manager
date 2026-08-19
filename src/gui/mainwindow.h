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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "network/communicationmanager.h"
#include <QMainWindow>
#include <memory>

namespace Ui {
class MainWindow;
}

class MqttSimpleClient;
class Experiment;
class ExperimentList;

class MainWindow : public QMainWindow {
  Q_OBJECT
  Q_DISABLE_COPY(MainWindow)
public:
  explicit MainWindow(std::shared_ptr<CommunicationManager> s,
                      ExperimentList *el, QWidget *parent = nullptr);
  ~MainWindow() override;

private slots:
  // Actions
  void onImportExperimentListRequest();
  void onExportExperimentListRequest();
  void onNewExperimentRequest();
  void onRenameExperimentRequest();
  void onConfigureExperimentRequest();
  void onDeleteExperimentRequest();
  void onDuplicateExperimentRequest();
  void onConfigureApplicationSettingsRequest();
  void onStartExperimentRequest();
  void onStopExperimentRequest();
  void onShowAboutRequest();
#ifdef QT_TEST_ENABLED
  void onPerformTestsRequest();
#endif
  // Window events
  void toggleMainWindow();
  void quit();
  void closeEvent(QCloseEvent *event) override;
  // Experiment list events
  void onShowExperimentListContextMenuRequest(QPoint pos);
  void onExperimentListChanged();
  void onExperimentListSelectionChanged();

private:
  void deleteExperimentsHelper(QList<Experiment *> *tbr);
  void setupTrayIcon();
  void updateStatusbar();

  struct pimpl;
  std::unique_ptr<pimpl> m_pimpl;
};

#endif // MAINWINDOW_H
