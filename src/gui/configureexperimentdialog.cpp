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
#include "gui/configureexperimentdialog.h"
#include "delegates/feedbackonupdatedelegate.h"
#include "delegates/messagetypecombodelegate.h"
#include "delegates/trafficdirectioncombodelegate.h"
#include "delegates/userrolecombodelegate.h"
#include "experiment/config/xmlconfigparser.h"
#include "gui/asyncmessageboxfactory.h"
#include "models/filterlist.h"
#include "models/propertylist.h"
#include "models/userlist.h"
#include "qrcodegen.h"
#include "qurlquery.h"
#include "ui_configureexperimentdialog.h"
#include "utils/persistentsettings.h"
#include <QClipboard>
#include <QDesktopServices>
#include <QDoubleSpinBox>
#include <QDropEvent>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QPainter>
#include <QStandardPaths>
#include <QSvgRenderer>
#include <qbuffer.h>

static QString qrToSvgString(const qrcodegen::QrCode &qr, int border) {
  if (border < 0)
    throw std::domain_error("Border must be non-negative");
  if (border > INT_MAX / 2 || border * 2 > INT_MAX - qr.getSize())
    throw std::overflow_error("Border too large");

  QString svgString;
  QTextStream stream(&svgString);

  stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  stream << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" "
            "\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
  stream << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" "
            "viewBox=\"0 0 ";
  stream << (qr.getSize() + border * 2) << " " << (qr.getSize() + border * 2)
         << "\" stroke=\"none\">\n";
  stream << "\t<rect width=\"100%\" height=\"100%\" fill=\"#FFFFFF\"/>\n";
  stream << "\t<path d=\"";
  for (int y = 0; y < qr.getSize(); y++) {
    for (int x = 0; x < qr.getSize(); x++) {
      if (qr.getModule(x, y)) {
        if (x != 0 || y != 0)
          stream << " ";
        stream << "M" << (x + border) << "," << (y + border) << "h1v1h-1z";
      }
    }
  }
  stream << "\" fill=\"#000000\"/>\n";
  stream << "</svg>\n";

  return svgString;
}

static QPixmap generateQrPixmap(const QString &id, QSize *size,
                                const QSize &requestedSize) {
  int width = 1000;
  int height = 1000;

  const qrcodegen::QrCode qr =
      qrcodegen::QrCode::encodeText(id.toLatin1(), qrcodegen::QrCode::Ecc::LOW);

  QString svgString = qrToSvgString(qr, 3);
  QSvgRenderer svgRenderer(svgString.toLatin1());

  if (size)
    *size = QSize(width, height);

  QPixmap pixmap(requestedSize.width() > 0 ? requestedSize.width() : width,
                 requestedSize.height() > 0 ? requestedSize.height() : height);

  QPainter painter(&pixmap);

  svgRenderer.render(&painter);

  return pixmap;
}

QUrl ConfigureExperimentDialog::getConnectionUrl(
    const UserCardPtr &card) const {
  PersistentSettings settings;
  auto uuid = settings.value("instanceUuid").toString();
  auto address = settings.value("clientUrl").toString();
  QUrl url{address};
  QUrlQuery query;

  QList<QPair<QString, QString>> values{
      {"m", uuid},
      {"u", card->username()},
      {"p", card->password()},
      {"e", m_experiment->name()},
      {"a", "1"},
      {"l", settings.value("language").toString()},
      {"b", settings.value("clientBrokerUrl").toString()}};
  query.setQueryItems(values);
  url.setQuery(query);
  return url;
}

