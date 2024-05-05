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
