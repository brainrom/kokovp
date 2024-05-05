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

    QStringList allowedBtnsActions = { "fullscreen", "play_pause", "show_context_menu" }; //TODO: can be extended...

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