ConfigureExperimentDialog::ConfigureExperimentDialog(Experiment *exp,
                                                     QWidget *parent)
    : QDialog{parent}, m_experiment{exp},
      ui{new Ui::ConfigureExperimentDialog} {
  ui->setupUi(this);
  this->setWindowTitle(tr("Experiment configuration (%1)").arg(exp->name()));
  ui->scriptNameTextEdit->setText(exp->script());
  ui->validateScriptButton->setEnabled(
      ui->scriptNameTextEdit->text().trimmed() != "");
  ui->descriptionTextEdit->setText(exp->description());
  ui->availableCheckBox->setChecked(exp->enabled());

  ui->userTableView->setModel(
      new UserList{exp->credentialManager(), exp->operatorsManager(), this});
  ui->userTableView->setSelectionMode(QAbstractItemView::SingleSelection);
  for (int c = 0; c < ui->userTableView->horizontalHeader()->count(); ++c) {
    ui->userTableView->horizontalHeader()->setSectionResizeMode(
        c, QHeaderView::Stretch);
  }
  ui->userTableView->setAlternatingRowColors(true);
  ui->userTableView->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->userTableView->setItemDelegateForColumn(
      2, new UserRoleComboDelegate(ui->userTableView));
  ui->userTableView->setItemDelegateForColumn(4, new NonEditingDelegate{this});

  ui->registryView->setModel(
      new PropertyList{exp->simulator()->registry(), this});
  for (int c = 0; c < ui->registryView->horizontalHeader()->count(); ++c) {
    ui->registryView->horizontalHeader()->setSectionResizeMode(
        c, QHeaderView::Stretch);
  }
  ui->registryView->setAlternatingRowColors(true);

  ui->filterTableView->setModel(
      new FilterList{exp->operatorsManager()->filter(), this});
  ui->filterTableView->setSelectionMode(QAbstractItemView::SingleSelection);
  for (int c = 0; c < ui->filterTableView->horizontalHeader()->count(); ++c) {
    ui->filterTableView->horizontalHeader()->setSectionResizeMode(
        c, QHeaderView::Stretch);
  }
  ui->filterTableView->setItemDelegateForColumn(
      0, new TrafficDirectionComboDelegate{this});
  ui->filterTableView->setItemDelegateForColumn(
      1, new MessageTypeComboDelegate{this});
  ui->filterTableView->setItemDelegateForColumn(
      4, new FeedbackOnUpdateDelegate{this});
  ui->filterTableView->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->filterTableView->setAlternatingRowColors(true);

  connect(ui->actionToggle_filter_editing, &QAction::triggered, this,
          &ConfigureExperimentDialog::toggleFilterEditTriggers);
  connect(ui->descriptionTextEdit, &QTextEdit::textChanged, this,
          &ConfigureExperimentDialog::onDescriptionTextChanged);
  connect(ui->chooseScriptButton, &QPushButton::clicked, this,
          &ConfigureExperimentDialog::onChooseScriptButton);
  connect(ui->closeButton, &QDialogButtonBox::clicked, this,
          &ConfigureExperimentDialog::close);
  connect(ui->actionRemove_user, &QAction::triggered, this,
          &ConfigureExperimentDialog::removeUser);
  connect(ui->actionAdd_user, &QAction::triggered, this,
          &ConfigureExperimentDialog::newUser);
  connect(ui->actionCopy_connection_Url_to_clipboard, &QAction::triggered, this,
          &ConfigureExperimentDialog::copyConnectionUrl);
  connect(ui->addUserButton, &QPushButton::clicked, ui->actionAdd_user,
          &QAction::trigger);
  connect(ui->scriptNameTextEdit, &QLineEdit::textChanged, this,
          &ConfigureExperimentDialog::onScriptChanged);
  connect(m_experiment->executionManager(), &ExecutionManager::stateChanged,
          this, &ConfigureExperimentDialog::onStateChanged);

  connect(ui->logTable, &QTableWidget::itemSelectionChanged, [this]() {
    if (ui->logTable->selectedItems().isEmpty()) {
      ui->deleteLogsButton->setEnabled(false);
      ui->exportLogsButton->setEnabled(false);
    } else {
      ui->deleteLogsButton->setEnabled(true);
      ui->exportLogsButton->setEnabled(true);
    }
  });
  ui->logTable->setAlternatingRowColors(true);

  ui->deleteLogsButton->setEnabled(false);
  ui->exportLogsButton->setEnabled(false);

  connect(ui->deleteLogsButton, &QPushButton::clicked, this,
          &ConfigureExperimentDialog::discardSelectedLog);
  connect(ui->exportLogsButton, &QPushButton::clicked, this,
          &ConfigureExperimentDialog::exportSelectedLog);

  connect(ui->userTableView, &QTableView::customContextMenuRequested, this,
          &ConfigureExperimentDialog::userTableContextMenu);

  connect(ui->actionStart, &QAction::triggered,
          m_experiment->executionManager(), &ExecutionManager::toggle);
  connect(ui->actionStop, &QAction::triggered, this, [this]() {
    this->m_experiment->operatorsManager()->disconnectAll();
    this->m_experiment->executionManager()->reset();
  });
  connect(ui->userTableView->model(), &QAbstractItemModel::dataChanged, [=]() {
    updateUserDetails();
    exp->onContentChanged();
  });
  connect(m_experiment->executionManager(), &ExecutionManager::tick, this,
          &ConfigureExperimentDialog::onUpdateTimestamp);
  ui->startToolButton->setDefaultAction(ui->actionStart);
  ui->stopToolButton->setDefaultAction(ui->actionStop);
  connect(ui->actionAdd_rule, &QAction::triggered, this,
          &ConfigureExperimentDialog::newFilterRule);
  connect(ui->actionRemove_rule, &QAction::triggered, this,
          &ConfigureExperimentDialog::removeFilterRule);
  connect(ui->filterTableView, &QTableView::customContextMenuRequested, this,
          &ConfigureExperimentDialog::filterTableContextMenu);
  connect(ui->validateScriptButton, &QPushButton::clicked, this,
          &ConfigureExperimentDialog::onValidateScriptButton);
  connect(ui->simSpeedSpinBox,
          QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
          &ConfigureExperimentDialog::onSimSpeedChanged);
  connect(ui->availableCheckBox, &QCheckBox::checkStateChanged, this,
          &ConfigureExperimentDialog::onUpdateAvailability);
  setAcceptDrops(true);
  ui->simSpeedSpinBox->setValue(m_experiment->executionManager()->speed());
  onStateChanged();
  refreshLogTable();
  onUpdateState();
  onUpdateTimestamp();
  ui->qrcodeButton->setEnabled(false);
  connect(
      ui->userTableView->selectionModel(),
      &QItemSelectionModel::selectionChanged,
      [=](const QItemSelection &selected, const QItemSelection &deselected) {
        if (selected.count() == 1) {
          updateUserDetails();
        } else {
          clearUserDetails();
        }
      });
  connect(ui->qrcodeButton, &QPushButton::clicked, [=]() {
    if (ui->userTableView->selectionModel()->selectedIndexes().count() != 1)
      return;
    int idx{ui->userTableView->selectionModel()->selectedIndexes().at(0).row()};
    auto card{m_experiment->credentialManager()->cardByIndex(idx)};
    auto url{getConnectionUrl(card)};
    QSize size{512, 512};
    QPixmap qr{generateQrPixmap(url.toString(), &size, {512, 512})};
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    qr.save(&buffer, "PNG");
    QFileDialog::saveFileContent(
        byteArray,
        QString{"%1_%2.png"}.arg(m_experiment->name(), card->username()));
  });
}

