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
#include "playlist.h"
#include "playlistmodel.h"
#include "cache.h"
#include "config.h"
#include "helper.h"
#include "extensions.h"

#include <QHeaderView>
#include <QTableView>
#include <QToolBar>
#include <QVBoxLayout>
#include <QAction>
#include <QMenu>
#include <QPushButton>
#include <QToolButton>
#include <QStyledItemDelegate>
#include <QFileDialog>
#include <QMessageBox>

// Playlists codecs
#include "m3ucodec.h"
#include "xspfcodec.h"
#include "plscodec.h"

class PlaylistDelegate : public QStyledItemDelegate {
public:
    PlaylistDelegate(QObject * parent = 0) : QStyledItemDelegate(parent) {};
    virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        if (index.column() == PlaylistModel::COL_LABEL) {
            bool current = index.data(PlaylistModel::CurrentRole).toBool();
            if (current) opt.font.setBold(true);
        }
        /*else if (index.column() == COL_FILENAME) {
                opt.textElideMode = Qt::ElideMiddle; //Cut middle of string, leaging string's begin and end
            }*/
        return QStyledItemDelegate::paint(painter, opt, index);
    }
};


Playlist::Playlist(QWidget *parent)
    : QWidget{parent},
    loadFuncs
    {
        {"pls", PLSCodec::load},
        {"xspf", XSPFCodec::load},
        {"m3u", M3UCodec::load},
        {"m3u8", M3UCodec::load},
    },
    saveFuncs
    {
        {"pls", PLSCodec::save},
        {"xspf", XSPFCodec::save},
        {"m3u", M3UCodec::save},
        {"m3u8", M3UCodec::save},
    }
{
    setWindowTitle(tr("Playlist"));

    plModel = new PlaylistModel(this);

    // Playlist Persistence.
    QByteArray encodedData = Cache::i().get("playlist/data").toByteArray();
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    stream >> *plModel;

    QGridLayout *l = new QGridLayout(this);
    l->setContentsMargins(0,0,0,0);

    plView = new QTableView(this);
    l->addWidget(plView, 0, 0, 1, -1);

    plView->setDragDropMode(QAbstractItemView::DragDrop);
    plView->setDefaultDropAction(Qt::MoveAction);
    plView->setDropIndicatorShown(true);
    plView->setDragDropOverwriteMode(false);

    plView->setShowGrid(false);

    plView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    plView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    plView->setSelectionBehavior(QAbstractItemView::SelectRows);
    plView->setContextMenuPolicy( Qt::CustomContextMenu );

    plView->verticalHeader()->hide();
    plView->horizontalHeader()->setStretchLastSection(true);

    plView->setModel(plModel);
    plView->setItemDelegate(new PlaylistDelegate(this));

    bottomBar = new QToolBar(this);
    l->addWidget(bottomBar, 1, 0, 1, -1);

    QMenu *fileMenu = new QMenu(this);
    QAction *loadPlaylistAct = fileMenu->addAction(tr("Load..."));
    loadPlaylistAct->setIcon(QIcon::fromTheme("document-open"));
    loadPlaylistAct->setToolTip(tr("Add files to playlist"));
    connect(loadPlaylistAct, &QAction::triggered, this, &Playlist::loadPlaylist);

    QAction *savePlaylistAct = fileMenu->addAction(tr("Save..."));
    savePlaylistAct->setIcon(QIcon::fromTheme("document-save"));
    savePlaylistAct->setToolTip(tr("Add directory content to playlist"));
    connect(savePlaylistAct, &QAction::triggered, this, qOverload<>(&Playlist::savePlaylist));

    QToolButton *tbFile = new QToolButton(this);
    tbFile->setIcon(QIcon::fromTheme("document-open"));
    bottomBar->addWidget(tbFile);

    tbFile->setToolTip(tr("Load/Save"));
    tbFile->setMenu(fileMenu);
    tbFile->setPopupMode(QToolButton::InstantPopup);

    QMenu *addMenu = new QMenu(this);
    QAction *addFAct = addMenu->addAction(tr("Add files..."));
    addFAct->setToolTip(tr("Add files to playlist"));
    connect(addFAct, &QAction::triggered, this, &Playlist::addFiles);

    QAction *addDAct = addMenu->addAction(tr("Add directory..."));
    addDAct->setToolTip(tr("Add directory content to playlist"));
    connect(addDAct, &QAction::triggered, this, &Playlist::addDirectory);

    QToolButton *tbAdd = new QToolButton(this);
    tbAdd->setIcon(QIcon::fromTheme("list-add"));
    bottomBar->addWidget(tbAdd);

    tbAdd->setToolTip(tr("Add new items to playlist"));
    tbAdd->setMenu(addMenu);
    tbAdd->setPopupMode(QToolButton::InstantPopup);

    QAction *delAct = new QAction(QIcon::fromTheme("list-remove"), tr("Remove selected"), this);
    delAct->setShortcut(Config::i().get(QString("shortcuts/playlist_remove"), QKeySequence(Qt::Key_Delete)).toString());
    connect(delAct, &QAction::triggered, this, &Playlist::delSelected);
    delAct->setToolTip(tr("Remove selected items from playlist"));
    bottomBar->addAction(delAct);

    QAction *clearAct = new QAction(QIcon::fromTheme("edit-delete"), tr("Clear"), this);
    clearAct->setShortcut(Config::i().get(QString("shortcuts/playlist_clear"), QKeySequence("Shift+Del")).toString());
    connect(clearAct, &QAction::triggered, this, &Playlist::clear);
    clearAct->setToolTip(tr("Clear playlist"));
    bottomBar->addAction(clearAct);

    QAction *shuffleAct = new QAction(QIcon::fromTheme("media-playlist-shuffle"), tr("Shuffle"), this);
    connect(shuffleAct, &QAction::triggered, plModel, &PlaylistModel::shuffle);
    shuffleAct->setToolTip(tr("Shuffle playlist"));
    bottomBar->addAction(shuffleAct);

    contextMenu = new QMenu(this);
    contextMenu->addActions(addMenu->actions());
    contextMenu->addSeparator();
    contextMenu->addAction(delAct);
    contextMenu->addAction(clearAct);

    connect(plView, &QWidget::customContextMenuRequested, this, [this](const QPoint &pos) {contextMenu->popup(plView->viewport()->mapToGlobal(pos));});
    connect(plView, &QTableView::activated, this, [this](const QModelIndex &index){ playRow(index.row()); });
}

