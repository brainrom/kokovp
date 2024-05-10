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
#include "prefinput.h"
#include "ui_prefinput.h"
#include "qactionlistmodel.h"
#include "shortcuteditordelegate.h"

#include "kokovp.h"
#include "config.h"

PrefInput::PrefInput(QWidget *parent)
    : PrefSection(parent)
    , ui(new Ui::PrefInput)
{
    ui->setupUi(this);

    confOptionsNames = { "actions/single_video_click", "actions/double_video_click", "actions/middle_video_click", "actions/right_video_click", "actions/x1_video_click", "actions/x2_video_click" };
    btnsActionsCombo = { ui->left_click_combo, ui->double_click_combo, ui->right_click_combo, ui->middle_click_combo, ui->xbutton1_click_combo, ui->xbutton2_click_combo };
    assert(confOptionsNames.count()==btnsActionsCombo.count());

    actsModel = new QActionListModel(this);
    ShortcutEditorDelegate *actsDelegate = new ShortcutEditorDelegate(this);
    ui->tableView->setModel(actsModel);
    ui->tableView->setItemDelegateForColumn(QActionListModel::COL_SHORTCUT, actsDelegate);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
}

PrefInput::~PrefInput()
{
    delete ui;
}

void PrefInput::load()
{
    actsModel->setQActionList(KokoVP::i()->actions());

    QStringList allowedBtnsActions = { "fullscreen", "play_pause", "show_context_menu", "mute" }; //TODO: can be extended...

    const QActionMap &actions = KokoVP::i()->actionsMap();

    int i=0;
    for (auto &cb : btnsActionsCombo)
    {
        cb->clear();
        cb->addItem(tr("No"));
        for (auto &actName : allowedBtnsActions)
        {
            const QAction *act = actions.value(actName);
            cb->addItem(act->text(), actName);
        }
        cb->setCurrentIndex(cb->findData(Config::i().get(confOptionsNames.at(i)))); // Select action, which was set for this button
        i++;
    }
}

void PrefInput::save()
{
    Config::i().beginGroup("shortcuts");
    for (auto &act : KokoVP::i()->actions())
        Config::i().set(act->objectName(), act->shortcut());
    Config::i().endGroup();
}
