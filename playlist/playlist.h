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
#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QUrl>
#include <QDockWidget>

class PlaylistModel;
class QTableView;
class QToolBar;
struct PlaylistItem;

class Playlist : public QWidget
{
    Q_OBJECT
public:
    explicit Playlist(QWidget *parent = nullptr);
    ~Playlist();
    void setCurrentRowMetainfo(QString label, double duration);
    bool addURLs(const QList<QUrl> &urls);
    void prev();
    void next();

    void clear();
    bool isEmpty();

    void playFirst();
    void playLast();
    void playCurrent();
signals:
    void playRequest(QUrl path);

private:
    typedef const QList<PlaylistItem> (*LoadPlaylistFunc)(const QString &file);
    typedef bool (*SavePlaylistFunc)(const QString &file, const QList<PlaylistItem> &list);
    QMap<QString, QPair<LoadPlaylistFunc, SavePlaylistFunc>> codecFuncs;

    QTableView *plView;
    PlaylistModel *plModel;
    QToolBar *bottomBar;
    QMenu *contextMenu;

    void loadPlaylist();

    bool savePlaylist() { return savePlaylist(QString()); }
    bool savePlaylist(QString s);

    void addFiles();
    void addDirectory();
    void delSelected();
    void playRow(int row);
};

#endif // PLAYLIST_H
