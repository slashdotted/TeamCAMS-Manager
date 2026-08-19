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

#include "gui/mainwindow.h"
#include <QAction>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QProgressDialog>
#include <QSystemTrayIcon>
#include <QTimer>
#ifdef QT_TEST_ENABLED
#include "tests/testcabin.h"
#include "tests/testcontrol.h"
#include "tests/testcooler.h"
#include "tests/testcrew.h"
#include "tests/testdehumidifier.h"
#include "tests/testexperiment.h"
#include "tests/testexperimentlist.h"
#include "tests/testheater.h"
#include "tests/testmixer.h"
#include "tests/testpipe.h"
#include "tests/testregistry.h"
#include "tests/testscheduler.h"
#include "tests/testscrubber.h"
#include "tests/testsensor.h"
#include "tests/testsimulator.h"
#include "tests/testtank.h"
#include "tests/testtriggers.h"
#include "tests/testuserlist.h"
#include "tests/testvalve.h"
#include "tests/testvent.h"
#include "tests/testxmlconfigurationparser.h"
#include <QtTest/QtTest>
#endif
#include "appconfigurationdialog.h"
#include "gui/asyncmessageboxfactory.h"
#include "gui/configureexperimentdialog.h"
#include "gui/newexperimentdialog.h"
#include "ui_mainwindow.h"
#include "utils/persistentsettings.h"

struct MainWindow::pimpl {
  pimpl(Ui::MainWindow *uiptr, ExperimentList *el,
        std::shared_ptr<CommunicationManager> s)
      : ui{uiptr}, m_experiment_list{el}, m_cmanager{s} {}

  Ui::MainWindow *ui;
  ExperimentList *m_experiment_list;
  QSystemTrayIcon *m_tray_icon;
  QMenu *m_tray_icon_menu;
  std::shared_ptr<CommunicationManager> m_cmanager;
  QString m_settingsfile;
  QLabel *m_status_label;
  QString m_zconfname;
};

