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
#include "feedbackonupdatedelegate.h"
#include <QApplication>
#include <QCheckBox>

QWidget *
FeedbackOnUpdateDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const {
  Q_UNUSED(option)
  Q_UNUSED(index)
  return new QCheckBox{parent};
}

void FeedbackOnUpdateDelegate::setEditorData(QWidget *editor,
                                             const QModelIndex &index) const {
  QCheckBox *cb = qobject_cast<QCheckBox *>(editor);
  cb->setChecked(index.data().toBool());
}

void FeedbackOnUpdateDelegate::setModelData(QWidget *editor,
                                            QAbstractItemModel *model,
                                            const QModelIndex &index) const {
  auto *cb{static_cast<QCheckBox *>(editor)};
  model->setData(index, cb->checkState() == Qt::Checked, Qt::EditRole);
}

void FeedbackOnUpdateDelegate::paint(QPainter *painter,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const {
  auto data{index.model()->data(index, Qt::DisplayRole).toBool()};
  QStyleOptionButton styleoption;
  auto rect{QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator,
                                                  &styleoption)};
  styleoption.rect = option.rect;
  styleoption.rect.setLeft(option.rect.x() + option.rect.width() / 2 -
                           rect.width() / 2);
  styleoption.state = data ? QStyle::State_On | QStyle::State_Enabled
                           : QStyle::State_Off | QStyle::State_Enabled;
  QApplication::style()->drawControl(QStyle::CE_CheckBox, &styleoption,
                                     painter);
}

void FeedbackOnUpdateDelegate::updateEditorGeometry(
    QWidget *editor, const QStyleOptionViewItem &option,
    const QModelIndex &index) const {
  Q_UNUSED(index);
  QStyleOptionButton styleoption;
  auto rect{QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator,
                                                  &styleoption)};
  styleoption.rect = option.rect;
  styleoption.rect.setLeft(option.rect.x() + option.rect.width() / 2 -
                           rect.width() / 2);
  editor->setGeometry(styleoption.rect);
}