Playlist::~Playlist()
{
    // Playlist Persistence
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    stream << *plModel;
    Cache::i().set("playlist/data", encodedData);
    Cache::i().sync();
}

void Playlist::setCurrentRowMetainfo(QString label, double duration)
{
    plModel->setCurrentRowMetainfo(label, duration);
}

bool Playlist::addURLs(const QList<QUrl> &urls)
{
    return plModel->addURLs(urls);
}

void Playlist::addFiles()
{
    addURLs(Helper::openMediaFiles(this));
}

void Playlist::addDirectory()
{
    addURLs(Helper::openMediaDirectory(this));
}

void Playlist::delSelected()
{
    QModelIndexList selItems = plView->selectionModel()->selectedRows();
    QList<QPersistentModelIndex> selItems_p;
    for (auto &idx : selItems)
        selItems_p.append(idx);

    for (auto &idx: selItems_p)
        plModel->removeRows(idx.row(), 1);
}

void Playlist::clear()
{
    plModel->removeRows(0, plModel->rowCount());
}

bool Playlist::isEmpty()
{
    return plModel->rowCount()==0;
}

void Playlist::playFirst()
{
    if (plModel->rowCount()==0)
        return;
    playRow(0);
}

void Playlist::playLast()
{
    if (plModel->rowCount()==0)
        return;
    playRow(plModel->rowCount()-1);
}

void Playlist::playCurrent()
{
    if (plModel->currentRow()<0)
        return;
    emit playRequest(plModel->data(plModel->index(plModel->currentRow(), 0), PlaylistModel::URLRole).value<QUrl>());
}

// This function has been copied (and modified a little bit) from SMPlayer (program under GPL license).
void Playlist::loadPlaylist()
{
    QString s = QFileDialog::getOpenFileName(
        this, tr("Choose a file"),
        Cache::i().get("file_open/last_file_dir").toString(),
        tr("Playlists") + Extensions.playlist().forFilter());

    if (!s.isEmpty()) {
        Cache::i().set("file_open/last_file_dir", QFileInfo(s).absolutePath());

        QString suffix = QFileInfo(s).suffix().toLower();
        auto f = loadFuncs.value(suffix);

        if (!f)
            return;

        plModel->removeRows(0, plModel->rowCount());
        plModel->addItems(f(s));
    }
}

// This function has been copied (and modified a little bit) from SMPlayer (program under GPL license).
bool Playlist::savePlaylist(QString s)
{
    if (s.isEmpty()) {
        s = QFileDialog::getSaveFileName(
            this, tr("Choose a filename"),
            Cache::i().get("file_open/last_file_dir").toString(),
            tr("Playlists") + Extensions.playlist().forFilter());
    }

    if (s.isEmpty())
        return false;

    // If filename has no extension, add it
    if (QFileInfo(s).suffix().isEmpty()) {
        s = s + ".m3u";
    }
    if (QFileInfo::exists(s))
    {
        int res = QMessageBox::question( this,
                    tr("Confirm overwrite?"),
                    tr("The file %1 already exists.\n"
                       "Do you want to overwrite?").arg(s),
                    QMessageBox::Yes | QMessageBox::No | QMessageBox::NoButton);
        if (res == QMessageBox::No ) {
            return false;
        }
    }
    Cache::i().set("file_open/last_file_dir", QFileInfo(s).absolutePath());

    QString suffix = QFileInfo(s).suffix().toLower();
    auto f = saveFuncs.value(suffix);
    if (!f)
        return false;

    return f(s, plModel->items());
}

void Playlist::playRow(int row)
{
    plModel->setRowCurrent(row);
    emit playRequest(plModel->data(plModel->index(row, 0), PlaylistModel::URLRole).value<QUrl>());
}

// It's better to ignore action, if actual new item isn't available
// because mpv can enter deadlock when constantly opening new files
void Playlist::prev()
{
    int nextRow = plModel->currentRow()-1;
    if (nextRow>=0)
        playRow(nextRow);
}

void Playlist::next()
{
    int nextRow = plModel->currentRow()+1;
    if (nextRow < plModel->rowCount())
        playRow(nextRow);
}