MainWindow::MainWindow(std::shared_ptr<CommunicationManager> s,
                       ExperimentList *el, QWidget *parent)
    : QMainWindow(parent),
      m_pimpl{std::make_unique<pimpl>(new Ui::MainWindow, el, s)} {
  m_pimpl->ui->setupUi(this);
  m_pimpl->ui->tableView->setModel(m_pimpl->m_experiment_list);
  m_pimpl->ui->tableView->setSelectionMode(
      QAbstractItemView::ExtendedSelection);
  m_pimpl->ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_pimpl->ui->tableView->horizontalHeader()->resizeSection(0, 100);
  m_pimpl->ui->tableView->horizontalHeader()->setSectionResizeMode(
      1, QHeaderView::Stretch);
  m_pimpl->ui->tableView->horizontalHeader()->resizeSection(2, 50);
  m_pimpl->ui->tableView->horizontalHeader()->resizeSection(3, 50);
  m_pimpl->ui->tableView->setWordWrap(true);
  m_pimpl->ui->tableView->verticalHeader()->setSectionResizeMode(
      QHeaderView::ResizeToContents);
  m_pimpl->ui->tableView->setTextElideMode(Qt::ElideRight);
  m_pimpl->ui->tableView->verticalHeader()->setSectionResizeMode(
      QHeaderView::Fixed);
  m_pimpl->ui->tableView->verticalHeader()->setDefaultSectionSize(50);
  m_pimpl->ui->tableView->setAlternatingRowColors(true);
  setupTrayIcon();
  connect(m_pimpl->ui->actionOpen_experiment__manager, &QAction::triggered,
          this, &MainWindow::toggleMainWindow);
  connect(m_pimpl->ui->actionQuit, &QAction::triggered, this,
          &MainWindow::quit);
  connect(m_pimpl->ui->actionNew_Experiment, &QAction::triggered, this,
          &MainWindow::onNewExperimentRequest);
  connect(m_pimpl->ui->actionRename_Experiment, &QAction::triggered, this,
          &MainWindow::onRenameExperimentRequest);
  connect(m_pimpl->ui->actionEdit_Experiment, &QAction::triggered, this,
          &MainWindow::onConfigureExperimentRequest);
  connect(m_pimpl->ui->tableView, &QTableView::doubleClicked, this,
          &MainWindow::onConfigureExperimentRequest);
  connect(m_pimpl->ui->actionDelete_Experiment, &QAction::triggered, this,
          &MainWindow::onDeleteExperimentRequest);

#ifndef QT_TEST_ENABLED
  m_pimpl->ui->actionRun_tests->setVisible(false);
  m_pimpl->ui->actionRun_tests->setEnabled(false);
#else
  connect(m_pimpl->ui->actionRun_tests, &QAction::triggered, this,
          &MainWindow::onPerformTestsRequest);
#endif

#ifdef Q_OS_WASM
  m_pimpl->ui->actionRun_tests->setDisabled(true);
#endif

  connect(m_pimpl->ui->actionExport_experiments, &QAction::triggered, this,
          &MainWindow::onExportExperimentListRequest);
  connect(m_pimpl->ui->actionImport_experiments, &QAction::triggered, this,
          &MainWindow::onImportExperimentListRequest);
  connect(m_pimpl->ui->tableView->selectionModel(),
          &QItemSelectionModel::selectionChanged, this,
          &MainWindow::onExperimentListSelectionChanged);
  connect(m_pimpl->ui->actionStart, &QAction::triggered, this,
          &MainWindow::onStartExperimentRequest);
  connect(m_pimpl->ui->actionStop, &QAction::triggered, this,
          &MainWindow::onStopExperimentRequest);
  connect(m_pimpl->m_experiment_list, &ExperimentList::dataChanged, this,
          &MainWindow::onExperimentListSelectionChanged);
  connect(m_pimpl->ui->actionAbout, &QAction::triggered, this,
          &MainWindow::onShowAboutRequest);
  connect(m_pimpl->ui->actionDuplicate_Experiment, &QAction::triggered, this,
          &MainWindow::onDuplicateExperimentRequest);
  connect(m_pimpl->ui->actionSettings, &QAction::triggered, this,
          &MainWindow::onConfigureApplicationSettingsRequest);
  connect(m_pimpl->ui->actionHelp, &QAction::triggered, [=]() {
    QString link = tr("https://syscall.org/doku.php/teamcamsmanager");
    QDesktopServices::openUrl(QUrl(link));
  });
  m_pimpl->ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(m_pimpl->ui->tableView, &QTableView::customContextMenuRequested, this,
          &MainWindow::onShowExperimentListContextMenuRequest);

  PersistentSettings settings;
  auto experiments = settings.value("experiments", QJsonDocument{});
  m_pimpl->m_experiment_list->fromJson(experiments.toJsonDocument());

  connect(m_pimpl->m_experiment_list, &ExperimentList::contentChanged, this,
          &MainWindow::onExperimentListChanged);
  m_pimpl->m_status_label = new QLabel{};
  statusBar()->addWidget(m_pimpl->m_status_label, 1);
  onExperimentListSelectionChanged();
  setWindowTitle(QString("TeamCAMS Manager"));
  connect(m_pimpl->m_cmanager->transport().get(),
          &MessageTransport::disconnected, this, [this](const QString &reason) {
            qDebug() << "[MainWindow] Reconnecting in 2s";
            QTimer::singleShot(2000, [this] {
              m_pimpl->m_cmanager->transport()->reconnect();
            });
          });
  connect(m_pimpl->m_cmanager->transport().get(),
          &MessageTransport::stateChanged, this,
          [this](QAbstractSocket::SocketState state) { updateStatusbar(); });
  updateStatusbar();
}

MainWindow::~MainWindow() {
  PersistentSettings settings;
  settings.setValue("experiments", m_pimpl->m_experiment_list->toJson());
  delete m_pimpl->ui;
  delete m_pimpl->m_experiment_list;
}

void MainWindow::onImportExperimentListRequest() {
  QMessageBox *msgBox = new QMessageBox(this);
  msgBox->setIcon(QMessageBox::Question);
  msgBox->setWindowTitle(tr("Confirm Import"));
  msgBox->setText(
      tr("Importing an experiment list will terminate and erase all current "
         "experiments, including logs. Are you sure you want to proceed?"));
  msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox->setDefaultButton(QMessageBox::No);
  msgBox->setAttribute(Qt::WA_DeleteOnClose);

  QObject::connect(msgBox, &QMessageBox::finished, this, [this](int result) {
    if (result != QMessageBox::Yes) {
      return;
    }

    QFileDialog::getOpenFileContent(
        "TeamCAMS Experiment List (*.tcams)",
        [this](const QString &fileName, const QByteArray &fileContent) {
          if (fileContent.isEmpty()) {
            QMessageBox *errBox = new QMessageBox(this);
            errBox->setIcon(QMessageBox::Critical);
            errBox->setWindowTitle(tr("Error"));
            errBox->setText(tr("The selected file is empty!"));
            errBox->setAttribute(Qt::WA_DeleteOnClose);
            errBox->open();
            return;
          }

          QJsonParseError parseError;
          QJsonDocument jsonDoc =
              QJsonDocument::fromJson(fileContent, &parseError);

          if (parseError.error == QJsonParseError::NoError) {
            m_pimpl->m_experiment_list->cleanup();
            m_pimpl->m_experiment_list->fromJson(jsonDoc);
            onExperimentListChanged();
            QMessageBox *msgBox = new QMessageBox(nullptr);
            msgBox->setIcon(QMessageBox::Information);
            msgBox->setWindowTitle(tr("Success"));
            msgBox->setText(tr("Experiment list imported successfully!"));
            msgBox->setStandardButtons(QMessageBox::Ok);
            msgBox->setAttribute(Qt::WA_DeleteOnClose);
            msgBox->open();
          } else {
            QMessageBox *errBox = new QMessageBox(this);
            errBox->setIcon(QMessageBox::Critical);
            errBox->setWindowTitle(tr("Error"));
            errBox->setText(tr("Failed to parse experiment list: ") +
                            parseError.errorString());
            errBox->setAttribute(Qt::WA_DeleteOnClose);
            errBox->open();
          }
        });
  });

  msgBox->open();
}

