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

#include "gui/asyncmessageboxfactory.h"
#include "gui/mainwindow.h"
#include "models/experimentlist.h"
#include "network/mqtt/mqttcommunicationmanager.h"
#include "network/mqtt/mqtttransport.h"
#include "utils/persistentsettings.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QSslCertificate>
#include <QSslConfiguration>
#include <QSslKey>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QThread>
#include <QTimer>
#include <QTranslator>
#ifdef Q_OS_WASM
#include <QUrlQuery>
#include <emscripten/val.h>
#endif

int main(int argc, char *argv[]) {
  QApplication::setStyle(QStyleFactory::create("Fusion"));
  QApplication a(argc, argv);
  QGuiApplication::setOrganizationName("University of Fribourg");
  QGuiApplication::setOrganizationDomain("unifr.ch");
  QGuiApplication::setApplicationDisplayName("TeamCAMS Manager");

  qRegisterMetaType<std::shared_ptr<Peer>>("std::shared_ptr<Peer>");

  // Set settings default
  PersistentSettings settings;
  settings.setDefault("language", QLocale::system().name().left(2));
  settings.setDefault("instanceUuid",
                      QUuid::createUuidV7().toString(QUuid::WithoutBraces));
  settings.setDefault("clientUrl",
                      "https://teamcams.syscall.org/teamcams/client/");
  settings.setDefault("brokerHost", "broker.hivemq.com");
  settings.setDefault("clientBrokerUrl", "wss://broker.hivemq.com:8884/mqtt");
  settings.setDefault("brokerPort", 8883);
  settings.setDefault("brokerSecurity", true);

  // Configure language settings
  QTranslator translator;
  const QStringList uiLanguages = QLocale::system().uiLanguages();
  QString preferredLanguage = settings.valueOrUpdated("language").toString();
  settings.setValue("language", preferredLanguage);
  const QString baseName = "teamcamsmanager_" + preferredLanguage;
  if (translator.load(":/i18n/" + baseName)) {
    a.installTranslator(&translator);
  }

  // Set heartbeat to settings to prevent multiple instances
  if (settings.contains("instanceHeartbeat")) {
    auto heartbeat = settings.value("instanceHeartbeat").toDateTime();
    auto now = QDateTime::currentDateTime();
    if (heartbeat.secsTo(now) < 10) {
      auto multipleInstancesMessage = QCoreApplication::translate(
          "main",
          "Another manager instance is already running.\n\n"
          "Please check your task manager and close any other instances of "
          "this application before trying again. If all instances are closed, "
          "wait 10s and retry.");
      // There is another instance running
#ifdef MANAGERGUI
      AsyncMessageBoxFactory::critical(
          nullptr, QCoreApplication::translate("main", "Critical Error"),
          multipleInstancesMessage, QMessageBox::Ok);
      return a.exec();
#endif
      qCritical() << multipleInstancesMessage;
      return 2;
    }
  }
  settings.setValue("instanceHeartbeat", QDateTime::currentDateTime());
  QTimer *timer = new QTimer();
  QObject::connect(timer, &QTimer::timeout, [&settings]() {
    settings.setValue("instanceHeartbeat", QDateTime::currentDateTime());
  });
  timer->start(5000);

  // Apply pending settings updates
  settings.applyUpdates();

  // Setup broker connection
  auto instanceUuid = settings.value("instanceUuid").toString();
  auto brokerHost = settings.value("brokerHost").toString();
  auto brokerSecurity = settings.value("brokerSecurity").toBool();
  auto brokerPort = settings.value("brokerPort").toUInt();
  auto security_mode =
      brokerSecurity ? MqttSecurityMode::Secure : MqttSecurityMode::Unsecure;

  auto cmanager{std::make_shared<MqttCommunicationManager>(instanceUuid)};
  cmanager->mqttTransport().connectToBroker(
      brokerHost, static_cast<quint16>(brokerPort), security_mode);

  auto experiment_list = new ExperimentList(cmanager);
  auto experiments = settings.value("experiments");
  experiment_list->fromJson(experiments.toJsonDocument());

  // Show main window
  MainWindow w{cmanager, experiment_list};
  w.show();

  return a.exec();
}
