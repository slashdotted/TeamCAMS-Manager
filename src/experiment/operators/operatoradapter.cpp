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
#include "operatoradapter.h"
#include "experiment/exceptions.h"
#include "experiment/experiment.h"
#include <QJsonObject>
#ifdef MANAGERGUI
#include "gui/asyncmessageboxfactory.h"
#include <QApplication>
#include <QMessageBox>
#endif
#include <QJsonDocument>

OperatorAdapter::OperatorAdapter(Experiment *exp) : QObject{exp} {
  // React to incoming triggers from clients
  connect(exp->operatorsManager(), &OperatorsManager::triggerReceived,
          [this, exp](const QString &key, const QJsonValue &payload,
                      const Operator &cc) {
            if (exp->executionManager()->state() ==
                ExecutionManager::ExecutionState::UNINITIALIZED)
              return;
            if (!payload.isObject())
              return;
            const auto &username{cc.card()->username()};
            QJsonObject data;
            data["key"] = key;
            data["value"] = payload.toObject();
            emit log("trigger", username,
                     QJsonDocument{data}.toJson(QJsonDocument::Compact));
            QVariantMap bindings{payload.toObject().toVariantMap()};
            bindings["_sender"] = username;
            bindings["_alias"] = cc.card()->alias();
            exp->triggers()->processTriggers(key, bindings);
          });

  // React to incoming updates from clients
  connect(
      exp->operatorsManager(), &OperatorsManager::updateReceived,
      [exp](const QString &key, const QJsonValue &payload, const Operator &cc) {
        if (exp->executionManager()->state() ==
            ExecutionManager::ExecutionState::UNINITIALIZED) {
          return;
        }
        // TODO: Evaluate if it's necessary to prevent updates in paused mode
        // By preventing updates we have a problem with tutorials that expect an
        // update to continue
        //        if (exp->executionManager()->state() ==
        //        ExecutionManager::ExecutionState::PAUSED) {
        //            exp->simulator()->registry()->touch(key);
        //            return;
        //        }
        try {
          exp->simulator()->registry()->set(key, payload, false,
                                            cc.card()->username());
        } catch (const UndefinedPropertyException &) {
#ifdef MANAGERGUI
          AsyncMessageBoxFactory::critical(
              QApplication::activeWindow(), tr("Runtime Exception"),
              tr("<b>Undefined property</b><br><br>%1 <-> %2")
                  .arg(key, payload.toString()),
              QMessageBox::Ok);
#endif
          exp->executionManager()->reset();
        } catch (const InvalidPropertyNameException &) {
#ifdef MANAGERGUI
          AsyncMessageBoxFactory::critical(
              QApplication::activeWindow(), tr("Runtime Exception"),
              tr("<b>Invalid property name</b><br><br>%1 <-> %2")
                  .arg(key, payload.toString()),
              QMessageBox::Ok);
#endif
          exp->executionManager()->reset();
        }
      });

  // React to incoming control requests from clients
  connect(
      exp->operatorsManager(), &OperatorsManager::controlReceived,
      [exp](const QString &key, const QJsonValue &payload, const Operator &cc) {
        auto card{cc.card()};
        if (card->access() == AccessCard::AccessType::FULL_VIEW_AUTOSTART ||
            card->access() == AccessCard::AccessType::FULL_VIEW_AUTORESTART) {
          if (key == "start") {
            exp->executionManager()->resume();
          } else if (key == "restart") {
            exp->executionManager()->reset();
          } else if (key == "stop") {
            exp->executionManager()->reset();
          } else if (key == "pause") {
            exp->executionManager()->pause();
          } else if (key == "resume") {
            exp->executionManager()->resume();
          } else if (key == "toggle") {
            exp->executionManager()->toggle();
          } else if (key == "init") {
            exp->executionManager()->initialize();
          } else if (key == "logout") {
            exp->operatorsManager()->disconnect(card);
          }
        }
        if (key == "log") {
          auto pobj{payload.toObject()};
          exp->loggingManager()->log(pobj["tag"].toString(),
                                     cc.card()->username(),
                                     pobj["message"].toString());
        } else if (key == "content") {
          auto pobj{payload.toObject()};
          if (pobj.contains("id")) {
            auto id{pobj["id"].toString()};
            if (exp->contentLibrary()->contains(id)) {
              QJsonObject response;
              response["id"] = id;
              response["data"] =
                  QString{exp->contentLibrary()->get(id).toBase64()};
              exp->operatorsManager()->sendControl("content", response,
                                                   cc.card());
            } else {
              QJsonObject response;
              response["id"] = id;
              exp->operatorsManager()->sendControl("content", response,
                                                   cc.card());
            }
          }
        } else if (key == "squawk") {
          QJsonObject response;
          response["type"] = static_cast<int>(card->access());
          response["alias"] = card->alias();
          response["username"] = card->username();
          response["experiment"] = exp->name();
          exp->operatorsManager()->sendControl("squawk", response, cc.card());
        } else if (key == "dump") {
          exp->simulator()->registry()->dump(cc.card()->username());
          QJsonObject response;
          response["type"] = static_cast<int>(card->access());
          response["alias"] = card->alias();
          response["username"] = card->username();
          response["experiment"] = exp->name();
          exp->operatorsManager()->sendControl("dump", response, cc.card());
        }
      });

  // Notify registry updates to clients
  connect(exp->simulator()->registry(), &Registry::changed,
          [exp](const QString &key, const QJsonValue &value, const QString &who,
                bool skipLogging, const QString &notifyTarget) {
            Q_UNUSED(who)
            Q_UNUSED(skipLogging)
            if (notifyTarget.isEmpty()) {
              exp->operatorsManager()->broadcastUpdate(key, value);
            } else {
              if (exp->credentialManager()->containsUsername(notifyTarget)) {
                auto cc =
                    exp->credentialManager()->cardByUsername(notifyTarget);
                exp->operatorsManager()->sendUpdate(key, value, cc);
              }
            }
          });
}
