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
#include "contentlibrary.h"
#include <QDebug>
#include <QMap>
#include <QString>

struct ContentLibrary::pimpl {
  QMap<QString, QByteArray> m_content;
};

ContentLibrary::~ContentLibrary() = default;

ContentLibrary::ContentLibrary(QObject *exp)
    : QObject{exp}, m_pimpl{std::make_unique<pimpl>()} {}

const QByteArray &ContentLibrary::get(const QString &key) const {
  return m_pimpl->m_content[key];
}

bool ContentLibrary::contains(const QString &key) const noexcept {
  return m_pimpl->m_content.contains(key);
}

void ContentLibrary::set(const QString &key, const QByteArray &value) noexcept {
  m_pimpl->m_content[key] = value;
}