void ConfigureExperimentDialog::refreshLogTable() {
  auto logger = m_experiment->loggingManager();

  ui->logTable->clear();
  ui->logTable->setColumnCount(2);

  QStringList headers;
  headers << tr("Date") << tr("Time");
  ui->logTable->setHorizontalHeaderLabels(headers);
  ui->logTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->logTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
  ui->logTable->horizontalHeader()->setSectionResizeMode(0,
                                                         QHeaderView::Stretch);
  ui->logTable->horizontalHeader()->setSectionResizeMode(
      1, QHeaderView::ResizeToContents);
  ui->logTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

  QSet<LogRecord> recordSet = logger->list();
  QList<LogRecord> recordList = recordSet.values();

  std::sort(recordList.begin(), recordList.end(),
            [](const LogRecord &a, const LogRecord &b) {
              return a.datetime > b.datetime;
            });

  ui->logTable->setRowCount(recordList.size());

  const QLocale locale;

  for (int row = 0; row < recordList.size(); ++row) {
    const LogRecord &record = recordList.at(row);
    const QString dateStr =
        QLocale().toString(record.datetime.date(), QLocale::LongFormat);
    const QString timeStr =
        QLocale().toString(record.datetime.time(), QLocale::ShortFormat);

    auto *dateItem = new QTableWidgetItem(dateStr);
    auto *timeItem = new QTableWidgetItem(timeStr);

    dateItem->setData(Qt::UserRole, record.key);
    dateItem->setData(Qt::UserRole + 1, record.datetime);

    timeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

    ui->logTable->setItem(row, 0, dateItem);
    ui->logTable->setItem(row, 1, timeItem);
  }
}

