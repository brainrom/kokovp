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
#include "kokovp.h"
#include "playercontroller.h"
#include "playerwidget.h"
#include "playlist/playlist.h"

#include <QtWidgets>
#include <QToolBar>
#include <QStandardPaths>

#include "autohidewidget.h"

#include "actions/incdecactions.h"
#include "actions/bistableaction.h"
#include "actions/actionwrapper.h"
#include "actions/timelabelaction.h"
#include "actions/volumeslideraction.h"
#include "actions/timeslideraction.h"
#include "actions/seekinterface.h"
#include "actions/incdecwheelaction.h"

#include "tracksmenu.h"
#include "config.h"
#include "cache.h"
#include "helper.h"

#include "prefs/prefdialog.h"
#include "persistency/filesettingshash.h"

// TODO: maybe move to some class. Anyway it must be static
QString extfolderRewriteRule(const PlayerController::Track &t)
{
    if (!t.isExternal)
        return QString();

    QFileInfo fI(t.filename);
    QString fName = fI.fileName();

    if (fName==t.title)
        return QApplication::translate("KokoVP", "[EXT] ")+fI.dir().dirName();

    return QString();
}

KokoVP* KokoVP::inst = nullptr;

KokoVP::KokoVP(QWidget *parent)
    : QMainWindow(parent)
{
    assert(!inst);
    inst = this;

    playerWidget = new PlayerWidget(this);
    player = new PlayerController(playerWidget);
    pref = new PrefDialog();
    connect(pref, &PrefDialog::applied, this, &KokoVP::readConfig);
    fileSettings = new FileSettingsHash(QStandardPaths::writableLocation(QStandardPaths::CacheLocation), player, this);

    setCentralWidget(playerWidget);
    createPlaylistDock();

    connect(player, &PlayerController::tracksUpdated, this, &KokoVP::handleTracks);
    connect(player, &PlayerController::fileMetaUpdated, playlist, &Playlist::setCurrentRowMetainfo);
    connect(player, &PlayerController::endFile, this, &KokoVP::handleEOF);
    connect(playerWidget, &PlayerWidget::draggedURLS, playlist, &Playlist::addURLs);
    connect(playlist, &Playlist::playRequest, this, qOverload<QUrl>(&KokoVP::playFile));

    menubar = new QMenuBar(this); // Open Play Video Audio Subtitles Misc Help
    setMenuBar(menubar);

    openMenu = menubar->addMenu(tr("Open"));
    playMenu = menubar->addMenu(tr("Play"));
    videoMenu = menubar->addMenu(tr("Video"));
    audioMenu = menubar->addMenu(tr("Audio"));
    subtitlesMenu = menubar->addMenu(tr("Subtitles"));
    // TODO: browse section? Or in Play?
    miscMenu = menubar->addMenu(tr("Misc"));
    helpMenu = menubar->addMenu(tr("Help"));

    // TODO: qt.qpa.wayland: Creating a popup with a parent, QWidgetWindow(0x555555d967f0, name="KokoVPClassWindow") which does not match the current topmost grabbing
    // Needs separate menus?...
    popup = new QMenu(this);
    popup->addMenu(playMenu);
    popup->addMenu(videoMenu);
    popup->addMenu(audioMenu);
    popup->addMenu(subtitlesMenu);

    ActionWrapper *showPopupAct = new ActionWrapper(tr("Show context menu"), QKeySequence(), this, "show_context_menu");
    connect(showPopupAct, &QAction::triggered, this, [this](){ popup->popup(QCursor::pos()); });

    populateMenu();

    // Widget actions
    autoHide = new AutohideWidget(playerWidget);
    autoHide->setAnimated(true);
    autoHide->setAutoHide(true);

    TimeLabelAction *tlAct = new TimeLabelAction(TimeLabelAction::CurrentAndTotalTime, this);
    tlAct->setObjectName("time_label");
    connect(player->prop("duration"), &PropertyObserver::changedDouble, tlAct, &TimeLabelAction::setTotalTime);
    connect(player->prop("time-pos"), &PropertyObserver::changedDouble, tlAct, &TimeLabelAction::setCurrentTime);
    //Shouldn't use addAction, because these actions shouldn't appear in actions() list, since they aren't triggerable, but should be in actionsMap for bars populating
    insertActionsMap(tlAct);

    VolumeSliderAction *vsAct = new VolumeSliderAction(this);
    vsAct->setObjectName("volume_slider");
    connect(player->prop("volume"), &PropertyObserver::changedDouble, vsAct, &VolumeSliderAction::setValue);
    connect(vsAct, &VolumeSliderAction::valueChanged, player->prop("volume"), &PropertyObserver::set);
    insertActionsMap(vsAct);

    TimeSliderAction *tsAct = new TimeSliderAction(this);
    tsAct->setObjectName("time_slider");
    tsAct->setWheelDelta(Config::i().get("steps/mouse_bar_seek", 10).toInt());
    connect(player->prop("duration"), &PropertyObserver::changedDouble, tsAct, &TimeSliderAction::setDuration);
    connect(player->prop("time-pos"), &PropertyObserver::changedDouble, tsAct, &TimeSliderAction::setTime);
    connect(tsAct, &TimeSliderAction::timeChanged, player->prop("time-pos"), &PropertyObserver::set);
    insertActionsMap(tsAct);

    // TOOLBARS.
    QStringList barActionsStr = Config::i().get("actions/bar_list", QStringList{"play_pause", "stop", "time_slider", "mute", "volume_slider", "time_label"}).toStringList();
    QList<QAction*> barActions;
    for (auto &str: barActionsStr)
        barActions.append(p_actionsMap.value(str));

    QToolBar *hideBar = new QToolBar(this);
    hideBar->addActions(barActions);
    autoHide->setInternalWidget(hideBar);
    autoHide->hide();

    bottomBar = new QToolBar(this);
    bottomBar->addActions(barActions);
    addToolBar(Qt::BottomToolBarArea, bottomBar);

    // CONFIGURATION
    readConfig();
}

