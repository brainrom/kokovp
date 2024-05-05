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
