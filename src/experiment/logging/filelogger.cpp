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

#include "filelogger.h"
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QUuid>
#include <memory>

#ifdef MANAGERGUI
#include "gui/asyncmessageboxfactory.h"
#include <QApplication>
#include <QMessageBox>
#endif

#ifdef Q_OS_WASM
#include <emscripten/emscripten.h>
#endif

#include "experiment/experiment.h"
#include "simulation/registry.h"

void syncWasmFileSystem(bool populate) {
#ifdef Q_OS_WASM
  EM_ASM(
      {
        FS.syncfs(
            $0, function(err) {
              if (err)
                console.error('IDBFS sync error:', err);
            });
      },
      populate ? 1 : 0);
#else
  Q_UNUSED(populate);
#endif
}

struct FileLogger::pimpl {
  const Experiment *m_exp{nullptr};
  const Registry *m_registry{nullptr};
  QFile m_debug_file;
  QString m_experiment_filename;
  std::unique_ptr<QTextStream> m_debug_stream{nullptr};
};

FileLogger::FileLogger(const Experiment *exp, const Registry *registry,
                       QObject *parent)
    : QObject{parent}, m_pimpl{std::make_unique<pimpl>()} {
  m_pimpl->m_exp = exp;
  m_pimpl->m_registry = registry;

#ifdef Q_OS_WASM
  EM_ASM({
    if (!FS.analyzePath('/Logs').exists) {
      FS.mkdir('/Logs');
    }

    var info = FS.analyzePath('/Logs');
    var isMounted = info.exists && info.object && info.object.mount &&
                    info.object.mount.mountpoint == '/Logs';

    if (!isMounted) {
      FS.mount(IDBFS, {}, '/Logs');
    }
  });
  syncWasmFileSystem(true);
#endif
}

FileLogger::~FileLogger() { cleanup(); }

void FileLogger::startSession(const QString &experiment_name) {
  m_pimpl->m_experiment_filename = experiment_name;
}

void FileLogger::createSession() {
  if (m_pimpl->m_debug_stream == nullptr &&
      !m_pimpl->m_experiment_filename.isEmpty()) {
    auto dir = logPath();
    if (dir.path().isEmpty()) {
#ifdef MANAGERGUI
      AsyncMessageBoxFactory::critical(
          QApplication::activeWindow(), tr("Cannot create log"),
          tr("Cannot access log directory"), QMessageBox::Ok);
#else
      qCritical() << "Cannot access log directory:" << dir;
#endif
      return;
    }

    QString filename{
        m_pimpl->m_experiment_filename + "_" +
        QDateTime::currentDateTime().toString("yyyy-MM-dd_HH.mm.ss") + "_" +
        QUuid::createUuid().toString(QUuid::WithoutBraces) + ".csv"};

    m_pimpl->m_debug_file.setFileName(dir.absoluteFilePath(filename));
    if (m_pimpl->m_debug_file.open(QIODevice::ReadWrite | QIODevice::Text)) {
      m_pimpl->m_debug_stream =
          std::make_unique<QTextStream>(&m_pimpl->m_debug_file);
    } else {
#ifdef MANAGERGUI
      AsyncMessageBoxFactory::critical(
          QApplication::activeWindow(), tr("Cannot create log"),
          tr("Cannot create log file %1").arg(dir.absoluteFilePath(filename)),
          QMessageBox::Ok);
#else
      qCritical() << "Cannot create log file:"
                  << dir.absoluteFilePath(filename);
#endif
    }
  }
}

void FileLogger::endSession() { cleanup(); }

