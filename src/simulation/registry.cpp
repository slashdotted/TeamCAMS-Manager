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
#include "simulation/registry.h"
#include "experiment/exceptions.h"
#include "qdebug.h"

struct Registry::pimpl {
  QMap<QString, QJsonValue> m_stuck_properties;
  QMap<QString, QJsonValue> m_properties;
};

Registry::~Registry() = default;

static QString canonicalName(const QString &name)
{
    auto cname{name.toLower()};
    if (cname != name) {
        qWarning() << "Property" << name << "is not lowercase";
    }
    return cname;
}

Registry::Registry(QObject *parent)
    : QObject{parent}, m_pimpl{std::make_unique<pimpl>()} {}

void Registry::set(const QString &pName,
                   const QJsonValue &value,
                   bool initialize,
                   const QString &who)
{
    auto name{canonicalName(pName)};
    // Stuck properties will not change value if in the stuck state
    if (initialize && m_pimpl->m_properties.contains(name)) {
        throw ReinitializePropertyException{name};
    } else if (name.toLower() != name) {
        throw InvalidPropertyNameException{name};
    } else if (initialize || m_pimpl->m_properties.contains(name)) {
        if (initialize || !isStuck(name)) {
            if (initialize || m_pimpl->m_properties[name] != value) {
                m_pimpl->m_properties[name] = value;
                if (initialize) {
                    emit propertyAdded();
                }
                emit changed(name, value, who);
            }
        } else {
            emit changed(name, m_pimpl->m_properties[name], who);
        }
    } else {
        throw UndefinedPropertyException{name};
    }
}

QJsonValue Registry::get(const QString &pName, bool ignoreUndefined) const
{
    auto name{canonicalName(pName)};
    if (m_pimpl->m_properties.contains(name))
        return m_pimpl->m_properties[name];
    else if (!ignoreUndefined) {
        throw UndefinedPropertyException{name};
    } else {
        return QJsonValue::Null;
    }
}

void Registry::stick(const QString &pName, const QJsonValue &value)
{
    auto name{canonicalName(pName)};
    if (!m_pimpl->m_properties.contains(name)) {
        throw UndefinedPropertyException{name};
    }
    m_pimpl->m_stuck_properties[name] = value;
    emit sticknessChanged(name);
}

void Registry::stick(const QString &pName)
{
    auto name{canonicalName(pName)};
    if (!m_pimpl->m_properties.contains(name)) {
        throw UndefinedPropertyException{name};
    }
    m_pimpl->m_stuck_properties[name] = get(name);
    emit sticknessChanged(name);
}

void Registry::setAndStick(const QString &pName, const QJsonValue &value)
{
    auto name{canonicalName(pName)};
    set(name, value);
    m_pimpl->m_stuck_properties[name] = value;
    emit sticknessChanged(name);
}

void Registry::unstick(const QString &pName)
{
    auto name{canonicalName(pName)};
    if (!m_pimpl->m_properties.contains(name)) {
        throw UndefinedPropertyException{name};
    }
    m_pimpl->m_stuck_properties.remove(name);
    emit sticknessChanged(name);
}

bool Registry::isStuck(const QString &pName) const
{
    auto name{canonicalName(pName)};
    if (!m_pimpl->m_properties.contains(name)) {
        throw UndefinedPropertyException{name};
    }
    return (m_pimpl->m_stuck_properties.contains(name)
            && m_pimpl->m_stuck_properties[name] == get(name));
}

void Registry::dump(const QString &notifyTarget) const {
  QMap<QString, QJsonValue>::const_iterator it;
  for (it = m_pimpl->m_properties.constBegin();
       it != m_pimpl->m_properties.constEnd(); ++it) {
    emit changed(it.key(), it.value(), "<system>", true, notifyTarget);
  }
}

bool Registry::exists(const QString &pName) const
{
    auto name{canonicalName(pName)};
    return m_pimpl->m_properties.contains(name);
}

void Registry::touch(const QString &pName) const
{
    auto name{canonicalName(pName)};
    if (!m_pimpl->m_properties.contains(name)) {
        throw UndefinedPropertyException{name};
    }
    emit changed(name, m_pimpl->m_properties[name], "<system>");
}

void Registry::reset() {
  emit beginReset();
  auto counter{m_pimpl->m_properties.count()};
  m_pimpl->m_properties.clear();
  m_pimpl->m_stuck_properties.clear();
  emit endReset();
}

QStringList Registry::stuckProperties() const {
  return m_pimpl->m_stuck_properties.keys();
}

QString Registry::name(unsigned int i) const {
  return m_pimpl->m_properties.keys().at(i);
}

QJsonValue Registry::value(unsigned int i) const {
  return m_pimpl->m_properties.values().at(i);
}

bool Registry::isStuck(unsigned int i) const {
  return m_pimpl->m_stuck_properties.contains(
      m_pimpl->m_properties.keys().at(i));
}

unsigned int Registry::count() const { return m_pimpl->m_properties.count(); }

unsigned int Registry::index(const QString &pName) const
{
    auto name{canonicalName(pName)};
    return m_pimpl->m_properties.keys().indexOf(name);
}
