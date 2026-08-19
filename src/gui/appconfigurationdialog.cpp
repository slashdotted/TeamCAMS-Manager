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
#include "appconfigurationdialog.h"
#include "ui_appconfigurationdialog.h"
#include "utils/persistentsettings.h"
#include <QDir>

AppConfigurationDialog::AppConfigurationDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AppConfigurationDialog) {
  ui->setupUi(this);
  // Populate the dialog
  updateContents();
  updateChangesHighlight();

  connect(ui->buttonBox, &QDialogButtonBox::accepted, this,
          &AppConfigurationDialog::applyChanges);
  connect(ui->buttonBox, &QDialogButtonBox::rejected, this,
          &AppConfigurationDialog::close);
  connect(ui->managerUuid, &QLineEdit::editingFinished, this,
          &AppConfigurationDialog::updateChangesHighlight);
  connect(ui->clientUrl, &QLineEdit::editingFinished, this,
          &AppConfigurationDialog::updateChangesHighlight);
  connect(ui->brokerHost, &QLineEdit::editingFinished, this,
          &AppConfigurationDialog::updateChangesHighlight);
  connect(ui->brokerPort, &QSpinBox::editingFinished, this,
          &AppConfigurationDialog::updateChangesHighlight);
  connect(ui->secureBrokerConnection, &QCheckBox::clicked, this,
          &AppConfigurationDialog::updateChangesHighlight);
  connect(ui->clientBrokerUrl, &QLineEdit::editingFinished, this,
          &AppConfigurationDialog::updateChangesHighlight);
  connect(ui->languageCombo, &QComboBox::currentTextChanged, this,
          &AppConfigurationDialog::updateChangesHighlight);

  connect(ui->restoreDefaultsButton, &QPushButton::clicked, [this]() {
    PersistentSettings settings;
    ui->managerUuid->setText(
        settings.valueOrUpdated("instanceUuid.default").toString());
    ui->clientUrl->setText(
        settings.valueOrUpdated("clientUrl.default").toString());
    ui->brokerHost->setText(
        settings.valueOrUpdated("brokerHost.default").toString());
    ui->brokerPort->setValue(
        settings.valueOrUpdated("brokerPort.default").toInt());
    ui->secureBrokerConnection->setChecked(
        settings.valueOrUpdated("brokerSecurity.default").toBool());
    ui->clientBrokerUrl->setText(
        settings.valueOrUpdated("clientBrokerUrl.default").toString());

    ui->languageCombo->clear();
    ui->languageCombo->addItem(tr("English (default)"), QString());
    ui->languageCombo->addItem("Italiano", "it");
    ui->languageCombo->addItem("Français", "fr");
    ui->languageCombo->addItem("Deutsch", "de");
    auto language = settings.valueOrUpdated("language.default").toString();
    const int index = ui->languageCombo->findData(language);
    if (index >= 0) {
      ui->languageCombo->setCurrentIndex(index);
    }
    updateChangesHighlight();
  });
  connect(ui->discardChangesButton, &QPushButton::clicked, [this]() {
    PersistentSettings settings;
    ui->managerUuid->setText(settings.value("instanceUuid").toString());
    ui->clientUrl->setText(settings.value("clientUrl").toString());
    ui->brokerHost->setText(settings.value("brokerHost").toString());
    ui->brokerPort->setValue(settings.value("brokerPort").toInt());
    ui->secureBrokerConnection->setChecked(
        settings.value("brokerSecurity").toBool());
    ui->clientBrokerUrl->setText(settings.value("clientBrokerUrl").toString());

    ui->languageCombo->clear();
    ui->languageCombo->addItem(tr("English (default)"), QString());
    ui->languageCombo->addItem("Italiano", "it");
    ui->languageCombo->addItem("Français", "fr");
    ui->languageCombo->addItem("Deutsch", "de");
    auto language = settings.value("language").toString();
    const int index = ui->languageCombo->findData(language);
    if (index >= 0) {
      ui->languageCombo->setCurrentIndex(index);
    }
    updateChangesHighlight();
  });
}