void MainWindow::onExportExperimentListRequest() {
  QMessageBox *msgBox = new QMessageBox(this);
  msgBox->setIcon(QMessageBox::Warning);
  msgBox->setWindowTitle(tr("Important Notice"));
  msgBox->setText(
      tr("Please note that logs are not included in the exported file."));
  msgBox->setStandardButtons(QMessageBox::Ok);
  msgBox->setAttribute(Qt::WA_DeleteOnClose);
  QByteArray jsonData =
      m_pimpl->m_experiment_list->toJson().toJson(QJsonDocument::Indented);

  QObject::connect(msgBox, &QMessageBox::finished, this, [jsonData]() {
    QFileDialog::saveFileContent(jsonData, "experiments.tcams");
  });
  msgBox->open();
}

void MainWindow::toggleMainWindow() { setVisible(!isVisible()); }

void MainWindow::quit() {
  for (auto i{0u}; i < m_pimpl->m_experiment_list->count(); ++i) {
    if (m_pimpl->m_experiment_list->getExperiment(i)
            ->executionManager()
            ->state() != ExecutionManager::ExecutionState::UNINITIALIZED) {
      QMessageBox *msgBox = new QMessageBox(this);
      msgBox->setIcon(QMessageBox::Question);
      msgBox->setWindowTitle(tr("Confirm Quitting"));
      msgBox->setText(
          tr("One or more experiments are still running. Do you really want to "
             "quit? (All users will be disconnected)"));
      msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
      msgBox->setAttribute(Qt::WA_DeleteOnClose);

      connect(msgBox, &QMessageBox::finished, this, [this](int result) {
        if (result == QMessageBox::Yes) {
          for (auto i{0u}; i < m_pimpl->m_experiment_list->count(); ++i) {
            auto experiment{m_pimpl->m_experiment_list->getExperiment(i)};
            experiment->operatorsManager()->disconnectAll();
            experiment->executionManager()->reset();
          }
          PersistentSettings settings;
          settings.remove("instanceHeartbeat");
          QApplication::quit();
        } else {
          return;
        }
      });

      msgBox->open();
    }
  }
  PersistentSettings settings;
  settings.remove("instanceHeartbeat");
  QApplication::quit();
}

void MainWindow::onNewExperimentRequest() {
  NewExperimentDialog *d{
      new NewExperimentDialog{m_pimpl->m_experiment_list, this}};
  d->setModal(true);
  d->setAttribute(Qt::WA_DeleteOnClose);
  connect(d, &QDialog::accepted, [this, d]() {
    m_pimpl->m_experiment_list->addExperiment(d->experimentName());
    if (d->addDefaultUser()) {
      auto exp{m_pimpl->m_experiment_list->getExperiment(d->experimentName())};
      exp->credentialManager()->add(
          "user0", "alias0", QUuid::createUuid().toString(QUuid::WithoutBraces),
          AccessCard::AccessType::FULL_VIEW);
    }
    d->hide();
    d->deleteLater();
  });
  connect(d, &QDialog::rejected, [this, d]() {
    d->hide();
    d->deleteLater();
  });
  d->show();
}

