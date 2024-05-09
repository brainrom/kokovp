#ifndef KOKOVP_H
#define KOKOVP_H

#include <QMainWindow>

class SeekInterface;
class PlayerController;
class PlayerWidget;
class PrefDialog;
class Playlist;
class TracksMenu;
class AutohideWidget;
class FileSettingsHash;
class QTableView;

using QActionMap = QMap<QString, QAction*>;

class KokoVP : public QMainWindow
{
    Q_OBJECT

public:
    KokoVP(QWidget *parent = nullptr);
    ~KokoVP();
    static KokoVP *i() { return inst; }
    static const QList<QUrl> pathsToUrls(const QStringList &paths);
    void addURLs(const QList<QUrl> &urls);
    QActionMap actionsMap() const { return p_actionsMap; }
    void handleNewMessage(QString msg);
private:
    void toggleFullscreen(bool on);

    TracksMenu *bindTracksMenu(const QIcon &icon, const QString &title, const QString &prop_name, QMenu *parentMenu);
    void populateMenu();
    void createPlaylistDock();
    void readConfig();
    void insertActionsMap(QAction *action);

    // Audio devices
    void populateAudioDeviceMenu();
    void setAudioDevice(QAction *audioDeviceAction);

    void tryPlayCurrent();
    void videoScreenshot();
    void videoSubScreenshot();
    void openFiles();
    void playFile(QUrl file);
    void handleTracks();
    void handleEOF(bool wasStopped);
    void callPropEditor(QAction *callEditorAction);

    void actionEvent(QActionEvent *event) override;

    PlayerWidget *playerWidget;
    PlayerController *player;
    SeekInterface *seek;
    PrefDialog *pref;
    FileSettingsHash *fileSettings;

    QDockWidget *playlistDock;
    Playlist *playlist;

    QMenuBar *menubar;
    QMenu *popup;
    QToolBar *bottomBar;
    AutohideWidget *autoHide;

    QMenu *openMenu;
    QMenu *playMenu;
    QMenu *videoMenu;
    QMenu *audioMenu;
    QMenu *subtitlesMenu;
    //QMenu *browseMenu;
    QMenu *miscMenu;
    QMenu *helpMenu;

    TracksMenu *videoTracksMenu = nullptr;
    TracksMenu *audioTracksMenu = nullptr;
    TracksMenu *subTracksMenu = nullptr;
    TracksMenu *secondSubTracksMenu = nullptr;

    QActionMap p_actionsMap;

    static KokoVP *inst;
};

#endif // KOKOVP_H
