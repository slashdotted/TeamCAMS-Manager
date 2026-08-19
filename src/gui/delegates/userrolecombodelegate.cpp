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
#include "userrolecombodelegate.h"
#include "experiment/operators/access/accesscard.h"
#include <QComboBox>

UserRoleComboDelegate::UserRoleComboDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

QWidget *UserRoleComboDelegate::createEditor(QWidget *parent,
                                             const QStyleOptionViewItem &option,
                                             const QModelIndex &index) const {
  Q_UNUSED(option);
  Q_UNUSED(index);
  QComboBox *combo{new QComboBox(parent)};
  for (int i{0}; i <= static_cast<int>(AccessCard::AccessType::DISABLED); i++) {
    combo->addItem(AccessCard::accessTypeName(i));
  }
  connect(combo, SIGNAL(currentIndexChanged(int)), this,
          SLOT(onCurrentIndexChanged(int)));
  return combo;
}

void UserRoleComboDelegate::setEditorData(QWidget *editor,
                                          const QModelIndex &index) const {
  auto comboBox{dynamic_cast<QComboBox *>(editor)};
  QString value = index.model()->data(index, Qt::EditRole).toString();
  comboBox->setCurrentText(value);
}

void UserRoleComboDelegate::setModelData(QWidget *editor,
                                         QAbstractItemModel *model,
                                         const QModelIndex &index) const {
  auto comboBox{dynamic_cast<QComboBox *>(editor)};
  model->setData(index, comboBox->currentText(), Qt::EditRole);
}

void UserRoleComboDelegate::updateEditorGeometry(
    QWidget *editor, const QStyleOptionViewItem &option,
    const QModelIndex & /* index */) const {
  editor->setGeometry(option.rect);
}

void UserRoleComboDelegate::onCurrentIndexChanged(int index) {
  Q_UNUSED(index)
  auto cb{dynamic_cast<QComboBox *>(sender())};
  emit commitData(cb);
}
