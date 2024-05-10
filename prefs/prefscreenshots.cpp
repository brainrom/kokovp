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
#include "prefscreenshots.h"
#include "ui_prefscreenshots.h"
#include "config.h"
#include <QFileDialog>
#include <QStandardPaths>

PrefScreenshots::PrefScreenshots(QWidget *parent)
    : PrefSection(parent)
    , ui(new Ui::PrefScreenshots)
{
    ui->setupUi(this);
    QAction *selectPath = new QAction(tr("Browse"),this);
    selectPath->setIcon(QIcon(":/icons/default/folder_open"));
    connect(selectPath, &QAction::triggered, this, &PrefScreenshots::selectScreenshotsFolder);
    ui->screenshot_edit->addAction(selectPath, QLineEdit::TrailingPosition);
}

PrefScreenshots::~PrefScreenshots()
{
    delete ui;
}

void PrefScreenshots::load()
{
    Config::i().beginGroup("screenshots");
    ui->screenshot_edit->setText(Config::i().get("dir").toString());
    ui->screenshot_template_edit->setText(Config::i().get("template").toString());
    ui->screenshot_format_combo->setCurrentText(Config::i().get("format").toString());
    Config::i().endGroup();
}

void PrefScreenshots::save()
{
    Config::i().beginGroup("screenshots");
    Config::i().set("dir", ui->screenshot_edit->text());
    Config::i().get("template", ui->screenshot_template_label->text());
    Config::i().get("format", ui->screenshot_format_combo->currentText());
    Config::i().endGroup();
}

void PrefScreenshots::selectScreenshotsFolder()
{
    QString directory = QFileDialog::getExistingDirectory(this, tr("Select screenshots directory"), QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));

    if (!directory.isEmpty()) {
        ui->screenshot_edit->setText(directory);
    }
}
