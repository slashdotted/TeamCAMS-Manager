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
#ifndef XMLCONFIGPARSER_H
#define XMLCONFIGPARSER_H
#include "events/scheduler.h"
#include "experiment/experiment.h"
#include "simulation/registry.h"
#include <QFile>
#include <QObject>
#include <QString>
#include <QXmlStreamReader>
#include <utility>

class XMLConfigParserException {
public:
  XMLConfigParserException(QString what) : m_what{std::move(what)} {}
  const QString &what() const { return m_what; }

private:
  QString m_what;
};

class XMLConfigParser : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(XMLConfigParser)

public:
  XMLConfigParser(Experiment *e = nullptr, Scheduler *s = nullptr,
                  Registry *r = nullptr);
  ~XMLConfigParser() override;
  void parse(const QString &filename, bool validationMode = false);
  void parseString(const QString &config);
  void validate(QIODevice &file);

private slots:
  void handleScriptElement(const QJsonObject &parameters);
  void handleQuestionElement(const QString &id, const QString &text);
  void handleDurationElement(unsigned int endTime);
  void handleAssistanceElement(unsigned int startTime,
                               const QJsonObject &parameters);
  void handleInterfaceElement(unsigned int startTime, const QString &display,
                              const QString &modifier, const QString &value,
                              const QString &users);
  void handleControlElement(unsigned int startTime, const QString &cmd);
  void handleAdaptiveElement(unsigned int startTime,
                             const QJsonObject &parameters);
  void handleErrorElement(unsigned int startTime, const QString &errorType,
                          const QJsonObject &parameters);
  void handleCodeElement(unsigned int startTime, const QString &code);
  void handleFilterElement(unsigned int startTime, bool filterEnabled,
                           const QString &mtype, const QString &key,
                           const QString &user, bool feedback);
  void handleAccessElement(unsigned int startTime, const QString &username,
                           int accessType);
  void handleMessageElement(unsigned int startTime, const QString &sender,
                            const QString &recipient, const QString &message);
  void handlePopupElement(unsigned int startTime, const QJsonObject &parameters);

  private:
  struct pimpl;
  std::unique_ptr<pimpl> m_pimpl;
};

#endif // XMLCONFIGPARSER_H
