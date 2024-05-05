#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QDockWidget>

class PlaylistModel;
class QTableView;
class QToolBar;

class Playlist : public QWidget
{
    Q_OBJECT
public:
    explicit Playlist(QWidget *parent = nullptr);
    ~Playlist();
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
    QTableView *plView;
    PlaylistModel *plModel;
    QToolBar *bottomBar;
    QMenu *contextMenu;

    void addFiles();
    void addDirectory();
    void delSelected();
    void playRow(int row);
};

#endif // PLAYLIST_H
