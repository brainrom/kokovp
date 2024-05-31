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

#include <QHeaderView>
#include <QTableView>
#include <QToolBar>
#include <QVBoxLayout>
#include <QAction>
#include <QMenu>
#include <QPushButton>
#include <QToolButton>
#include <QStyledItemDelegate>
#include <QIODevice>

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
    : QWidget{parent}
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

    QMenu *addMenu = new QMenu(this);
    QAction *addFAct = addMenu->addAction(tr("Add files..."));
    addFAct->setToolTip(tr("Add files to playlist"));
    connect(addFAct, &QAction::triggered, this, &Playlist::addFiles);

    QAction *addDAct = addMenu->addAction(tr("Add directory..."));
    addDAct->setToolTip(tr("Add directory content to playlist"));
    connect(addDAct, &QAction::triggered, this, &Playlist::addDirectory);

    QToolButton *tbAdd = new QToolButton(this);
    addFAct->setToolTip(tr("Add files to playlist"));
    tbAdd->setIcon(QIcon(":/icons/default/plus"));
    bottomBar->addWidget(tbAdd);

    tbAdd->setToolTip(tr("Add new items to playlist"));
    tbAdd->setMenu(addMenu);
    tbAdd->setPopupMode(QToolButton::InstantPopup);

    QAction *delAct = new QAction(QIcon(":/icons/default/minus"), tr("Remove selected"), this);
    delAct->setShortcut(Config::i().get(QString("shortcuts/playlist_remove"), QKeySequence(Qt::Key_Delete)).toString());
    connect(delAct, &QAction::triggered, this, &Playlist::delSelected);
    delAct->setToolTip(tr("Remove selected items from playlist"));
    bottomBar->addAction(delAct);

    QAction *clearAct = new QAction(QIcon(":/icons/default/delete"), tr("Clear"), this);
    clearAct->setShortcut(Config::i().get(QString("shortcuts/playlist_clear"), QKeySequence("Shift+Del")).toString());
    connect(clearAct, &QAction::triggered, this, &Playlist::clear);
    clearAct->setToolTip(tr("Clear playlist"));
    bottomBar->addAction(clearAct);

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