KokoVP::~KokoVP()
{
    fileSettings->saveSettingsFor(player->lastOpenFile(), true); // Always save time-pos on exit
}

void KokoVP::handleNewMessage(QString msg)
{
    int del = msg.indexOf(':');
    QString cmd = msg.left(del);
    QStringList args = msg.trimmed().mid(del+1).split(',');
    if (cmd=="open")
        playlist->addURLs(Helper::pathsToUrls(args));
    else if (cmd=="playlast")
        QTimer::singleShot(100, playlist, &Playlist::playLast); // Workaroun to wait until Qt event loop and libmpv will be ready
}

void KokoVP::toggleFullscreen(bool on)
{
    if (on)
    {
        showFullScreen();
        menuBar()->hide();
        bottomBar->hide();
        autoHide->activate();
        playerWidget->setAllowHideCursor(true);
        playlistDock->setVisible(false);
    }
    else
    {
        showNormal();
        menuBar()->show();
        bottomBar->show();
        autoHide->deactivate();
        playerWidget->setAllowHideCursor(false);
        playlistDock->setVisible(Cache::i().get("gui_playlist/show").toBool());
    }
}

TracksMenu *KokoVP::bindTracksMenu(const QIcon &icon, const QString &title, const QString &prop_name, QMenu *parentMenu)
{
    TracksMenu *newTracksMenu = new TracksMenu(title, parentMenu);
    newTracksMenu->setIcon(icon);
    parentMenu->addMenu(newTracksMenu);

    PropertyObserver *propObs = player->prop(prop_name);
    connect(newTracksMenu,  &TracksMenu::trackSwitched, propObs, &PropertyObserver::set);
    connect(propObs, &PropertyObserver::changedInt, newTracksMenu, &TracksMenu::setCurrentId);
    return newTracksMenu;
}

