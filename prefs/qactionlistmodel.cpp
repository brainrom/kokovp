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
#include "qactionlistmodel.h"

#include <QMimeData>
#include <QIODevice>
#include <QFileInfo>

#include <QAction>

QActionListModel::QActionListModel(QObject *parent) : QAbstractTableModel{parent} {
    columnNames = {tr("Shortcut"), tr("Name") };
}

QActionListModel::QActionListModel(QList<QAction *> list, QObject *parent) : QActionListModel{parent}
{
    values = list;
}

void QActionListModel::setQActionList(QList<QAction *> list)
{
    beginResetModel();
    values = list;
    endResetModel();
}

int QActionListModel::rowCount(const QModelIndex &parent) const
{
    return values.count();
}

int QActionListModel::columnCount(const QModelIndex &parent) const
{
    return columnNames.count();
}

QVariant QActionListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return columnNames.at(section);

    return QVariant();
}

QVariant QActionListModel::data(const QModelIndex &index, int role) const
{
    const QAction *item = values.at(index.row());
    switch (role)
    {
    case Qt::DisplayRole:
    {
        switch(index.column())
        {
        case COL_LABEL:
            return item->text();
        case COL_SHORTCUT:
            return item->shortcut().toString();
        }
    }
    case Qt::DecorationRole:
        if (index.column()==COL_LABEL)
            return item->icon();
    case Qt::EditRole:
        if (index.column()==COL_SHORTCUT)
            return item->shortcut();

    }
    return QVariant();
}

bool QActionListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.column()!=COL_SHORTCUT)
        return false;

    QAction *item = values.at(index.row());
    item->setShortcut(value.toString());
    emit dataChanged(index, index);
    return true;
}

Qt::ItemFlags QActionListModel::flags(const QModelIndex &index) const {
    if (index.column() == COL_SHORTCUT) {
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }
    return QAbstractItemModel::flags(index);
}
