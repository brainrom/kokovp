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
#include "config.h"
#include "ui_prefappearance.h"

#include <QDir>
#include <QString>
#include <QStyleFactory>

const QString PrefAppearance::uiThemeConfigKey = QString("appearance/ui_theme");
const QString PrefAppearance::iconThemeConfigKey = QString("appearance/icon_theme");

PrefAppearance::PrefAppearance(QWidget *parent)
    : PrefSection(parent), ui(new Ui::PrefAppearance)
{
    ui->setupUi(this);

    ui->cbUiTheme->addItem(tr("<System default>"), QString());
    for (auto &theme: QStyleFactory::keys())
        ui->cbUiTheme->addItem(theme, theme);

    ui->cbIconTheme->addItem(tr("<System default>"), QString());
    for (auto &path : QIcon::themeSearchPaths())
    {
        QDir iconDir = QDir(path);
        if (!iconDir.exists())
            continue;
        QStringList iconThemes = iconDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::NoSort);
        for (auto &theme : iconThemes)
            ui->cbIconTheme->addItem(theme, theme);
    }
}

PrefAppearance::~PrefAppearance()
{
    delete ui;
}

void PrefAppearance::load()
{
    int uiThemeNum = ui->cbUiTheme->findData(Config::i().get(uiThemeConfigKey));
    if (uiThemeNum >= 0)
        ui->cbUiTheme->setCurrentIndex(uiThemeNum);

    int iconThemeNum = ui->cbIconTheme->findData(Config::i().get(iconThemeConfigKey));
    if (iconThemeNum >= 0)
        ui->cbIconTheme->setCurrentIndex(iconThemeNum);
}

void PrefAppearance::save()
{
    Config::i().set(uiThemeConfigKey, ui->cbUiTheme->currentData().toString());
    Config::i().set(iconThemeConfigKey, ui->cbIconTheme->currentData().toString());
}
