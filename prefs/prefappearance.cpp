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
#include "prefappearance.h"
#include "ui_prefappearance.h"
#include "config.h"

#include <QDir>
#include <QStyleFactory>
#include <QString>

const std::string uiThemeConfigKey = "appearance/ui_theme";
const std::string iconThemeConfigKey = "appearance/icon_theme";

const std::string uiThemeDefault = "<System default>";
const std::string iconThemeDefault = "<System default>";

PrefAppearance::PrefAppearance(QWidget *parent)
    : PrefSection(parent)
    , ui(new Ui::PrefAppearance)
{
    ui->setupUi(this);

    ui->cbUiTheme->addItem(tr(uiThemeDefault.c_str()));
    ui->cbUiTheme->addItems(QStyleFactory::keys());

    ui->cbIconTheme->addItem(tr(iconThemeDefault.c_str()));
    for (auto path : QIcon::themeSearchPaths()) {
        QDir iconDir = QDir(path);
        if (!iconDir.exists())
            continue;
        QStringList iconThemes = iconDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::NoSort);
        std::for_each(iconThemes.begin(), iconThemes.end(), [&](const QString &theme) {ui->cbIconTheme->addItem(theme);});
    }
}

PrefAppearance::~PrefAppearance()
{
    delete ui;
}

void PrefAppearance::load()
{
    ui->cbUiTheme->setCurrentText(Config::i().get(uiThemeConfigKey, tr(uiThemeDefault.c_str())).toString());
    ui->cbIconTheme->setCurrentText(Config::i().get(iconThemeConfigKey, tr(iconThemeDefault.c_str())).toString());
}

void PrefAppearance::save()
{
    Config::i().set(uiThemeConfigKey, ui->cbUiTheme->currentText());
    Config::i().set(iconThemeConfigKey, ui->cbIconTheme->currentText());
}