void ConfigureExperimentDialog::exportSelectedLog() {
  auto logger = m_experiment->loggingManager();

  QList<QTableWidgetSelectionRange> ranges = ui->logTable->selectedRanges();
  QList<int> selectedRows;
  for (const auto &range : std::as_const(ranges)) {
    for (int i = range.topRow(); i <= range.bottomRow(); ++i) {
      if (!selectedRows.contains(i)) {
        selectedRows.append(i);
      }
    }
  }

  if (selectedRows.isEmpty())
    return;

  for (int row : selectedRows) {
    QTableWidgetItem *item = ui->logTable->item(row, 0);
    if (!item)
      continue;

    LogRecord record;
    record.key = item->data(Qt::UserRole).toString();
    QString logContent = logger->fetch(record);
    if (logContent.isEmpty())
      continue;

#ifdef Q_OS_WASM
    QByteArray fileData = logContent.toUtf8();
    QFileDialog::saveFileContent(fileData, record.key);
#else
    QString downloadDir =
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    QString defaultPath = (!downloadDir.isEmpty())
                              ? downloadDir + "/" + record.key
                              : QDir::homePath() + "/" + record.key;

    QString saveFilePath = QFileDialog::getSaveFileName(
        this, tr("Export Log: %1").arg(record.key), defaultPath,
        tr("File CSV (*.csv);;Tutti i file (*)"));

    if (!saveFilePath.isEmpty()) {
      QFile file(saveFilePath);
      if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << logContent;
        file.close();
      }
    }
#endif
  }
}

void ConfigureExperimentDialog::discardSelectedLog() {
  auto logger = m_experiment->loggingManager();

  QList<QTableWidgetSelectionRange> ranges = ui->logTable->selectedRanges();
  QList<int> selectedRows;
  for (const auto &range : std::as_const(ranges)) {
    for (int i = range.topRow(); i <= range.bottomRow(); ++i) {
      if (!selectedRows.contains(i)) {
        selectedRows.append(i);
      }
    }
  }

  if (selectedRows.isEmpty())
    return;

  std::sort(selectedRows.begin(), selectedRows.end(), std::greater<int>());

  for (int row : selectedRows) {
    QTableWidgetItem *item = ui->logTable->item(row, 0);
    if (!item)
      continue;

    LogRecord record;
    record.key = item->data(Qt::UserRole).toString();

    logger->discard(record);

    ui->logTable->removeRow(row);
  }
}

ConfigureExperimentDialog::~ConfigureExperimentDialog() { delete ui; }

void ConfigureExperimentDialog::onDescriptionTextChanged() {
  m_experiment->description(ui->descriptionTextEdit->document()->toPlainText());
}

void ConfigureExperimentDialog::onChooseScriptButton() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Choose script"), "",
                                                  tr("*.xml *.js"));
  if (fileName != "") {
    ui->scriptNameTextEdit->setText(fileName);
    m_experiment->script(fileName);
    ui->validateScriptButton->setEnabled(
        ui->scriptNameTextEdit->text().trimmed() != "");
  }
}

void ConfigureExperimentDialog::onValidateScriptButton() {
  XMLConfigParser parser;
  try {
    parser.parse(ui->scriptNameTextEdit->text().trimmed(), true);
  } catch (const XMLConfigParserException &e) {
    AsyncMessageBoxFactory::critical(
        this, tr("Validation result"),
        tr("<b>Validation failed</b><br><br>%1").arg(e.what()),
        QMessageBox::Ok);
    return;
  }
  AsyncMessageBoxFactory::information(this, tr("Validation result"),
                                      tr("Script successfully validated"),
                                      QMessageBox::Ok);
}

void ConfigureExperimentDialog::onScriptChanged() {
  m_experiment->script(ui->scriptNameTextEdit->text());
  ui->validateScriptButton->setEnabled(
      ui->scriptNameTextEdit->text().trimmed() != "");
}

