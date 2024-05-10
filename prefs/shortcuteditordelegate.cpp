/*  This is part of KokoVP

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
// Copyright (C) 2022 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "shortcuteditordelegate.h"

#include <QAbstractItemModel>
#include <QKeySequenceEdit>
#include <QPushButton>
#include <QBoxLayout>

QWidget *ShortcutEditorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const
{
    QKeySequenceEdit *editor = new QKeySequenceEdit(parent);
    editor->setClearButtonEnabled(true);
    editor->setMaximumSequenceLength(1);
    connect(editor, &QKeySequenceEdit::editingFinished, this, &ShortcutEditorDelegate::commitAndCloseEditor);
    return editor;
}

void ShortcutEditorDelegate::commitAndCloseEditor()
{
    QKeySequenceEdit *editor = static_cast<QKeySequenceEdit *>(sender());
    Q_EMIT commitData(editor);
    Q_EMIT closeEditor(editor);
}

void ShortcutEditorDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (!editor || !index.isValid())
        return;

    QString value = index.model()->data(index, Qt::EditRole).toString();

    QKeySequenceEdit *keySequenceEdit = static_cast<QKeySequenceEdit *>(editor);
    keySequenceEdit->setKeySequence(value);
}

void ShortcutEditorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (!editor || !model || !index.isValid())
        return;

    const QKeySequenceEdit *keySequenceEdit = static_cast<QKeySequenceEdit *>(editor);
    const QKeySequence keySequence = keySequenceEdit->keySequence();
    QString keySequenceString = keySequence.toString(QKeySequence::NativeText);
    model->setData(index, keySequenceString);
}

void ShortcutEditorDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}
