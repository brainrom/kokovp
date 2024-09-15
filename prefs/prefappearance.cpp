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

namespace
{
const QString themeDefaultDisplayName = QObject::tr("<System default>");

QString readAndConvert(const QString &themeConfigKey)
{
    QString val = Config::i().get(themeConfigKey, PrefAppearance::themeDefaultValue).toString();
    if (val.isEmpty())
        return themeDefaultDisplayName;
    else
        return val;
}

void unconvertAndWrite(const QString &themeConfigKey, const QString &themeName)
{
    if (themeDefaultDisplayName == themeName)
        Config::i().set(themeConfigKey, PrefAppearance::themeDefaultValue);
    else
        Config::i().set(themeConfigKey, themeName);
}
} // namespace

const QString PrefAppearance::uiThemeConfigKey = QString("appearance/ui_theme");
const QString PrefAppearance::iconThemeConfigKey = QString("appearance/icon_theme");
const QString PrefAppearance::themeDefaultValue = QString();

PrefAppearance::PrefAppearance(QWidget *parent)
    : PrefSection(parent), ui(new Ui::PrefAppearance)
{
    ui->setupUi(this);

    ui->cbUiTheme->addItem(themeDefaultDisplayName, themeDefaultValue);
    ui->cbUiTheme->addItems(QStyleFactory::keys());

    ui->cbIconTheme->addItem(themeDefaultDisplayName, themeDefaultValue);
    for (auto path : QIcon::themeSearchPaths())
    {
        QDir iconDir = QDir(path);
        if (!iconDir.exists())
            continue;
        QStringList iconThemes = iconDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::NoSort);
        std::for_each(iconThemes.begin(), iconThemes.end(), [&](const QString &theme)
                      { ui->cbIconTheme->addItem(theme); });
    }
}

PrefAppearance::~PrefAppearance()
{
    delete ui;
}

void PrefAppearance::load()
{
    ui->cbUiTheme->setCurrentText(readAndConvert(uiThemeConfigKey));
    ui->cbIconTheme->setCurrentText(readAndConvert(iconThemeConfigKey));
}

void PrefAppearance::save()
{
    unconvertAndWrite(uiThemeConfigKey, ui->cbUiTheme->currentText());
    unconvertAndWrite(iconThemeConfigKey, ui->cbIconTheme->currentText());
}