void ConfigureExperimentDialog::onStateChanged() {
  if (m_experiment->executionManager()->state() !=
      ExecutionManager::ExecutionState::UNINITIALIZED) {
    ui->chooseScriptButton->setEnabled(false);
    ui->scriptNameTextEdit->setEnabled(false);
    ui->validateScriptButton->setEnabled(false);

  } else {
      ui->chooseScriptButton->setEnabled(true);
      ui->scriptNameTextEdit->setEnabled(true);
      ui->validateScriptButton->setEnabled(ui->scriptNameTextEdit->text().trimmed() != "");
  }
  onUpdateTimestamp();
  refreshLogTable();
  onUpdateState();
}

void ConfigureExperimentDialog::newUser() {
  auto model{dynamic_cast<UserList *>(ui->userTableView->model())};
  model->createUser();
}

void ConfigureExperimentDialog::removeUser() {
  if (ui->userTableView->selectionModel()->selectedIndexes().empty())
    return;
  int idx{ui->userTableView->selectionModel()->selectedIndexes().at(0).row()};
  auto card{m_experiment->credentialManager()->cardByIndex(idx)};
  if (m_experiment->operatorsManager()->containsCard(card)) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(
        this, tr("Confirm delete"),
        tr("User %1 is still connected. "
           "If you delete it the client will be disconnected. "
           "Do you want to continue?")
            .arg(card->username()),
        QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) {
      return;
    }
  }
  auto model{dynamic_cast<UserList *>(ui->userTableView->model())};
  model->removeUser(idx);
}

void ConfigureExperimentDialog::copyConnectionUrl() const {
  if (ui->userTableView->selectionModel()->selectedIndexes().empty())
    return;
  int idx{ui->userTableView->selectionModel()->selectedIndexes().at(0).row()};
  auto card{m_experiment->credentialManager()->cardByIndex(idx)};
  QClipboard *clipboard = QGuiApplication::clipboard();
  auto url{getConnectionUrl(card)};
  clipboard->setText(url.toString());
}

void ConfigureExperimentDialog::userTableContextMenu(QPoint pos) {
  QModelIndex index = ui->userTableView->indexAt(pos);
  if (!index.isValid()) {
    auto menu{new QMenu{this}};
    menu->addAction(ui->actionAdd_user);
    menu->popup(ui->userTableView->viewport()->mapToGlobal(pos));
  } else {
    auto menu{new QMenu{this}};
    menu->addAction(ui->actionAdd_user);
    menu->addAction(ui->actionRemove_user);
    menu->addSeparator();
    menu->addAction(ui->actionCopy_connection_Url_to_clipboard);
    menu->popup(ui->userTableView->viewport()->mapToGlobal(pos));
  }
}

void ConfigureExperimentDialog::filterTableContextMenu(QPoint pos) {
  if (QGuiApplication::queryKeyboardModifiers().testFlag(Qt::ShiftModifier) &&
      QGuiApplication::queryKeyboardModifiers().testFlag(Qt::ControlModifier)) {
    QModelIndex index = ui->filterTableView->indexAt(pos);
    ui->actionToggle_filter_editing->setChecked(
        ui->filterTableView->editTriggers().testFlag(
            QAbstractItemView::DoubleClicked));
    if (!index.isValid()) {
      auto menu{new QMenu{this}};
      menu->addAction(ui->actionAdd_rule);
      menu->popup(ui->filterTableView->viewport()->mapToGlobal(pos));
    } else {
      auto menu{new QMenu{this}};
      menu->addAction(ui->actionAdd_rule);
      menu->addAction(ui->actionRemove_rule);
      menu->addAction(ui->actionToggle_filter_editing);
      menu->popup(ui->filterTableView->viewport()->mapToGlobal(pos));
    }
  }
}