void MainWindow::onRenameExperimentRequest() {
  if (m_pimpl->ui->tableView->selectionModel()->selectedRows().size() == 0)
    return;
  int idx{m_pimpl->ui->tableView->selectionModel()->selectedRows().at(0).row()};
  Experiment *exp{m_pimpl->m_experiment_list->getExperiment(idx)};
  if (exp->executionManager()->state() ==
      ExecutionManager::ExecutionState::UNINITIALIZED) {
    QMessageBox *msgBox = new QMessageBox(nullptr);
    msgBox->setIcon(QMessageBox::Question);
    msgBox->setWindowTitle(tr("Confirm Rename"));
    msgBox->setText(tr("If you rename this experiment you will lose all logs. "
                       "Are you sure you "
                       "want to proceed?"));
    msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox->setAttribute(Qt::WA_DeleteOnClose);

    connect(msgBox, &QMessageBox::finished, this, [exp, this](int result) {
      if (result == QMessageBox::Yes) {
        NewExperimentDialog *d{
            new NewExperimentDialog{m_pimpl->m_experiment_list, this, exp}};
        d->setModal(true);
        d->setAttribute(Qt::WA_DeleteOnClose);
        connect(d, &QDialog::accepted, [this, d] {
          if (d->experimentName() != d->experiment()->name()) {
            QDir path(QDir::currentPath());
            QStringList logfiles =
                path.entryList({d->experiment()->name() + "_*.csv"});
            for (auto &f : logfiles) {
              QFile::remove(path.absoluteFilePath(f));
            }
            d->experiment()->name(d->experimentName());
            m_pimpl->m_experiment_list->dataChanged(
                m_pimpl->ui->tableView->selectionModel()->selectedRows().at(0),
                m_pimpl->ui->tableView->selectionModel()->selectedRows().at(0));
          }
          d->hide();
          d->deleteLater();
        });
        connect(d, &QDialog::rejected, [this, d]() {
          d->hide();
          d->deleteLater();
        });
        d->show();
      }
    });

    msgBox->open();
  }
}

void MainWindow::closeEvent(QCloseEvent *event) {
  event->ignore();
  toggleMainWindow();
}

void MainWindow::onExperimentListChanged() {
  PersistentSettings settings;
  settings.setValue("experiments", m_pimpl->m_experiment_list->toJson());
}

void MainWindow::onConfigureExperimentRequest() {
  if (m_pimpl->ui->tableView->selectionModel()->selectedRows().size() == 0)
    return;
  QModelIndex idx{
      m_pimpl->ui->tableView->selectionModel()->selectedRows().at(0)};
  Experiment *e{m_pimpl->m_experiment_list->getExperiment(idx.row())};
  auto *ced = new ConfigureExperimentDialog(e, this);
  ced->setModal(true);
  connect(ced, &QDialog::finished, this, [idx, ced, this](int result) {
    emit m_pimpl->m_experiment_list->dataChanged(idx, idx);
    ced->deleteLater();
  });
  ced->open();
}

void MainWindow::deleteExperimentsHelper(QList<Experiment *> *tbr) {
  if (tbr->isEmpty()) {
    delete tbr;
    return;
  }
  Experiment *exp = tbr->first();
  QMessageBox *msgBox = new QMessageBox(this);
  msgBox->setIcon(QMessageBox::Question);
  msgBox->setWindowTitle(tr("Confirm delete"));
  msgBox->setText(
      tr("Do you want to delete experiment '%1' and all the associated logs? "
         "All users will be disconnected.")
          .arg(exp->name()));
  msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox->setAttribute(Qt::WA_DeleteOnClose);

  connect(msgBox, &QMessageBox::finished, this, [tbr, this, exp](int result) {
    if (result == QMessageBox::Yes) {
      exp->cleanup();
      m_pimpl->m_experiment_list->removeExperiment(exp->name());
    }
    tbr->removeFirst();
    deleteExperimentsHelper(tbr);
  });
  msgBox->open();
}

void MainWindow::onDeleteExperimentRequest() {
  if (m_pimpl->ui->tableView->selectionModel()->selectedRows().size() == 0)
    return;
  QList<Experiment *> *tbr{new QList<Experiment *>{}};
  for (const auto &selection :
       m_pimpl->ui->tableView->selectionModel()->selectedRows()) {
    if (!selection.isValid())
      continue;
    auto idx{selection.row()};
    Experiment *exp{m_pimpl->m_experiment_list->getExperiment(idx)};
    tbr->push_back(exp);
  }
  deleteExperimentsHelper(tbr);
}

