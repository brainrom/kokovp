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
#include "prefadvanced.h"
#include "optionsmodel.h"
#include "utils/checkboxdelegate.h"
#include "ui_prefadvanced.h"
#include "config.h"

#include <QFileDialog>
#include <QItemSelectionRange>
#include <QMessageBox>
#include <QMimeData>
#include <QTextStream>

const QString PrefAdvanced::optionTableConfigKey = QString("advanced/options");
const QString PrefAdvanced::shadersListConfigKey = QString("advanced/shaders");
const QString PrefAdvanced::shadersEnableConfigKey = QString("advanced/shaders_enable");

PrefAdvanced::PrefAdvanced(QWidget *parent) : PrefSection(parent),
                                              ui(new Ui::PrefAdvanced)
{
    ui->setupUi(this);
    CheckBoxDelegate *cbDelegate = new CheckBoxDelegate(this);

    optsModel = new OptionsModel(this);
    ui->twOptions->setModel(optsModel);

    shadersModel = new OptionsModel(this);
    ui->twShaders->setModel(shadersModel);

    connect(ui->pbImport, &QPushButton::clicked, this, &PrefAdvanced::importTable);
    connect(ui->pbExport, &QPushButton::clicked, this, &PrefAdvanced::exportTable);

    ManipulationTableView *tables[] = {ui->twOptions, ui->twShaders};
    for (auto t: tables)
    {
        t->setDragDropMode(QAbstractItemView::DragDrop);
        t->setSelectionBehavior(QHeaderView::SelectRows); // For now SelectRows is important for move in OptionsModel work correctly
        t->setDefaultDropAction(Qt::MoveAction);
        t->setDropIndicatorShown(true);
        t->setDragDropOverwriteMode(false);
        t->setShowGrid(false);
        t->setItemDelegateForColumn(OptionsModel::OptionsModel_Enabled, cbDelegate);
        t->horizontalHeader()->setSectionResizeMode(OptionsModel::OptionsModel_Enabled, QHeaderView::ResizeToContents);
    }
    ui->twShaders->setColumnHidden(OptionsModel::OptionsModel_Value, true);
}

void PrefAdvanced::load()
{
    importTableModel(ui->twOptions->model(), Config::i().get(optionTableConfigKey).toString());
    importTableModel(ui->twShaders->model(), Config::i().get(shadersListConfigKey).toString());
    ui->cbEnableShaders->setChecked(Config::i().get(shadersEnableConfigKey).toBool());
    ui->twOptions->resizeColumnsToContents();
    ui->twShaders->resizeColumnsToContents();
}

void PrefAdvanced::save()
{
    Config::i().set(optionTableConfigKey, exportTableModel(ui->twOptions->model()));
    Config::i().set(shadersListConfigKey, exportTableModel(ui->twShaders->model()));
    Config::i().set(shadersEnableConfigKey, ui->cbEnableShaders->isChecked());
}

PrefAdvanced::~PrefAdvanced()
{
    delete ui;
}

QTableView *PrefAdvanced::currentView()
{
    switch (ui->tabs->currentIndex())
    {
    case 0:
        return ui->twOptions;
    case 1:
        return ui->twShaders;
    default:
        return nullptr;
    }
}

void PrefAdvanced::importTable()
{
    const QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Import Table from INI"),
        QString(),
        tr("INI Files (*.ini);;All Files (*)"));

    if (fileName.isEmpty())
        return;

    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this,
                             tr("Import Failed"),
                             tr("Cannot read file:\n%1").arg(fileName));
        return;
    }

    QTextStream in(&f);
    in.setEncoding(QStringConverter::Utf8);
    importTableModel(currentView()->model(), in.readAll());
    currentView()->resizeColumnsToContents();
}

void PrefAdvanced::exportTable()
{
    QFileDialog dlg(this, tr("Export Table to INI"));
    dlg.setAcceptMode(QFileDialog::AcceptSave);
    dlg.setNameFilters({
        tr("INI Files (*.ini)"),
        tr("All Files (*)")
    });
    dlg.setDefaultSuffix("ini");

    if (!dlg.exec())
        return;

    const QString fileName = dlg.selectedFiles().constFirst();

    if (fileName.isEmpty())
        return;

    QFile f(fileName);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this,
                             tr("Export Failed"),
                             tr("Cannot write to file:\n%1").arg(fileName));
        return;
    }
    QTextStream out(&f);
    out.setEncoding(QStringConverter::Utf8);
    out << exportTableModel(currentView()->model());
}

const QString PrefAdvanced::exportTableModel(QAbstractItemModel *model)
{
    if (!model)
        return QString();

    QModelIndexList indexes;
    indexes.reserve(model->rowCount() * model->columnCount());

    for (int row = 0; row < model->rowCount(); ++row) {
        for (int col = 0; col < model->columnCount(); ++col)
            indexes << model->index(row, col);
    }

    QScopedPointer<QMimeData> mime(model->mimeData(indexes));

    if (!mime || !mime->hasText()) {
        QMessageBox::warning(this,
                             tr("Export Failed"),
                             tr("Model did not provide text/plain data."));
        return QString();
    }
    return mime->text();
}

void PrefAdvanced::importTableModel(QAbstractItemModel *model, QString data)
{
    if (!model)
        return;

    /* Clear existing rows */
    if (model->rowCount() > 0)
        model->removeRows(0, model->rowCount());

    QMimeData mime;
    mime.setText(data);

    if (!model->dropMimeData(&mime,
                             Qt::CopyAction,
                             -1,
                             -1,
                             QModelIndex())) {
        QMessageBox::warning(this,
                             tr("Import Failed"),
                             tr("Model rejected imported data."));
    }
}