void KokoVP::populateMenu()
{
    //TODO: The Great And Ugly Toolbar
    QActionGroup *callPropEditorAg = new QActionGroup(this);
    connect(callPropEditorAg, &QActionGroup::triggered, this, &KokoVP::callPropEditor);

    // --- Open ---
    // TODO: maybe add recent, favs?
    ActionWrapper *openFileAct = new ActionWrapper(tr("Files..."), QKeySequence("Ctrl+F"), openMenu, "openFile", QIcon(":/icons/default/open.png"));
    connect(openFileAct, &QAction::triggered, this, qOverload<>(&KokoVP::openFiles));

    ActionWrapper *openDirectoryAct = new ActionWrapper(tr("Directory..."), QKeySequence(), openMenu, "openDirectory", QIcon(":/icons/default/openfolder.png"));
    connect(openDirectoryAct, &QAction::triggered, this, qOverload<>(&KokoVP::openDirectory));

    ActionWrapper *exitAct = new ActionWrapper(tr("Exit"), QKeySequence("Ctrl+Q"), openMenu, "exit", QIcon(":/icons/default/exit.png"));
    connect(exitAct, &QAction::triggered, qApp, &QApplication::exit);

    // --- Play ---
    BistableAction *playPauseAct = new BistableAction(Qt::Key_Space, playMenu, "play_pause");
    playPauseAct->setPassiveState(tr("Pause"), QIcon(":/icons/default/pause.png"));
    playPauseAct->setActiveState(tr("Play"), QIcon(":/icons/default/play.png"));
    connect(player->prop("pause"), &PropertyObserver::changedBool, playPauseAct, &BistableAction::switchState);
    connect(playPauseAct, &QAction::triggered, player, &PlayerController::togglePlayback);
    connect(playPauseAct, &QAction::triggered, this, &KokoVP::tryPlayCurrent);

    ActionWrapper *stopAct = new ActionWrapper(tr("Stop"), Qt::Key_X, playMenu, "stop", QIcon(":/icons/default/stop.png"));
    connect(stopAct, &QAction::triggered, player, &PlayerController::stop);

    IncDecWheelAction *framesActions = new IncDecWheelAction("Frame", playMenu);
    connect(framesActions, &IncDecWheelAction::valueChanged, player, &PlayerController::frameStep);
    framesActions->setNamePrefix("frame");
    framesActions->setLabelTemplate(tr("%1 frame"));
    framesActions->setDecOptions(tr("Previous"), Qt::Key_Comma);
    framesActions->setIncOptions(tr("Next"), Qt::Key_Period);

    seek = new SeekInterface(playMenu);
    connect(seek, &SeekInterface::seek, player, &PlayerController::seekRelative);

    IncDecActionsPair *chaptersActions = new IncDecActionsPair(this);
    connect(chaptersActions, &IncDecActionsPair::valueChanged, player->prop("chapter"), &PropertyObserver::setRelative);
    chaptersActions->setDecAction(new ActionWrapper(tr("Previous chapter"), Qt::Key_Exclam, playMenu, "prev_chapter"));
    chaptersActions->setIncAction(new ActionWrapper(tr("Next chapter"), Qt::Key_At, playMenu, "next_chapter"));

    ActionWrapper *prevAct = new ActionWrapper(tr("Previous"), QKeySequence("Ctrl+,"), playMenu, "prev", QIcon(":/icons/default/previous"));
    connect(prevAct, &QAction::triggered, playlist, &Playlist::prev);
    ActionWrapper *nextAct = new ActionWrapper(tr("Next"), QKeySequence("Ctrl+."), playMenu, "next", QIcon(":/icons/default/next"));
    connect(nextAct, &QAction::triggered, playlist, &Playlist::next);

    // --- Video ---
    ActionWrapper *fullscreenAct = new ActionWrapper(tr("Fullscreen"), Qt::Key_F, videoMenu, "fullscreen", QIcon(":/icons/default/fullscreen"));
    fullscreenAct->setCheckable(true);
    connect(fullscreenAct, &QAction::toggled, this, &KokoVP::toggleFullscreen);

    videoTracksMenu = bindTracksMenu(QIcon(":/icons/default/video_track"), tr("Track"), "vid", videoMenu);

    ActionWrapper *screenshotWithNoSubsAct = new ActionWrapper(tr("Screenshot without subtitles"), Qt::Key_S, videoMenu, "screenshot_without_subtitles", QIcon(":/icons/default/screenshot.png"));
    connect(screenshotWithNoSubsAct, &QAction::triggered, this, &KokoVP::videoScreenshot);

    ActionWrapper *screenshotWithSubsAct = new ActionWrapper(tr("Screenshot with subtitles"), QKeySequence("Shift+S"), videoMenu, "screenshot_with_subtitles", QIcon(":/icons/default/screenshot.png"));
    connect(screenshotWithSubsAct, &QAction::triggered, this, &KokoVP::videoSubScreenshot);

    // --- Audio ---
    // TODO: add external
    audioTracksMenu = bindTracksMenu(QIcon(":/icons/default/audio_track"), tr("Track"), "aid", audioMenu);
    audioTracksMenu->setRewriteRule(extfolderRewriteRule);

    populateAudioDeviceMenu();

    IncDecActionsPair *volumeActions = new IncDecActionsPair(this);
    volumeActions->setDelta(Config::i().get("steps/volume_acts", 5).toDouble());
    connect(volumeActions, &IncDecActionsPair::valueChanged, player->prop("volume"), &PropertyObserver::setRelative);
    volumeActions->setIncAction(new ActionWrapper(tr("Volume +"), Qt::Key_Plus, audioMenu, "volume_plus"));
    volumeActions->setDecAction(new ActionWrapper(tr("Volume -"), Qt::Key_Minus, audioMenu, "volume_minus"));

    BistableAction *muteAct = new BistableAction(Qt::Key_M, audioMenu, "mute");
    muteAct->setCheckable(true);
    muteAct->setPassiveState(tr("Mute"), QIcon(":/icons/default/volume"));
    muteAct->setActiveState(tr("Mute"), QIcon(":/icons/default/mute"));
    connect(player->prop("mute"), &PropertyObserver::changedBool, muteAct, &BistableAction::switchState);
    connect(muteAct, &QAction::toggled, player->prop("mute"), &PropertyObserver::set);

    IncDecActionsPair *audioDelayActions = new IncDecActionsPair(this);
    audioDelayActions->setDelta(Config::i().get("steps/audio_delay_acts", 0.1).toDouble());
    connect(audioDelayActions, &IncDecActionsPair::valueChanged, player->prop("audio-delay"), &PropertyObserver::setRelative);
    audioDelayActions->setIncAction(new ActionWrapper(tr("Delay +"), QKeySequence(), audioMenu, "audio_delay_plus"));
    audioDelayActions->setDecAction(new ActionWrapper(tr("Delay -"), QKeySequence(), audioMenu, "audio_delay_minus"));
    ActionWrapper *audioDelayResetAct = new ActionWrapper(tr("Reset delay"), QKeySequence(), audioMenu, "audio_delay_reset", QIcon());
    connect(audioDelayResetAct, &QAction::triggered, player->prop("audio-delay"), &PropertyObserver::reset);
    ActionWrapper *audioDelaySetAct = new ActionWrapper(tr("Set delay..."), QKeySequence(), audioMenu, "audio_delay_set", QIcon());
    audioDelaySetAct->setData(QVariantList{"audio-delay", tr("Set audio delay (in seconds)"), -300, 300});
    callPropEditorAg->addAction(audioDelaySetAct);

    // --- Subtitles ---
    // TODO: add external
    subTracksMenu = bindTracksMenu(QIcon(":/icons/default/sub"), tr("Primary track"), "sid", subtitlesMenu);
    subTracksMenu->setRewriteRule(extfolderRewriteRule);
    secondSubTracksMenu = bindTracksMenu(QIcon(":/icons/default/sub"), tr("Secondary track"), "secondary-sid", subtitlesMenu);
    subtitlesMenu->addSeparator();

    IncDecActionsPair *subDelayActions = new IncDecActionsPair(this);
    subDelayActions->setDelta(Config::i().get("steps/sub_delay_acts", 0.1).toDouble());
    connect(subDelayActions, &IncDecActionsPair::valueChanged, player->prop("sub-delay"), &PropertyObserver::setRelative);
    subDelayActions->setIncAction(new ActionWrapper(tr("Delay +"), QKeySequence(), subtitlesMenu, "sub_delay_plus"));
    subDelayActions->setDecAction(new ActionWrapper(tr("Delay -"), QKeySequence(), subtitlesMenu, "sub_delay_minus"));
    ActionWrapper *subDelaySetAct = new ActionWrapper(tr("Set delay..."), QKeySequence(), subtitlesMenu, "sub_delay_set", QIcon());
    subDelaySetAct->setData(QVariantList{"sub-delay", tr("Set subtitle delay (in seconds)"), -300, 300});
    callPropEditorAg->addAction(subDelaySetAct);
    ActionWrapper *subDelayResetAct = new ActionWrapper(tr("Reset delay"), QKeySequence(), subtitlesMenu, "sub_delay_reset", QIcon());
    connect(subDelayResetAct, &QAction::triggered, player->prop("sub-delay"), &PropertyObserver::reset);
    subtitlesMenu->addSeparator();

    IncDecActionsPair *subPosActions = new IncDecActionsPair(this);
    subPosActions->setDelta(Config::i().get("steps/sub_pos_acts", 1).toDouble());
    connect(subPosActions, &IncDecActionsPair::valueChanged, player->prop("sub-pos"), &PropertyObserver::setRelative);
    subPosActions->setDecAction(new ActionWrapper(tr("Up"), Qt::Key_8, subtitlesMenu, "sub_pos_plus"));
    subPosActions->setIncAction(new ActionWrapper(tr("Down"), Qt::Key_2, subtitlesMenu, "sub_pos_minus"));
    subtitlesMenu->addSeparator();

    IncDecActionsPair *subScaleActions = new IncDecActionsPair(this);
    subScaleActions->setDelta(Config::i().get("steps/sub_scale_acts", 0.1).toDouble());
    connect(subScaleActions, &IncDecActionsPair::valueChanged, player->prop("sub-scale"), &PropertyObserver::setRelative);
    subScaleActions->setIncAction(new ActionWrapper(tr("Size +"), QKeySequence(), subtitlesMenu, "sub_scale_plus"));
    subScaleActions->setDecAction(new ActionWrapper(tr("Size -"), QKeySequence(), subtitlesMenu, "sub_scale_minus"));
    subtitlesMenu->addSeparator();

    IncDecActionsPair *subSeekActions = new IncDecActionsPair(this);
    connect(subSeekActions, &IncDecActionsPair::valueChanged, player, &PlayerController::subSeekPrimary);
    subSeekActions->setIncAction(new ActionWrapper(tr("Seek to next subtitle"), QKeySequence(), subtitlesMenu, "sub_seek_plus"));
    subSeekActions->setDecAction(new ActionWrapper(tr("Seek to previous subtitle"), QKeySequence(), subtitlesMenu, "sub_seek_minus"));
    subtitlesMenu->addSeparator();

    IncDecActionsPair *subStepActions = new IncDecActionsPair(this);
    connect(subStepActions, &IncDecActionsPair::valueChanged, player, &PlayerController::subStepPrimary);
    subStepActions->setIncAction(new ActionWrapper(tr("Show next subtitle"), QKeySequence(), subtitlesMenu, "sub_step_plus"));
    subStepActions->setDecAction(new ActionWrapper(tr("Show previous subtitle"), QKeySequence(), subtitlesMenu, "sub_step_minus"));

    // --- Misc ---
    ActionWrapper *prefAction = new ActionWrapper(tr("Preferences"), QKeySequence(), miscMenu, "prefs", QIcon(":/icons/default/prefs"));
    connect(prefAction, &QAction::triggered, pref, &QDialog::show);

    QAction *toggleAction = playlistDock->toggleViewAction();
    toggleAction->setIcon(QIcon(":/icons/default/playlist"));
    miscMenu->addAction(toggleAction);
}