void MainWindow::onShowExperimentListContextMenuRequest(QPoint pos) {
  auto index{m_pimpl->ui->tableView->indexAt(pos)};
  if (!index.isValid()) {
    auto menu{new QMenu{this}};
    menu->addAction(m_pimpl->ui->actionNew_Experiment);
    menu->popup(m_pimpl->ui->tableView->viewport()->mapToGlobal(pos));
  } else {
    if (m_pimpl->ui->tableView->selectionModel()->selectedRows().size() > 0) {
      auto enableAction{new QAction{this}};
      enableAction->setText(tr("Available to users"));
      enableAction->setCheckable(true);
      bool checked{true};
      for (const auto &s :
           m_pimpl->ui->tableView->selectionModel()->selectedRows()) {
        if (!m_pimpl->m_experiment_list->getExperiment(s.row())->enabled()) {
          checked = false;
          break;
        }
      }
      enableAction->setChecked(checked);
      connect(enableAction, &QAction::triggered, this, [checked, this]() {
        for (const auto &s :
             m_pimpl->ui->tableView->selectionModel()->selectedRows()) {
          m_pimpl->m_experiment_list->getExperiment(s.row())->enabled(!checked);
        }
      });
      auto menu{new QMenu{this}};
      menu->addAction(m_pimpl->ui->actionEdit_Experiment);
      menu->addSeparator();
      menu->addAction(m_pimpl->ui->actionRename_Experiment);
      menu->addAction(m_pimpl->ui->actionDuplicate_Experiment);
      menu->addAction(m_pimpl->ui->actionDelete_Experiment);
      menu->addSeparator();
      menu->addAction(m_pimpl->ui->actionStart);
      menu->addAction(m_pimpl->ui->actionStop);
      menu->addSeparator();
      menu->addAction(enableAction);
      menu->popup(m_pimpl->ui->tableView->viewport()->mapToGlobal(pos));
    } else {
      m_pimpl->ui->tableView->clearSelection();
      m_pimpl->ui->tableView->selectRow(index.row());
      auto enableAction{new QAction{this}};
      enableAction->setText(tr("Available to users"));
      enableAction->setCheckable(true);
      enableAction->setChecked(
          m_pimpl->m_experiment_list->getExperiment(index.row())->enabled());
      connect(enableAction, &QAction::triggered, this,
              [enableAction, index, this]() {
                m_pimpl->m_experiment_list->getExperiment(index.row())
                    ->enabled(enableAction->isChecked());
              });
      auto menu{new QMenu{this}};
      menu->addAction(m_pimpl->ui->actionNew_Experiment);
      menu->addSeparator();
      menu->addAction(m_pimpl->ui->actionDuplicate_Experiment);
      menu->addAction(m_pimpl->ui->actionRename_Experiment);
      menu->addAction(m_pimpl->ui->actionDelete_Experiment);
      menu->addSeparator();
      menu->addAction(m_pimpl->ui->actionEdit_Experiment);
      menu->addSeparator();
      menu->addAction(m_pimpl->ui->actionStart);
      menu->addAction(m_pimpl->ui->actionStop);
      menu->addSeparator();
      menu->addAction(enableAction);
      menu->popup(m_pimpl->ui->tableView->viewport()->mapToGlobal(pos));
    }
  }
}

