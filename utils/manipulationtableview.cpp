#include "manipulationtableview.h"
#include <QMenu>
#include <QGuiApplication>
#include <QClipboard>

#define tr_(...) QGuiApplication::translate(__VA_ARGS__)

ManipulationTableView::ManipulationTableView(QWidget *parent) : QTableView{parent}
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    QAction *copyAct = new QAction(QIcon::fromTheme("edit-copy"), tr_("QShortcut", "Copy"), this);
    copyAct->setShortcut(QKeySequence::Copy);
    connect(copyAct, &QAction::triggered, this, &ManipulationTableView::copy);

    QAction *pasteAct = new QAction(QIcon::fromTheme("edit-paste"), tr_("QShortcut", "Paste"), this);
    pasteAct->setShortcut(QKeySequence::Paste);
    connect(pasteAct, &QAction::triggered, this, &ManipulationTableView::paste);

    QAction *cutAct = new QAction(QIcon::fromTheme("edit-cut"), tr_("QShortcut", "Cut"), this);
    cutAct->setShortcut(QKeySequence::Cut);
    connect(cutAct, &QAction::triggered, this, &ManipulationTableView::cut);

    QAction *delAct = new QAction(QIcon::fromTheme("list-remove"), tr_("QShortcut", "Delete"), this);
    delAct->setShortcut(QKeySequence::Delete);
    connect(delAct, &QAction::triggered, this, &ManipulationTableView::removeSelectedRows);

    QAction *clearAct = new QAction(QIcon::fromTheme("edit-delete"), tr_("QShortcut", "Clear"), this);
    connect(clearAct, &QAction::triggered, this, &ManipulationTableView::clear);

    contextMenu = new QMenu(this);
    contextMenu->addAction(copyAct);
    contextMenu->addAction(cutAct);
    contextMenu->addAction(pasteAct);
    contextMenu->addSeparator();
    contextMenu->addAction(delAct);
    contextMenu->addSeparator();
    contextMenu->addAction(clearAct);

    addActions(contextMenu->actions());

    connect(this, &QWidget::customContextMenuRequested, this, &ManipulationTableView::customMenu);
}

void ManipulationTableView::clear()
{
    if (!model())
        return;

    const int rows = model()->rowCount();
    if (rows > 0)
        model()->removeRows(0, rows);
}

void ManipulationTableView::cut()
{
    copy();
    removeSelectedRows();
}

void ManipulationTableView::copy()
{
    if (!model())
        return;

    const QModelIndexList indexes = selectionModel()->selectedIndexes();
    if (indexes.isEmpty())
        return;

    QMimeData *mime = model()->mimeData(indexes);
    if (!mime)
        return;

    QClipboard *cb = QGuiApplication::clipboard();
    cb->setMimeData(mime);
}

void ManipulationTableView::paste()
{
    if (!model())
        return;

    const QClipboard *cb = QGuiApplication::clipboard();
    const QMimeData *mime = cb->mimeData();
    if (!mime)
        return;

    QModelIndex target;

    if (selectionModel()->hasSelection())
        target = selectionModel()->currentIndex();
    else
        target = model()->index(model()->rowCount()-1, 0);

    if (!target.isValid())
        return;

    if (!model()->canDropMimeData(mime, Qt::CopyAction,
                                  target.row(), target.column(),
                                  target.parent()))
        return;

    model()->dropMimeData(mime, Qt::CopyAction,
                          target.row(), target.column(),
                          target.parent());
}


void ManipulationTableView::customMenu(const QPoint &pos)
{
    contextMenu->popup(viewport()->mapToGlobal(pos));
}

void ManipulationTableView::removeSelectedRows()
{
    auto *view  = this;
    auto *model = view->model();
    auto *sel   = view->selectionModel();
    if (!model || !sel)
        return;

    QModelIndexList indices = sel->selectedIndexes();
    if (indices.isEmpty())
        return;

    // Collect unique rows, skipping invalid and "empty" last row
    QSet<int> rowSet;
    for (const QModelIndex &idx : indices) {
        if (idx.isValid() && idx.row() < model->rowCount() - 1)
            rowSet.insert(idx.row());
    }
    if (rowSet.isEmpty())
        return;

    // Sort rows descending
    QList<int> rows = rowSet.values();
    std::sort(rows.begin(), rows.end(), std::greater<int>());

    // Determine follow index BEFORE deletion
    const int followRow = rows.last();
    const int followCol = indices.last().column();

    for (const auto row : rows)
        model->removeRow(row);

    sel->clearSelection();

    QModelIndex followIndex = model->index(
        qMin(followRow, model->rowCount() - 1), followCol);

    if (followIndex.isValid()) {
        sel->setCurrentIndex(followIndex, QItemSelectionModel::ClearAndSelect);
        view->scrollTo(followIndex, QAbstractItemView::PositionAtCenter);
    }
}
