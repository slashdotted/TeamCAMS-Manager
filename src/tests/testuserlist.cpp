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
#include "testuserlist.h"
#include "experiment/experiment.h"
#include "experiment/operators/access/accesscard.h"
#include "experiment/operators/access/accessmanager.h"
#include <QtTest/QtTest>

TestUserList::TestUserList(QObject *parent) : QObject(parent) {}

void TestUserList::testAddUser() {
  Experiment e{"default"};
  AccessManager ul{&e};
  int i = ul.add("sample", "samplealias", "password",
                 AccessCard::AccessType::FULL_VIEW_AUTOSTART);
  QVERIFY(ul.containsUsername("sample"));
  int j = ul.add("sample", "samplealias", "password",
                 AccessCard::AccessType::FULL_VIEW_AUTOSTART);
  QVERIFY(i == j);
  ul.add("another", "anotheralias", "password",
         AccessCard::AccessType::DISABLED);
  QVERIFY(ul.containsUsername("another"));
}

void TestUserList::testContainsUser() {
  Experiment e{"default"};
  AccessManager ul{&e};
  ul.add("sample", "samplealias", "password",
         AccessCard::AccessType::FULL_VIEW_AUTOSTART);
  QVERIFY(ul.containsUsername("sample"));
}

void TestUserList::testRemoveUser() {
  Experiment e{"default"};
  AccessManager ul{&e};
  ul.add("sample", "samplealias", "password",
         AccessCard::AccessType::FULL_VIEW_AUTOSTART);
  QVERIFY(ul.containsUsername("sample"));
  ul.removeByUsername("sample");
  QVERIFY(!ul.containsUsername("sample"));
}

void TestUserList::testAuthenticated() {
  Experiment e{"default"};
  AccessManager ul{&e};
  ul.add("sample", "samplealias", "password",
         AccessCard::AccessType::FULL_VIEW_AUTOSTART);
  QVERIFY(ul.authenticate("sample", "password"));
  QVERIFY(!ul.authenticate("sample", "wrong"));
}

void TestUserList::testUsername() {
  Experiment e{"default"};
  AccessManager ul{&e};
  int i = ul.add("sample", "samplealias", "password",
                 AccessCard::AccessType::FULL_VIEW_AUTOSTART);
  QVERIFY(ul.cardByIndex(i)->username() == "sample");
}

void TestUserList::testUserRole() {
  Experiment e{"default"};
  AccessManager ul{&e};
  ul.add("sample", "samplealias", "password",
         AccessCard::AccessType::FULL_VIEW_AUTOSTART);
  QVERIFY(ul.cardByUsername("sample")->access() ==
          AccessCard::AccessType::FULL_VIEW_AUTOSTART);
}

void TestUserList::testSerialization() {
  QJsonObject data;
  Experiment e{"default"};
  AccessManager ul{&e};
  ul.add("sample", "samplealias", "password",
         AccessCard::AccessType::FULL_VIEW_AUTOSTART);
  ul.add("sample2", "samplealias", "password",
         AccessCard::AccessType::FULL_VIEW);
  QVERIFY(ul.containsUsername("sample"));
  QVERIFY(ul.containsUsername("sample2"));
  ul.save(data);
  Experiment e2{"default"};
  AccessManager ul2{&e2};
  ul2.load(data);
  QVERIFY(ul2.containsUsername("sample"));
  QVERIFY(ul2.containsUsername("sample2"));
  QVERIFY(ul2.cardByUsername("sample")->access() ==
          AccessCard::AccessType::FULL_VIEW_AUTOSTART);
  QVERIFY(ul2.cardByUsername("sample2")->access() ==
          AccessCard::AccessType::FULL_VIEW);
  QVERIFY(ul2.authenticate("sample", "password"));
}