void MainWindow::onExperimentListSelectionChanged() {
  if (m_pimpl->ui->tableView->selectionModel()->selectedRows().size() == 0) {
    m_pimpl->ui->actionEdit_Experiment->setEnabled(false);
    m_pimpl->ui->actionDelete_Experiment->setEnabled(false);
    m_pimpl->ui->actionRename_Experiment->setEnabled(false);
    m_pimpl->ui->actionDuplicate_Experiment->setEnabled(false);
    m_pimpl->ui->actionStart->setEnabled(false);
    m_pimpl->ui->actionStop->setEnabled(false);
    m_pimpl->ui->actionStart->setIcon(
        QPixmap(":/images/media-playback-start.png"));
  } else if (m_pimpl->ui->tableView->selectionModel()->selectedRows().size() >
             1) {
    m_pimpl->ui->actionEdit_Experiment->setEnabled(false);
    m_pimpl->ui->actionDelete_Experiment->setEnabled(true);
    m_pimpl->ui->actionRename_Experiment->setEnabled(false);
    m_pimpl->ui->actionDuplicate_Experiment->setEnabled(false);
    m_pimpl->ui->actionStop->setEnabled(false);
    m_pimpl->ui->actionStart->setEnabled(true);
    for (const auto &s :
         m_pimpl->ui->tableView->selectionModel()->selectedRows()) {
      if (m_pimpl->m_experiment_list->getExperiment(s.row())
              ->executionManager()
              ->state() == ExecutionManager::ExecutionState::RUNNING) {
        m_pimpl->ui->actionStop->setEnabled(true);
        break;
      }
    }
    bool allPaused{true};
    for (const auto &s :
         m_pimpl->ui->tableView->selectionModel()->selectedRows()) {
      if (m_pimpl->m_experiment_list->getExperiment(s.row())
              ->executionManager()
              ->state() != ExecutionManager::ExecutionState::PAUSED) {
        m_pimpl->ui->actionStop->setEnabled(true);
        allPaused = false;
        break;
      }
    }
    bool allUninitialized{true};
    for (const auto &s :
         m_pimpl->ui->tableView->selectionModel()->selectedRows()) {
      if (m_pimpl->m_experiment_list->getExperiment(s.row())
              ->executionManager()
              ->state() != ExecutionManager::ExecutionState::UNINITIALIZED) {
        m_pimpl->ui->actionStop->setEnabled(true);
        allUninitialized = false;
        break;
      }
    }
    bool allRunning{true};
    for (const auto &s :
         m_pimpl->ui->tableView->selectionModel()->selectedRows()) {
      if (m_pimpl->m_experiment_list->getExperiment(s.row())
              ->executionManager()
              ->state() != ExecutionManager::ExecutionState::RUNNING) {
        m_pimpl->ui->actionStop->setEnabled(true);
        allRunning = false;
        break;
      }
    }
    if (allUninitialized) {
      m_pimpl->ui->actionStart->setIcon(
          QPixmap(":/images/media-playback-start.png"));
      m_pimpl->ui->actionStart->setText(tr("Start"));
      m_pimpl->ui->actionStart->setToolTip(tr("Start selected experiments"));
    } else if (allRunning) {
      m_pimpl->ui->actionStart->setIcon(
          QPixmap(":/images/media-playback-pause.png"));
      m_pimpl->ui->actionStart->setText(tr("Pause"));
      m_pimpl->ui->actionStart->setToolTip(tr("Pause selected experiment"));
    } else if (allPaused) {
      m_pimpl->ui->actionStart->setIcon(
          QPixmap(":/images/media-playback-start.png"));
      m_pimpl->ui->actionStart->setText(tr("Resume"));
      m_pimpl->ui->actionStart->setToolTip(tr("Resume selected experiment"));
    } else {
      m_pimpl->ui->actionStart->setIcon(
          QPixmap(":/images/media-playback-toggle.png"));
      m_pimpl->ui->actionStart->setText(tr("Toggle"));
      m_pimpl->ui->actionStart->setToolTip(
          tr("Toggle the execution state of selected experiments"));
    }
  } else {
    QModelIndex idx{
        m_pimpl->ui->tableView->selectionModel()->selectedRows().first()};
    if (!idx.isValid())
      return;
    Experiment *e{m_pimpl->m_experiment_list->getExperiment(idx.row())};
    m_pimpl->ui->actionEdit_Experiment->setEnabled(true);
    m_pimpl->ui->actionDuplicate_Experiment->setEnabled(true);
    m_pimpl->ui->actionDelete_Experiment->setEnabled(
        e->executionManager()->state() ==
        ExecutionManager::ExecutionState::UNINITIALIZED);
    m_pimpl->ui->actionRename_Experiment->setEnabled(
        e->executionManager()->state() ==
        ExecutionManager::ExecutionState::UNINITIALIZED);
    m_pimpl->ui->actionStart->setEnabled(true);
    m_pimpl->ui->actionStop->setEnabled(
        e->executionManager()->state() !=
        ExecutionManager::ExecutionState::UNINITIALIZED);
    if (e->executionManager()->state() ==
        ExecutionManager::ExecutionState::RUNNING) {
      m_pimpl->ui->actionStart->setIcon(
          QPixmap(":/images/media-playback-pause.png"));
      m_pimpl->ui->actionStart->setText(tr("Pause"));
      m_pimpl->ui->actionStart->setToolTip(tr("Pause selected experiment"));
    } else if (e->executionManager()->state() ==
               ExecutionManager::ExecutionState::PAUSED) {
      m_pimpl->ui->actionStart->setIcon(
          QPixmap(":/images/media-playback-start.png"));
      m_pimpl->ui->actionStart->setText(tr("Resume"));
      m_pimpl->ui->actionStart->setToolTip(tr("Resume selected experiment"));
    } else {
      m_pimpl->ui->actionStart->setIcon(
          QPixmap(":/images/media-playback-start.png"));
      m_pimpl->ui->actionStart->setText(tr("Start"));
      m_pimpl->ui->actionStart->setToolTip(tr("Start selected experiment"));
    }
  }
}

void MainWindow::onStartExperimentRequest() {
  for (const auto &idx :
       m_pimpl->ui->tableView->selectionModel()->selectedRows()) {
    if (!idx.isValid())
      return;
    Experiment *e{m_pimpl->m_experiment_list->getExperiment(idx.row())};
    e->executionManager()->toggle();
  }
}

void MainWindow::onStopExperimentRequest() {
  for (const auto &idx :
       m_pimpl->ui->tableView->selectionModel()->selectedRows()) {
    if (!idx.isValid())
      return;
    Experiment *e{m_pimpl->m_experiment_list->getExperiment(idx.row())};
    e->operatorsManager()->disconnectAll();
    e->executionManager()->reset();
  }
}

