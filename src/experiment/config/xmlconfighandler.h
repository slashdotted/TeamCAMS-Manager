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
#ifndef XMLCONFIGHANDLER_H
#define XMLCONFIGHANDLER_H
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QStringView>
#include <QXmlStreamAttributes>
#include <memory>
#include <utility>

class XMLConfigHandlerException {
public:
  explicit XMLConfigHandlerException(QString what) : m_what{std::move(what)} {}
  const QString &what() const { return m_what; }

private:
  QString m_what;
};

class XMLConfigHandler : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(XMLConfigHandler)
public:
  XMLConfigHandler(bool strict_verification = true, QObject *parent = nullptr);
  ~XMLConfigHandler() override;
  bool startElement(QStringView localName, const QXmlStreamAttributes &atts);
  bool characters(QStringView ch);
  bool endElement(QStringView localName);

signals:
  void scriptElementRead(const QJsonObject &parameters);
  void questionElementRead(const QString &id, const QString &text);
  void durationElementRead(unsigned int endTime);
  void assistanceElementRead(unsigned int startTime,
                             const QJsonObject &parameters);
  void interfaceElementRead(unsigned int startTime, const QString &display,
                            const QString &modifier, const QString &value,
                            const QString &users);
  void controlElementRead(unsigned int startTime, const QString &cmd);
  void adaptiveElementRead(unsigned int startTime,
                           const QJsonObject &parameters);
  void errorElementRead(unsigned int startTime, const QString &errorType,
                        const QJsonObject &parameters);
  void codeElementRead(unsigned int startTime, const QString &code);
  void filterElementRead(unsigned int startTime, bool filterEnabled,
                         const QString &mtype, const QString &key,
                         const QString &user, bool feedback);
  void messageElementRead(unsigned int startTime, const QString &sender,
                          const QString &recipient, const QString &text);
  void accessElementRead(unsigned int startTime, const QString &sender,
                         int accessType);
  void popupElementRead(unsigned int startTime, const QJsonObject &parameters);
  void popupContentElementRead(const QString &id, const QString &data);

protected:
  void parseScriptElement(const QXmlStreamAttributes &atts);
  void parseQuestionElement(const QXmlStreamAttributes &atts);
  void parseDurationElement(const QXmlStreamAttributes &atts);
  void parseAssistanceElement(const QXmlStreamAttributes &atts);
  void parseInterfaceElement(const QXmlStreamAttributes &atts);
  void parseControlElement(const QXmlStreamAttributes &atts);
  void parseAdaptiveElement(const QXmlStreamAttributes &atts);
  void parseCodeElement(const QXmlStreamAttributes &atts);
  void parseErrorElement(const QXmlStreamAttributes &atts);
  void parseFilterElement(const QXmlStreamAttributes &atts);
  void parseMessageElement(const QXmlStreamAttributes &atts);
  void parseAccessElement(const QXmlStreamAttributes &atts);
  void parsePopupElement(const QXmlStreamAttributes &atts);
  void parsePopupContentElement(const QXmlStreamAttributes &atts);

private:
  struct pimpl;
  std::unique_ptr<pimpl> m_pimpl;
};

#endif // XMLCONFIGHANDLER_H