void FileLogger::log(double timestamp, const QString &tag,
                     const QString &sender, const QString &text) {
  createSession();

  if (m_pimpl->m_debug_stream != nullptr && m_pimpl->m_registry != nullptr) {
    QString escapedText{text};
    escapedText.replace('"', "\\\"");
    QString cleanTag{'"' + tag + '"'};
    QString cleanText{'"' + escapedText + '"'};

    static long runningNumber{0};

    (*m_pimpl->m_debug_stream)
        << timestamp / 1000.0 << ";" << QDateTime::currentMSecsSinceEpoch()
        << ";"
        << m_pimpl->m_registry->get("components.cabin.o2.relvalue", true)
               .toDouble()
        << ";"
        << m_pimpl->m_registry->get("components.cabin.pressure", true)
               .toDouble()
        << ";"
        << m_pimpl->m_registry->get("components.cabin.temperature", true)
               .toDouble()
        << ";"
        << m_pimpl->m_registry->get("components.cabin.co2.relvalue", true)
               .toDouble()
        << ";"
        << m_pimpl->m_registry->get("components.cabin.humidity", true)
               .toDouble()
        << ";"
        << m_pimpl->m_registry->get("components.o2tank.volume", true).toDouble()
        << ";"
        << m_pimpl->m_registry->get("components.n2tank.volume", true).toDouble()
        << ";" << cleanTag << ";" << cleanText << ";"
        << m_pimpl->m_registry->get("system.error", true).toString() << ";"
        << runningNumber++ << ";" << sender << "\n";
    if (runningNumber % 1024 == 0) {
      m_pimpl->m_debug_file.flush();
      syncWasmFileSystem(false);
    }
  }
}

QSet<LogRecord> FileLogger::list() const {
  QSet<LogRecord> records;
  QDir dir = logPath();
  QStringList filters;
  filters << "*.csv";

  QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files, QDir::Time);

  for (const QFileInfo &fileInfo : std::as_const(fileList)) {
    QString filename = fileInfo.fileName();
    QString nameWithoutExt = fileInfo.baseName();
    QStringList tokens = nameWithoutExt.split('_');
    QDateTime dt;
    if (tokens.size() >= 4) {
      QString dtString =
          tokens.at(tokens.size() - 3) + "_" + tokens.at(tokens.size() - 2);
      dt = QDateTime::fromString(dtString, "yyyy-MM-dd_HH.mm.ss");
    }
    if (!dt.isValid()) {
      dt = fileInfo.lastModified();
    }
    LogRecord record;
    record.key = filename;
    record.datetime = dt;
    records.insert(record);
  }

  return records;
}

QString FileLogger::fetch(const LogRecord &logRecordKey) const {
  QDir dir = logPath();
  QFile file(dir.absoluteFilePath(logRecordKey.key));

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning() << "Cannot read the log record:" << file.fileName();
    return QString();
  }

  QTextStream in(&file);
  QString content = in.readAll();
  file.close();

  return content;
}

void FileLogger::discard(const LogRecord &logRecordKey) const {
  QDir dir = logPath();
  QString filePath = dir.absoluteFilePath(logRecordKey.key);

  if (m_pimpl->m_debug_file.isOpen() &&
      m_pimpl->m_debug_file.fileName() == filePath) {
    const_cast<FileLogger *>(this)->endSession();
  }

  if (QFile::exists(filePath)) {
    if (QFile::remove(filePath)) {
      syncWasmFileSystem(false);
    } else {
      qWarning() << "Cannot discard log record file:" << filePath;
    }
  }
}

void FileLogger::cleanup() {
  if (m_pimpl->m_debug_stream != nullptr) {
    m_pimpl->m_debug_stream->flush();
    m_pimpl->m_debug_file.flush();
    m_pimpl->m_debug_file.close();
    m_pimpl->m_debug_stream.reset();
    m_pimpl->m_experiment_filename.clear();
    syncWasmFileSystem(false);
  }
}

QDir FileLogger::logPath() const {
  QDir dir;
#ifdef Q_OS_WASM
  dir.setPath("/Logs");
#else
  QString appDataPath =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  dir.setPath(appDataPath + QDir::separator() + "Logs");
#endif

  if (!dir.exists()) {
    dir.mkpath(".");
  }
  return dir;
}
