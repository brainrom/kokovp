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

#ifndef SHORTCUTEDITORDELEGATE_H
#define SHORTCUTEDITORDELEGATE_H

#include <QStyledItemDelegate>

class QKeySequenceEdit;
class ClearableShortcutEditor : public QWidget
{
    Q_OBJECT
public:
    ClearableShortcutEditor(QWidget *parent);
    QKeySequenceEdit *editor() { return p_editor; };
signals:
    void editingFinished();
private:
    QKeySequenceEdit *p_editor;
};


class ShortcutEditorDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit ShortcutEditorDelegate(QObject *parent = nullptr) {};
    ~ShortcutEditorDelegate() override = default;

protected:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    void commitAndCloseEditor();
};

#endif