void MainWindow::onDuplicateExperimentRequest() {
  if (m_pimpl->ui->tableView->selectionModel()->selectedRows().size() == 0)
    return;
  int idx{m_pimpl->ui->tableView->selectionModel()->selectedRows().at(0).row()};
  Experiment *exp{m_pimpl->m_experiment_list->getExperiment(idx)};
  auto ned = new NewExperimentDialog(m_pimpl->m_experiment_list, this);
  ned->setModal(true);
  connect(ned, &QDialog::finished, this, [this, exp, ned](int result) {
    if (result == QDialog::Accepted && ned->experimentName() != exp->name()) {
      m_pimpl->m_experiment_list->addExperiment(ned->experimentName());
      Experiment *clone{
          m_pimpl->m_experiment_list->getExperiment(ned->experimentName())};
      QJsonObject data;
      exp->save(data);
      clone->load(data);
      m_pimpl->m_experiment_list->dataChanged(QModelIndex(), QModelIndex());
    }
    ned->deleteLater();
  });
  ned->open();
}

void MainWindow::onShowAboutRequest() {
  QMessageBox *msgBox = new QMessageBox(this);
  msgBox->setIcon(QMessageBox::NoIcon);
  msgBox->setWindowTitle(tr("About TeamCAMS"));
  msgBox->setText(
      tr("<h3>TeamCAMS Experiment Manager 2026.08</h3>"

         "<p>"
         "Copyright &copy; 2015-2026 "
         "<b>Amos Brocco</b> / University of Fribourg (Switzerland)."
         "</p>"

         "<p>"
         "This software is free software released under the terms of the "
         "<b>GNU General Public License version 3 (GPLv3)</b>. "
         "You are free to use, study, modify and redistribute it under the "
         "conditions of that license."
         "</p>"

         "<p>"
         "<b>Disclaimer:</b> This program is provided <i>AS IS</i>, without "
         "any express or implied warranty, including but not limited to the "
         "warranties of merchantability, fitness for a particular purpose, "
         "and non-infringement."
         "</p>"

         "<p><b>Third-party components</b></p>"

         "<ul>"
         "<li><b>Qt Toolkit</b> &mdash; Copyright The Qt Company "
         "(GNU LGPL license).</li>"

         "<li><b>Qt QR Code Generator Library</b> &mdash; "
         "Copyright &copy; 2023 Alex Spataru "
         "(MIT License).</li>"

         "<li><b>Monocypher</b> &mdash; "
         "Copyright &copy; 2017-2023 Loup Vaillant, "
         "Copyright &copy; 2017-2019 Michael Savage, "
         "Copyright &copy; 2017-2023 Fabio Scotoni.</li>"
         "</ul>"));
  msgBox->setStandardButtons(QMessageBox::Ok);
  msgBox->setAttribute(Qt::WA_DeleteOnClose);
  msgBox->open();
}