void ConfigureExperimentDialog::onUpdateState() {
  switch (m_experiment->executionManager()->state()) {
  case ExecutionManager::ExecutionState::UNINITIALIZED:
    ui->statusLabel->setText(tr("Status: <b>Stopped</b>"));
    ui->actionStart->setText(tr("Start"));
    ui->actionStart->setIcon(QPixmap(":/images/media-playback-start.png"));
    ui->actionStart->setEnabled(true);
    ui->actionStop->setEnabled(false);
    break;
  case ExecutionManager::ExecutionState::PAUSED:
    ui->statusLabel->setText(tr("Status: <b>Paused</b>"));
    ui->actionStart->setText(tr("Resume"));
    ui->actionStart->setIcon(QPixmap(":/images/media-playback-start.png"));
    ui->actionStart->setEnabled(true);
    ui->actionStop->setEnabled(true);
    break;
  case ExecutionManager::ExecutionState::RUNNING:
    ui->statusLabel->setText(tr("Status: <b>Running</b>"));
    ui->actionStart->setText(tr("Pause"));
    ui->actionStart->setIcon(QPixmap(":/images/media-playback-pause.png"));
    ui->actionStart->setEnabled(true);
    ui->actionStop->setEnabled(true);
    break;
  }
}

void ConfigureExperimentDialog::onUpdateTimestamp() {
  ui->timestampLabel->setText(
      tr("Time: %1")
          .arg(QTime(0, 0)
                   .addSecs(static_cast<int>(
                       m_experiment->executionManager()->timestamp() / 1000))
                   .toString("hh:mm:ss")));
}

void ConfigureExperimentDialog::newFilterRule() {
  auto model{dynamic_cast<FilterList *>(ui->filterTableView->model())};
  model->add();
}

void ConfigureExperimentDialog::toggleFilterEditTriggers() {
  if (ui->filterTableView->editTriggers().testFlag(
          QAbstractItemView::DoubleClicked)) {
    ui->filterTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  } else {
    ui->filterTableView->setEditTriggers(QAbstractItemView::DoubleClicked);
  }
}

void ConfigureExperimentDialog::removeFilterRule() {
  auto model{dynamic_cast<FilterList *>(ui->filterTableView->model())};
  if (ui->filterTableView->selectionModel()->selectedIndexes().size() == 0)
    return;
  int idx{ui->filterTableView->selectionModel()->selectedIndexes().at(0).row()};
  model->remove(idx);
}

void ConfigureExperimentDialog::onUpdateAvailability() {
  m_experiment->enabled(ui->availableCheckBox->isChecked());
}

void ConfigureExperimentDialog::onSimSpeedChanged(double factor) {
  m_experiment->executionManager()->speed(factor);
}

void ConfigureExperimentDialog::dropEvent(QDropEvent *event) {
  auto mime{event->mimeData()};
  if (mime) {
    if (mime->hasFormat("text/xml")) {
      XMLConfigParser cparser{m_experiment, m_experiment->scheduler(),
                              m_experiment->simulator()->registry()};
      try {
        auto code{mime->data("text/xml")};
        cparser.parseString(code);
      } catch (const XMLConfigParserException &e) {
        AsyncMessageBoxFactory::critical(
            this, tr("Error"),
            tr("<b>Configuration injection failed:</b><br><br>%1")
                .arg(e.what()),
            QMessageBox::Ok);
        return;
      }
    }
  }
}

void ConfigureExperimentDialog::dragEnterEvent(QDragEnterEvent *event) {
  event->setAccepted(m_experiment->executionManager()->state() !=
                         ExecutionManager::ExecutionState::UNINITIALIZED &&
                     event->mimeData()->hasFormat("text/xml"));
}

void ConfigureExperimentDialog::updateUserDetails() {
  if (!ui->userTableView->selectionModel()->hasSelection()) {
    clearUserDetails();
    return;
  }
  int idx{ui->userTableView->selectionModel()->selectedIndexes().at(0).row()};
  auto card{m_experiment->credentialManager()->cardByIndex(idx)};
  auto url{getConnectionUrl(card)};
  QSize size{128, 128};
  QPixmap qr{generateQrPixmap(url.toString(), &size, {128, 128})};
  ui->qrcodeButton->setEnabled(true);
  ui->qrcodeButton->setIcon(qr);
  ui->qrcodeButton->setIconSize(QSize{128, 128});
  ui->aliasLabel->setText(
      QString{tr("<a href=\"%1\">%2 (right-click to copy connection Url)</a>")}
          .arg(url.toString(), card->alias()));
}

void ConfigureExperimentDialog::clearUserDetails() {
  ui->qrcodeButton->setIcon({});
  ui->qrcodeButton->setEnabled(false);
  ui->aliasLabel->setText("");
}
