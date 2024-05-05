#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QUrl>
#include <QAbstractTableModel>

struct PlaylistItem
{
    PlaylistItem(QUrl path = QUrl())
        : path(std::move(path)) {
        label = this->path.fileName(); // TODO: this is the quckfix to provide at least some labels, later labels reading from mpv should be done
    }

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
    bool addURLs(const QList<QUrl> &urls, int row=-1);

    enum Columns {COL_LABEL, COL_DURATION, COL_MAX};
    enum Roles {CurrentRole = Qt::UserRole +1, URLRole};

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    int currentRow() { return current.row(); }
    void setRowCurrent(int row);

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
    bool addItems(int row, const QList<PlaylistItem> &items);
    QList<PlaylistItem> values;
    QPersistentModelIndex current;
    QStringList columnNames;
};

#endif // PLAYLISTMODEL_H
