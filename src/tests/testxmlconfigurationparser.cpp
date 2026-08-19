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
#include "testxmlconfigurationparser.h"
#include "experiment/config/xmlconfigparser.h"
#include <QtTest/QtTest>

TestXMLConfigurationParser::TestXMLConfigurationParser(QObject *parent)
    : QObject(parent) {}

void TestXMLConfigurationParser::testScript() {
  XMLConfigParser xcp;
  {
    auto ba = QByteArray{"<?xml version=\"1.0\" encoding=\"UTF-8\" ?><script "
                         "adaptive=\"bogus\"/>"};
    auto data = QBuffer{&ba};
    QVERIFY_THROWS_EXCEPTION(XMLConfigParserException, xcp.validate(data));
  }
  {
    auto ba = QByteArray{"<?xml version=\"1.0\" encoding=\"UTF-8\" ?><script "
                         "adaptiveparams=\"bogus\"/>"};
    auto data = QBuffer{&ba};
    QVERIFY_THROWS_EXCEPTION(XMLConfigParserException, xcp.validate(data));
  }
}

void TestXMLConfigurationParser::testAdaptive() {
  XMLConfigParser xcp;
  {
    auto ba = QByteArray{"<?xml version=\"1.0\" encoding=\"UTF-8\" ?><adaptive "
                         "adaptiveparams=\"z\"/>"};
    auto data = QBuffer{&ba};
    QVERIFY_THROWS_EXCEPTION(XMLConfigParserException, xcp.validate(data));
  }
}

void TestXMLConfigurationParser::testQuestion() {
  XMLConfigParser xcp;
  {
    auto ba =
        QByteArray{"<?xml version=\"1.0\" encoding=\"UTF-8\" ?><question/>"};
    auto data = QBuffer{&ba};
    QVERIFY_THROWS_EXCEPTION(XMLConfigParserException, xcp.validate(data));
  }
  {
    auto ba = QByteArray{
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><question id=\"z\"/>"};
    auto data = QBuffer{&ba};
    QVERIFY_THROWS_EXCEPTION(XMLConfigParserException, xcp.validate(data));
  }
}

void TestXMLConfigurationParser::testDuration() {
  XMLConfigParser xcp;
  {
    auto ba =
        QByteArray{"<?xml version=\"1.0\" encoding=\"UTF-8\" ?><duration/>"};
    auto data = QBuffer{&ba};
    QVERIFY_THROWS_EXCEPTION(XMLConfigParserException, xcp.validate(data));
  }
  {
    auto ba = QByteArray{
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><duration seconds=\"z\"/>"};
    auto data = QBuffer{&ba};
    QVERIFY_THROWS_EXCEPTION(XMLConfigParserException, xcp.validate(data));
  }
}

void TestXMLConfigurationParser::testAssistance() {
  XMLConfigParser xcp;

  {
    auto ba =
        QByteArray{"<?xml version=\"1.0\" encoding=\"UTF-8\" ?><assistance/>"};
    auto data = QBuffer{&ba};
    QVERIFY_THROWS_EXCEPTION(XMLConfigParserException, xcp.validate(data));
  }
  {
    auto ba = QByteArray{
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><assistance start=\"z\"/>"};
    auto data = QBuffer{&ba};
    QVERIFY_THROWS_EXCEPTION(XMLConfigParserException, xcp.validate(data));
  }
  {
    auto ba = QByteArray{"<?xml version=\"1.0\" encoding=\"UTF-8\" "
                         "?><assistance start=\"33\"/>"};
    auto data = QBuffer{&ba};
    QVERIFY_THROWS_EXCEPTION(XMLConfigParserException, xcp.validate(data));
  }
}

void TestXMLConfigurationParser::testInterface() {
  XMLConfigParser xcp;
  {
    auto ba =
        QByteArray{"<?xml version=\"1.0\" encoding=\"UTF-8\" ?><interface/>"};
    auto data = QBuffer{&ba};
    QVERIFY_THROWS_EXCEPTION(XMLConfigParserException, xcp.validate(data));
  }
  {
    auto ba = QByteArray{
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><interface start=\"z\"/>"};
    auto data = QBuffer{&ba};
    QVERIFY_THROWS_EXCEPTION(XMLConfigParserException, xcp.validate(data));
  }
  {
    auto ba = QByteArray{
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><interface display=\"\"/>"};
    auto data = QBuffer{&ba};
    QVERIFY_THROWS_EXCEPTION(XMLConfigParserException, xcp.validate(data));
  }
  {
    auto ba = QByteArray{
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><interface visible=\"\"/>"};
    auto data = QBuffer{&ba};
    QVERIFY_THROWS_EXCEPTION(XMLConfigParserException, xcp.validate(data));
  }
  {
    auto ba = QByteArray{
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><interface enabled=\"\"/>"};
    auto data = QBuffer{&ba};
    QVERIFY_THROWS_EXCEPTION(XMLConfigParserException, xcp.validate(data));
  }
}

void TestXMLConfigurationParser::testControl() {
  XMLConfigParser xcp;

  {
    auto ba =
        QByteArray{"<?xml version=\"1.0\" encoding=\"UTF-8\" ?><control/>"};
    auto data = QBuffer{&ba};
    QVERIFY_THROWS_EXCEPTION(XMLConfigParserException, xcp.validate(data));
  }
  {
    auto ba = QByteArray{
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><control start=\"z\"/>"};
    auto data = QBuffer{&ba};
    QVERIFY_THROWS_EXCEPTION(XMLConfigParserException, xcp.validate(data));
  }
}

void TestXMLConfigurationParser::testCode() {
  XMLConfigParser xcp;

  {
    auto ba = QByteArray{
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><code start=\"z\"/>"};
    auto data = QBuffer{&ba};
    QVERIFY_THROWS_EXCEPTION(XMLConfigParserException, xcp.validate(data));
  }
}

void TestXMLConfigurationParser::testError() {
  XMLConfigParser xcp;
  {
    auto ba = QByteArray{
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><error start=\"z\"/>"};
    auto data = QBuffer{&ba};
    QVERIFY_THROWS_EXCEPTION(XMLConfigParserException, xcp.validate(data));
  }
  {
    auto ba = QByteArray{"<?xml version=\"1.0\" encoding=\"UTF-8\" ?><error "
                         "diagnostics=\"z\"/>"};
    auto data = QBuffer{&ba};
    QVERIFY_THROWS_EXCEPTION(XMLConfigParserException, xcp.validate(data));
  }
  {
    auto ba = QByteArray{
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><error fake=\"z\"/>"};
    auto data = QBuffer{&ba};
    QVERIFY_THROWS_EXCEPTION(XMLConfigParserException, xcp.validate(data));
  }
  {
    auto ba = QByteArray{
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><error efficiency=\"z\"/>"};
    auto data = QBuffer{&ba};
    QVERIFY_THROWS_EXCEPTION(XMLConfigParserException, xcp.validate(data));
  }
  {
    auto ba = QByteArray{
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><error notify=\"-3\"/>"};
    auto data = QBuffer{&ba};
    QVERIFY_THROWS_EXCEPTION(XMLConfigParserException, xcp.validate(data));
  }
  {
    auto ba = QByteArray{
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><error efficiency=\"z\"/>"};
    auto data = QBuffer{&ba};
    QVERIFY_THROWS_EXCEPTION(XMLConfigParserException, xcp.validate(data));
  }
}