void KokoVP::createPlaylistDock()
{
    playlist = new Playlist(this);

    playlistDock = new QDockWidget(this);
    playlistDock->setWidget(playlist);
    playlistDock->setWindowTitle(playlist->windowTitle());

    playlistDock->setVisible(Cache::i().get("gui_playlist/show").toBool());
    playlistDock->setAllowedAreas(Qt::AllDockWidgetAreas);

    addDockWidget(Cache::i().get("gui_playlist/dock_area", Qt::RightDockWidgetArea).value<Qt::DockWidgetArea>(), playlistDock);

    // Need to check this conditions, because entering fullscreen, exiting app, etc. may invalidate these settings
    connect(playlistDock, &QDockWidget::dockLocationChanged, this,[this](Qt::DockWidgetArea area){ if(static_cast<QWidget*>(sender())->isVisible()) Cache::i().set("gui_playlist/dock_area", area); });
    connect(playlistDock, &QDockWidget::visibilityChanged, this, [this](bool toggled){ if(isVisible() && !isFullScreen()) Cache::i().set("gui_playlist/show", toggled); });
}

void KokoVP::readConfig()
{
    Config::i().beginGroup("shortcuts");
    for (auto &act : actions())
        act->setShortcut(Config::i().get(act->objectName(), act->shortcut()).value<QKeySequence>());
    Config::i().endGroup();

    // Screenshots
    Config::i().beginGroup("screenshots");
    player->setScreenshotOpts(Config::i().get("dir", QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)).toString(),
                              Config::i().get("template", "cap_%F_%p_%02n").toString(),
                              Config::i().get("format", "jpg").toString());
    Config::i().endGroup();

    // Ext files
    Config::i().beginGroup("extfiles");
    player->setExtSubOptions(Config::i().get("sub_mode", "exact").toString(), Config::i().get("sub_depth", 2).toInt());
    player->setExtAudioOptions(Config::i().get("audio_mode", "exact").toString(), Config::i().get("audio_depth", 2).toInt());
    Config::i().endGroup();

    // Seek
    Config::i().beginGroup("steps");
    seek->setStepTime(0, Config::i().get("seek0", 10).toInt());
    seek->setStepTime(1, Config::i().get("seek1", 30).toInt());
    seek->setStepTime(2, Config::i().get("seek2", 300).toInt());
    Config::i().endGroup();

    TimeSliderAction *tsAct = dynamic_cast<TimeSliderAction*>(p_actionsMap.value("time_slider"));
    assert(tsAct);
    tsAct->setChangeTimeOnDrag(Config::i().get("misc/seek_on_drag", false).toBool());

    // Mouse actions
    playerWidget->setClickAction(p_actionsMap.value(Config::i().get("actions/single_video_click", "play_pause").toString()));
    playerWidget->setDoubleClickAction(p_actionsMap.value(Config::i().get("actions/double_video_click", "fullscreen").toString()));
    playerWidget->setRightClickAction(p_actionsMap.value(Config::i().get("actions/right_video_click", "show_context_menu").toString()));
    playerWidget->setWheelActions(p_actionsMap.value(Config::i().get("actions/wheel_up", "seek0_plus").toString()),
                                  p_actionsMap.value(Config::i().get("actions/wheel_down", "seek0_minus").toString()));

    // Misc. properties
    player->setProp("audio-device", Config::i().get("misc/audio_device", "auto").toString());
    player->setOption("hwdec", Config::i().get("misc/hwdec", "auto").toString());

    // TODO: for SVP testing, should be handled separately
    player->setProp("input-ipc-server", "/tmp/mpvsocket");
    player->setProp("hr-seek-framedrop", false);

    if (Config::i().get("misc/glsl_enable", false).toBool())
        player->setProp("glsl-shaders", Config::i().get("misc/glsl").toStringList());
    else
        player->setProp("glsl-shaders", QStringList{});

    player->setOption("alang", Config::i().get("tracks/alang").toStringList());
    player->setOption("slang", Config::i().get("tracks/slang").toStringList());
}

