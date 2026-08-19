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
#ifndef CONFIGUREEXPERIMENTDIALOG_H
#define CONFIGUREEXPERIMENTDIALOG_H

#include "experiment/experiment.h"
#include <QDialog>
#include <QGraphicsSceneDragDropEvent>
#include <QItemDelegate>

namespace Ui {
class ConfigureExperimentDialog;
}

class ConfigureExperimentDialog : public QDialog {
  Q_OBJECT
  Q_DISABLE_COPY(ConfigureExperimentDialog)
public:
  explicit ConfigureExperimentDialog(Experiment *exp,
                                     QWidget *parent = nullptr);
  ~ConfigureExperimentDialog() override;

public slots:
  void onDescriptionTextChanged();
  void onChooseScriptButton();
  void onValidateScriptButton();
  void onScriptChanged();
  void onStateChanged();
  void newUser();
  void removeUser();
  void copyConnectionUrl() const;
  void userTableContextMenu(QPoint pos);
  void filterTableContextMenu(QPoint pos);
  void onUpdateState();
  void onUpdateTimestamp();
  void newFilterRule();
  void toggleFilterEditTriggers();
  void removeFilterRule();
  void onUpdateAvailability();
  void onSimSpeedChanged(double factor);
  void dropEvent(QDropEvent *event) override;
  void dragEnterEvent(QDragEnterEvent *event) override;
  void updateUserDetails();
  void clearUserDetails();
  QUrl getConnectionUrl(const UserCardPtr &card) const;
  void refreshLogTable();
  void exportSelectedLog();
  void discardSelectedLog();

private:
  QStringList m_logfiles;
  Experiment *m_experiment;
  Ui::ConfigureExperimentDialog *ui;
};

class NonEditingDelegate : public QItemDelegate {
  Q_OBJECT

public:
  explicit NonEditingDelegate(QObject *parent = nullptr)
      : QItemDelegate(parent) {}

protected:
  bool editorEvent(QEvent *, QAbstractItemModel *, const QStyleOptionViewItem &,
                   const QModelIndex &) override {
    return false;
  }
  QWidget *createEditor(QWidget *, const QStyleOptionViewItem &,
                        const QModelIndex &) const override {
    return nullptr;
  }
};

#endif // CONFIGUREEXPERIMENTDIALOG_H
