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
namespace Amber { class MprisPlayer; }

using QActionMap = QMap<QString, QAction*>;

class KokoVP : public QMainWindow
{
    Q_OBJECT

public:
    KokoVP(QWidget *parent = nullptr);
    ~KokoVP();
    static KokoVP *i() { return inst; }
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
    void openDirectory();
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

    Amber::MprisPlayer *mpris;

    static KokoVP *inst;
};

#endif // KOKOVP_H