void KokoVP::insertActionsMap(QAction *action)
{
    p_actionsMap.insert(action->objectName(), action);
}

void KokoVP::openFiles()
{
    playlist->addURLs(Helper::openMediaFiles(this));
    playlist->playLast();
}

void KokoVP::openDirectory()
{
    const auto &urls = Helper::openMediaDirectory(this);
    if (urls.empty())
        return;

    playlist->clear();
    playlist->addURLs(urls);
    playlist->playFirst();
}

void KokoVP::videoScreenshot()
{
    player->screenshot(QString(), false); //TODO: maybe frontend will format filenames, instead of mpv itself
}

void KokoVP::videoSubScreenshot()
{
    player->screenshot(QString(), true); //TODO: maybe frontend will format filenames, instead of mpv itself
}

void KokoVP::handleTracks()
{
    videoTracksMenu->clear();
    audioTracksMenu->clear();
    subTracksMenu->clear();
    secondSubTracksMenu->clear();

    auto &tracks = player->tracks();

    for (auto &t : tracks)
    {
        switch (t.type) {
        case PlayerController::Track::TRACK_TYPE_VIDEO:
            videoTracksMenu->addTrack(t);
            break;
        case PlayerController::Track::TRACK_TYPE_AUDIO:
            audioTracksMenu->addTrack(t);
            break;
        case PlayerController::Track::TRACK_TYPE_SUB:
            subTracksMenu->addTrack(t);
            secondSubTracksMenu->addTrack(t);
            break;
        };
    }
    if (Config::i().get("play_mode/keep_props", true).toBool())
        fileSettings->loadSettingsFor(player->currentFile(), Config::i().get("play_mode/keep_timepos", true).toBool());
}

