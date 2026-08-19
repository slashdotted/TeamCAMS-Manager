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
#include "gui/newexperimentdialog.h"
#include "ui_newexperimentdialog.h"
#include <QPushButton>

NewExperimentDialog::NewExperimentDialog(ExperimentList *elist, QWidget *parent,
                                         Experiment *e)
    : QDialog(parent), m_experimentlist{elist}, m_experiment{e},
      ui(new Ui::NewExperimentDialog) {
  ui->setupUi(this);
  connect(ui->lineEdit, &QLineEdit::textEdited, this,
          &NewExperimentDialog::onTextEdited);
  ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
  ui->lineEdit->setFocus();
  if (e != nullptr) {
    ui->lineEdit->setText(e->name());
    ui->label_2->setText("");
  }
}

NewExperimentDialog::~NewExperimentDialog() { delete ui; }

QString NewExperimentDialog::experimentName() const {
  return ui->lineEdit->text();
}

bool NewExperimentDialog::addDefaultUser() {
  return ui->addDefaultUserCheckbox->isChecked();
}

Experiment *NewExperimentDialog::experiment() const { return m_experiment; }

void NewExperimentDialog::onTextEdited(QString text) {
  if (text.contains(' ') || text.contains('\t') || text.contains('_') ||
      text.contains(',') || text.contains('\t') || text.contains('\n') ||
      text.contains('<') || text.contains('>') || text.contains('/') ||
      text.contains('\\') || text.contains(':') || text.contains('|') ||
      text.contains('"') || text.contains('?') || text.contains('?') ||
      text.contains('*') || text.contains('%')) {
    ui->label_2->setText(tr("Experiment name contains invalid characters"));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
  } else if (text.trimmed().length() == 0) {
    ui->label_2->setText(tr("Experiment name cannot be empty"));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
  } else if (((m_experiment == nullptr) &&
              (m_experimentlist->hasExperiment(text))) ||
             ((m_experiment != nullptr) && (m_experiment->name() != text) &&
              m_experimentlist->hasExperiment(text))) {
    ui->label_2->setText(tr("Duplicate experiment name"));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
  } else {
    ui->label_2->setText("");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
  }
}
