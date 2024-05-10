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
#include "prefextfile.h"
#include "ui_prefextfile.h"
#include "config.h"

PrefExtFile::PrefExtFile(QWidget *parent)
    : PrefSection(parent)
    , ui(new Ui::PrefExtFile)
{
    ui->setupUi(this);
    ui->tabWidget->setTabEnabled(1, false);
    ui->tabWidget->setTabVisible(1, false);

    // Qt Designer currently doesn't allow to set data directly via .ui, so there is a small workaround to fill QComboBox'es data
    QStringList modesData = {"no", "exact", "fuzzy", "all"};
    assert(modesData.size()==ui->audio_mode->count());
    assert(modesData.size()==ui->sub_mode->count());
    for (int i=0; i<modesData.size(); i++)
    {
        ui->audio_mode->setItemData(i, modesData.at(i));
        ui->sub_mode->setItemData(i, modesData.at(i));
    }
}

PrefExtFile::~PrefExtFile()
{
    delete ui;
}

void PrefExtFile::load()
{
    Config::i().beginGroup("extfiles");

    ui->sub_mode->setCurrentIndex(ui->sub_mode->findData(Config::i().get("sub_mode").toString()));
    ui->sub_depth->setValue(Config::i().get("sub_depth").toInt());

    ui->audio_mode->setCurrentIndex(ui->audio_mode->findData(Config::i().get("audio_mode").toString()));
    ui->audio_depth->setValue(Config::i().get("audio_depth").toInt());

    Config::i().endGroup();
}

void PrefExtFile::save()
{
    Config::i().beginGroup("extfiles");

    Config::i().set("sub_mode", ui->sub_mode->currentData());
    Config::i().set("sub_depth", ui->sub_depth->value());

    Config::i().set("audio_mode", ui->audio_mode->currentData());
    Config::i().set("audio_depth", ui->audio_depth->value());

    Config::i().endGroup();
}