void KokoVP::handleEOF(bool wasStopped)
{
    setWindowTitle("KokoVP");
    fileSettings->saveSettingsFor(player->lastOpenFile(), wasStopped); // If file is ended, then time-pos shouldn't be saved
    player->prop("pause")->set(true);
    if (!wasStopped && Config::i().get("play_mode/next_on_eof", true).toBool())
        playlist->next();
}

void KokoVP::callPropEditor(QAction *callEditorAction)
{
    QVariantList data = callEditorAction->data().value<QVariantList>();
    QString propName = data.at(0).toString();
    QString propLabel = data.at(1).toString();
    double min = data.at(2).toDouble();
    double max = data.at(3).toDouble();

    PropertyObserver *obs = player->prop(propName);
    QVariant currVal = obs->get();

    bool ok;
    double newVal = QInputDialog::getDouble(this, tr("Set value"),
                                       propLabel, currVal.toDouble(), min, max, 2, &ok,
                                       Qt::WindowFlags(), 0.1);
    if (ok)
        obs->set(newVal);
}

void KokoVP::populateAudioDeviceMenu()
{
    QMenu *audioDevicesMenu = new QMenu(tr("Audio device"), audioMenu);
    audioDevicesMenu->setIcon(QIcon(":/icons/default/send_audio"));

    connect(audioDevicesMenu, &QMenu::triggered, this, &KokoVP::setAudioDevice);
    audioMenu->addMenu(audioDevicesMenu);
    QVariantList audioDevices = playerWidget->getProp("audio-device-list").value<QVariantList>();

    QStringList allowedAudioSubsystems = Config::i().get("misc/allowed_audio_subsystems", QStringList{"auto", "pulse", "pipewire", "wasapi"}).toStringList();
    for (auto &dev : audioDevices)
    {
        QVariantMap devItems = dev.value<QVariantMap>();
        QString devname = devItems.value("name").toString();
        QString label = devItems.value("description").toString();

        if (!allowedAudioSubsystems.contains(devname.split('/').at(0)))
            continue;

        QAction *act = new QAction(label, audioDevicesMenu);
        act->setData(devname);
        audioDevicesMenu->addAction(act);
    }
}

void KokoVP::setAudioDevice(QAction *audioDeviceAction)
{
    QString devName = audioDeviceAction->data().toString();
    Config::i().set("misc/audio_device", devName);
    playerWidget->setProp("audio-device", devName);
}

void KokoVP::tryPlayCurrent()
{
    if (player->currentFile().isEmpty())
        playlist->playCurrent();
}

void KokoVP::playFile(QUrl file) {
    player->open(file);
    setWindowTitle(QString("%1 - KokoVP").arg(file.fileName()));
}

void KokoVP::actionEvent(QActionEvent *event)
{
    switch (event->type())
    {
        case QEvent::ActionChanged: // Qt docs doesn't specify what is ActionChanged
            break;
        case QEvent::ActionAdded:
            p_actionsMap.insert(event->action()->objectName(), event->action());
            break;
        case QEvent::ActionRemoved:
            p_actionsMap.remove(event->action()->objectName());
            break;
        default:
            break;
    }
    QMainWindow::actionEvent(event);
}
