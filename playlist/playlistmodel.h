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
#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QUrl>
#include <QAbstractTableModel>

struct PlaylistItem
{
    PlaylistItem(QUrl path = QUrl())
        : path(std::move(path)) {
        label = this->path.fileName(); // Using filename as lablel, when file is loaded, mpv may provide human-readable video's name
    }

    PlaylistItem(QUrl path, QString label, double duration)
        : path(std::move(path)), label(std::move(label)), duration(duration) {}

    QUrl path;
    QString label;
    double duration=0;

    friend QDataStream &operator<<(QDataStream &out, const PlaylistItem &rhs) {
        out << rhs.path;
        out << rhs.label;
        out << rhs.duration;
        return out;
    }
    friend QDataStream &operator>>(QDataStream &in, PlaylistItem &rhs){
        in >> rhs.path;
        in >> rhs.label;
        in >> rhs.duration;
        return in;
    }
};

class PlaylistModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    PlaylistModel(QObject *parent = nullptr);
    bool addItems(const QList<PlaylistItem> &items, int row = -1);
    bool addURLs(const QList<QUrl> &urls, int row=-1);
    const QList<PlaylistItem> &items() { return values; }
    void shuffle();

    enum Columns {COL_LABEL, COL_DURATION, COL_MAX};
    enum Roles {CurrentRole = Qt::UserRole +1, URLRole};

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    int currentRow() { return current.row(); }
    void setRowCurrent(int row);
    void setCurrentRowMetainfo(QString label, double duration);

    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent = QModelIndex()) override;
    Qt::DropActions supportedDropActions() const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    friend QDataStream &operator<<(QDataStream &out, const PlaylistModel &rhs) {
        out << rhs.values;
        out << rhs.current.row();
        return out;
    }
    friend QDataStream &operator>>(QDataStream &in, PlaylistModel &rhs){
        int curRow;
        in >> rhs.values;
        in >> curRow;
        rhs.current = rhs.index(curRow, 0);
        return in;
    }
private:
    QList<PlaylistItem> values;
    QPersistentModelIndex current;
    QStringList columnNames;
};

#endif // PLAYLISTMODEL_H