#ifdef QT_TEST_ENABLED
#define ADD_MY_TEST(tklass)                                                    \
  tklass instance_##tklass;                                                    \
  test_instances.push_back(static_cast<QObject *>(&instance_##tklass));        \
  {                                                                            \
    const QMetaObject *metaObject = instance_##tklass.metaObject();            \
    QStringList methods;                                                       \
    int testCount{0};                                                          \
    for (int i = metaObject->methodOffset(); i < metaObject->methodCount();    \
         ++i) {                                                                \
      if (metaObject->method(i).methodType() == QMetaMethod::Slot &&           \
          metaObject->method(i).access() == QMetaMethod::Access::Private) {    \
        ++total;                                                               \
        ++testCount;                                                           \
      }                                                                        \
    }                                                                          \
    test_count[static_cast<QObject *>(&instance_##tklass)] = testCount;        \
  }

#define RUN_MY_TEST(it)                                                        \
  {                                                                            \
    result = QTest::qExec(it);                                                 \
    if (result)                                                                \
      AsyncMessageBoxFactory::critical(                                        \
          QApplication::activeWindow(), MainWindow::tr("Test failed"),         \
          QString{it->metaObject()->className()} +                             \
              MainWindow::tr(" test failed"),                                  \
          QMessageBox::Ok);                                                    \
    if (progress.wasCanceled())                                                \
      return;                                                                  \
    testsDone += test_count[it];                                               \
    progress.setValue(testsDone);                                              \
  }

#define RUN_ALL_TEST                                                           \
  for (auto &k : test_instances) {                                             \
    RUN_MY_TEST(k)                                                             \
  }

#define SETUP_MY_TEST                                                          \
  int result{0};                                                               \
  int total{0};                                                                \
  int testsDone{0};                                                            \
  QProgressDialog progress(MainWindow::tr("Running self tests..."),            \
                           MainWindow::tr("Cancel"), 0, 61, this);             \
  progress.setWindowModality(Qt::WindowModal);                                 \
  progress.setWindowTitle(this->windowTitle());                                \
  progress.show();                                                             \
  QList<QObject *> test_instances;                                             \
  QMap<QObject *, int> test_count;

#define FINISH_MY_TEST                                                         \
  progress.close();                                                            \
  if (total == testsDone) {                                                    \
    QMessageBox::information(this, MainWindow::tr("Done"),                     \
                             MainWindow::tr("All test completed"),             \
                             QMessageBox::Ok);                                 \
  } else {                                                                     \
    QMessageBox::information(                                                  \
        this, MainWindow::tr("Ooops!"),                                        \
        QString{MainWindow::tr("Only %1 of %2 tests passed")}                  \
            .arg(testsDone)                                                    \
            .arg(total),                                                       \
        QMessageBox::Ok);                                                      \
  }

void MainWindow::onPerformTestsRequest() {
  SETUP_MY_TEST

  ADD_MY_TEST(TestRegistry)
  ADD_MY_TEST(TestCabin)
  ADD_MY_TEST(TestCrew)
  ADD_MY_TEST(TestControl)
  ADD_MY_TEST(TestCooler)
  ADD_MY_TEST(TestDehumidifier)
  ADD_MY_TEST(TestExperiment)
  ADD_MY_TEST(TestExperimentList)
  ADD_MY_TEST(TestHeater)
  ADD_MY_TEST(TestMixer)
  ADD_MY_TEST(TestPipe)
  ADD_MY_TEST(TestScheduler)
  ADD_MY_TEST(TestScrubber)
  ADD_MY_TEST(TestSensor)
  ADD_MY_TEST(TestSimulator)
  ADD_MY_TEST(TestTank)
  ADD_MY_TEST(TestTriggers)
  ADD_MY_TEST(TestUserList)
  ADD_MY_TEST(TestValve)
  ADD_MY_TEST(TestVent)
  ADD_MY_TEST(TestXMLConfigurationParser)

  RUN_ALL_TEST

  auto testscripts{QDir(":/testscripts/testscripts").entryList()};
  progress.setMaximum(testscripts.size());
  progress.setValue(0);
  foreach (const QString &script, testscripts) {
    ++total;
    Experiment e{script, nullptr, this};
    e.enabled(true);
    e.script(":/testscripts/testscripts/" + script);
    progress.setLabelText(tr("Testing script %1").arg(script));
    e.executionManager()->resume();
    while (e.executionManager()->state() ==
           ExecutionManager::ExecutionState::RUNNING) {
      if (progress.wasCanceled())
        return;
      QCoreApplication::instance()->processEvents();
    }
    if (e.scriptingManager()->hasErrorCondition()) {
      AsyncMessageBoxFactory::critical(
          QApplication::activeWindow(), tr("Test failed"),
          tr("Test script %1 failed").arg(script), QMessageBox::Ok);
    } else {
      ++testsDone;
    }
    progress.setValue(progress.value() + 1);
  }

  FINISH_MY_TEST
}
#endif

void MainWindow::onConfigureApplicationSettingsRequest() {
  auto *acd = new AppConfigurationDialog{this};
  acd->setModal(true);
  connect(acd, &QDialog::finished, this,
          [acd](int result) { acd->deleteLater(); });
  acd->open();
}

void MainWindow::setupTrayIcon() {
  m_pimpl->m_tray_icon_menu = new QMenu(this);
  m_pimpl->m_tray_icon_menu->addAction(
      m_pimpl->ui->actionOpen_experiment__manager);
  m_pimpl->m_tray_icon_menu->addSeparator();
  m_pimpl->m_tray_icon_menu->addAction(m_pimpl->ui->actionQuit);
  m_pimpl->m_tray_icon = new QSystemTrayIcon(this);
  m_pimpl->m_tray_icon->setIcon(QPixmap(":/images/icon.png"));
  m_pimpl->m_tray_icon->setContextMenu(m_pimpl->m_tray_icon_menu);
  m_pimpl->m_tray_icon->setVisible(true);
}

void MainWindow::updateStatusbar() {
  auto state = m_pimpl->m_cmanager->transport()->state();
  switch (state) {
  case QAbstractSocket::UnconnectedState:
    m_pimpl->m_status_label->setText({tr("Not connected")});
    break;
  case QAbstractSocket::HostLookupState: {
    m_pimpl->m_status_label->setText(tr("Looking up broker..."));
    break;
  }
  case QAbstractSocket::ConnectingState: {
    m_pimpl->m_status_label->setText(tr("Connecting to broker..."));
    break;
  }
  case QAbstractSocket::ConnectedState:
    m_pimpl->m_status_label->setText({tr("Connected")});
    break;
  case QAbstractSocket::BoundState:
    m_pimpl->m_status_label->setText({tr("Bound")});
    break;
  case QAbstractSocket::ListeningState:
    m_pimpl->m_status_label->setText({tr("Listening")});
    break;
  case QAbstractSocket::ClosingState:
    m_pimpl->m_status_label->setText({tr("Closing...")});
    break;
  }
}
