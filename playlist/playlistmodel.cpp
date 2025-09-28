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
#include "playlistmodel.h"
#include "extensions.h"
#include "helper.h"

#include <QMimeData>
#include <QIODevice>
#include <QFileInfo>

#include <algorithm>
#include <random>

PlaylistModel::PlaylistModel(QObject *parent) : QAbstractTableModel{parent} {
    columnNames = {tr("Name"), tr("Duration") };
}

bool PlaylistModel::addURLs(const QList<QUrl> &urls, int row)
{
    QList<PlaylistItem> items;
    for (auto &url : urls)
        items.emplaceBack(url);
    return addItems(items, row);
}

void PlaylistModel::shuffle()
{
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(values.begin(), values.end(), g);
    emit dataChanged(index(0, COL_LABEL), index(rowCount(), COL_MAX-1), QList<int>{Qt::DisplayRole, CurrentRole});
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    return values.count();
}

int PlaylistModel::columnCount(const QModelIndex &parent) const
{
    return columnNames.count();
}

QVariant PlaylistModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return columnNames.at(section);

    return QVariant();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    const PlaylistItem &item = values.at(index.row());
    switch (role)
    {
    case Qt::DisplayRole:
    {
        switch(index.column())
        {
        case COL_LABEL:
            return item.label;
        case COL_DURATION:
            return Helper::formatTime(item.duration);
        }
    }
    case Qt::ToolTipRole:
        if (index.column()==COL_LABEL)
            return item.label;
        return QVariant();
    case PlaylistModel::CurrentRole:
        return current.isValid() && index.isValid() && current.row() == index.row();
    case PlaylistModel::URLRole:
        return item.path;
    }
    return QVariant();
}

QStringList PlaylistModel::mimeTypes() const
{
    QStringList types;
    types << "application/x-kokovpplaylistelement" << "text/uri-list";
    return types;
}

QMimeData *PlaylistModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData;
    QByteArray encodedData;

    QList<int> rows;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    for (const QModelIndex &index : indexes) {
        if (index.isValid() && !rows.contains(index.row())) {
            int row = index.row();
            bool isCurrent = (row==current.row());
            rows.append(row);
            stream << row << values.at(row) << isCurrent;
        }
    }

    mimeData->setData("application/x-kokovpplaylistelement", encodedData);
    return mimeData;
}

bool PlaylistModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(action);
    Q_UNUSED(row);
    Q_UNUSED(parent);

    if (data->hasUrls() && action==Qt::CopyAction)
    {
        for (auto &url : data->urls())
        {
            if (!Extensions.multimedia().contains(QFileInfo(url.fileName()).suffix()))
                return false;
        }
        return true;
    }

    if (data->hasFormat("application/x-kokovpplaylistelement"))
        return true;

    return false;
}

bool PlaylistModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (!canDropMimeData(data, action, row, column, parent))
        return false;

    if (action == Qt::IgnoreAction)
        return true;

    int beginRow;

    if (row != -1)
        beginRow = row;
    else if (parent.isValid())
        beginRow = parent.row();
    else
        beginRow = rowCount(QModelIndex());


    QList<QUrl> urls = data->urls();
    if (urls.count()>0)
        return addURLs(urls, beginRow);

    QByteArray encodedData = data->data("application/x-kokovpplaylistelement");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QList<QPersistentModelIndex> removeRows;
    QList<PlaylistItem> movingItems;
    QPersistentModelIndex beforeRow = index(beginRow, 0);

    int relCurrent = -1;
    int relCurrentCnt = 0;
    while (!stream.atEnd())
    {
        int row;
        PlaylistItem item;
        bool isCurrent;
        stream >> row >> item >> isCurrent;
        if (isCurrent)
            relCurrent = relCurrentCnt;
        relCurrentCnt++;
        removeRows.append(index(row,0));
        movingItems.append(item);
    }

    for (auto &rRow : removeRows)
        removeRow(rRow.row());

    // Because removeRow calls invalidates beginRow we need to save it via QPersistentModelIndex
    if (movingItems.count()>0)
    {
        int row = beforeRow.row();
        if (row<0 || row > rowCount(QModelIndex()))
            row = rowCount();

        bool res = addItems(movingItems, row);
        if (relCurrent>=0)
            setRowCurrent(row+relCurrent);
        return res;
    }

    return false;
}

Qt::DropActions PlaylistModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

bool PlaylistModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (count<=0 || row<0)
        return false;

    if (row+count>values.count())
        count = values.count()-row;

    beginRemoveRows(QModelIndex(), row, row+count-1);

    if (count==values.count())
        values.clear();
    else
        values.erase(values.constBegin() + row, values.constBegin()+row+count);

    endRemoveRows();

    if (rowCount()==0)
        current = QModelIndex();
    return true;
}

void PlaylistModel::setRowCurrent(int row)
{
    QModelIndex prevCurrent = current;
    current = index(row, 0);
    emit dataChanged(prevCurrent, prevCurrent, QList<int>{CurrentRole});
    emit dataChanged(current, current, QList<int>{CurrentRole});
}

void PlaylistModel::setCurrentRowMetainfo(QString label, double duration)
{
    if (currentRow()<0)
        return;
    PlaylistItem &i = values[currentRow()];
    i.label = label;
    i.duration = duration;
    emit dataChanged(index(currentRow(), COL_LABEL), index(currentRow(), COL_DURATION), QList<int>{Qt::DisplayRole});
}

Qt::ItemFlags PlaylistModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);

    if (index.isValid())
        return Qt::ItemIsDragEnabled | defaultFlags;
    else
        return Qt::ItemIsDropEnabled | defaultFlags;
}

bool PlaylistModel::addItems(const QList<PlaylistItem> &items, int row)
{
    if (items.count()==0)
        return false;

    if (row<0 || row > rowCount(QModelIndex()))
        row = rowCount();

    int count = items.count();

    beginInsertRows(QModelIndex(), row, row+count-1);
    for (auto &item : items)
        values.insert(row++, item);
    endInsertRows();
    return true;
}