AppConfigurationDialog::~AppConfigurationDialog() { delete ui; }

void AppConfigurationDialog::applyChanges() {
  PersistentSettings settings;
  if (ui->managerUuid->text() != settings.value("instanceUuid").toString()) {
    settings.setUpdatedValue("instanceUuid", ui->managerUuid->text());
  }
  if (ui->clientUrl->text() != settings.value("clientUrl").toString()) {
    settings.setUpdatedValue("clientUrl", ui->clientUrl->text());
  }
  if (ui->clientBrokerUrl->text() !=
      settings.value("clientBrokerUrl").toString()) {
    settings.setUpdatedValue("clientBrokerUrl", ui->clientBrokerUrl->text());
  }
  if (ui->brokerHost->text() != settings.value("brokerHost").toString()) {
    settings.setUpdatedValue("brokerHost", ui->brokerHost->text());
  }
  if (ui->brokerPort->value() != settings.value("brokerPort").toInt()) {
    settings.setUpdatedValue("brokerPort", ui->brokerPort->value());
  }
  if (ui->secureBrokerConnection->isChecked() !=
      settings.value("brokerSecurity").toBool()) {
    settings.setUpdatedValue("brokerSecurity",
                             ui->secureBrokerConnection->isChecked());
  }
  if (ui->languageCombo->currentData() !=
      settings.value("language").toString()) {
    settings.setUpdatedValue("language", ui->languageCombo->currentData());
  }
}

void AppConfigurationDialog::updateChangesHighlight() {
  constexpr auto changedStyle = "color: red;";
  constexpr auto noStyle = "";
  PersistentSettings settings;
  ui->managerUuid->setStyleSheet(
      settings.valueOrUpdated("instanceUuid").toString() !=
              ui->managerUuid->text()
          ? changedStyle
          : noStyle);

  ui->clientUrl->setStyleSheet(
      settings.valueOrUpdated("clientUrl").toString() != ui->clientUrl->text()
          ? changedStyle
          : noStyle);
  ui->clientBrokerUrl->setStyleSheet(
      settings.valueOrUpdated("clientBrokerUrl").toString() !=
              ui->clientBrokerUrl->text()
          ? changedStyle
          : noStyle);
  ui->brokerHost->setStyleSheet(
      settings.valueOrUpdated("brokerHost").toString() != ui->brokerHost->text()
          ? changedStyle
          : noStyle);
  ui->brokerPort->setStyleSheet(
      settings.valueOrUpdated("brokerPort").toUInt() != ui->brokerPort->value()
          ? changedStyle
          : noStyle);
  ui->secureBrokerConnection->setStyleSheet(
      settings.valueOrUpdated("brokerSecurity").toBool() !=
              ui->secureBrokerConnection->isChecked()
          ? changedStyle
          : noStyle);
  ui->languageCombo->setStyleSheet(
      settings.valueOrUpdated("language").toString() !=
              ui->languageCombo->currentData()
          ? changedStyle
          : noStyle);
}

void AppConfigurationDialog::updateContents() {
  PersistentSettings settings;
  ui->managerUuid->setText(settings.valueOrUpdated("instanceUuid").toString());
  ui->clientUrl->setText(settings.valueOrUpdated("clientUrl").toString());
  ui->brokerHost->setText(settings.valueOrUpdated("brokerHost").toString());
  ui->brokerPort->setValue(settings.valueOrUpdated("brokerPort").toInt());
  ui->secureBrokerConnection->setChecked(
      settings.valueOrUpdated("brokerSecurity").toBool());
  ui->clientBrokerUrl->setText(
      settings.valueOrUpdated("clientBrokerUrl").toString());

  ui->languageCombo->clear();
  ui->languageCombo->addItem(tr("English (default)"), QString());
  ui->languageCombo->addItem("Italiano", "it");
  ui->languageCombo->addItem("Français", "fr");
  ui->languageCombo->addItem("Deutsch", "de");
  auto language = settings.valueOrUpdated("language").toString();
  const int index = ui->languageCombo->findData(language);
  if (index >= 0) {
    ui->languageCombo->setCurrentIndex(index);
  }
}
