#include "prefmain.h"
#include "ui_prefmain.h"
#include "config.h"

PrefMain::PrefMain(QWidget *parent)
    : PrefSection(parent)
    , ui(new Ui::PrefMain)
{
    ui->setupUi(this);
    ui->cbHwdec->addItem(tr("None"), "no");
    ui->cbHwdec->addItem(tr("Auto"), "auto");
    //TODO: add platform-dependent hwdec APIs?

    ui->seek0->setLabel( tr("Short jump") );
    ui->seek1->setLabel( tr("Medium jump") );
    ui->seek2->setLabel( tr("Long jump") );

    ui->seek0->setIcon(QIcon(":/icons/default/forward10s"));
    ui->seek1->setIcon(QIcon(":/icons/default/forward1m"));
    ui->seek2->setIcon(QIcon(":/icons/default/forward10m"));
}

PrefMain::~PrefMain()
{
    delete ui;
}

void PrefMain::load()
{
    ui->cbAutoPlayNext->setChecked(Config::i().get("play_mode/next_on_eof").toBool());
    ui->cbRememberFileSettings->setChecked(Config::i().get("play_mode/keep_props").toBool());
    ui->cbRememberTimePos->setChecked(Config::i().get("play_mode/keep_timepos").toBool());
    ui->cbHwdec->setCurrentIndex(ui->cbHwdec->findData(Config::i().get("misc/hwdec").toString()));

    Config::i().beginGroup("steps");
    ui->seek0->setTime(Config::i().get("seek0").toInt());
    ui->seek1->setTime(Config::i().get("seek1").toInt());
    ui->seek2->setTime(Config::i().get("seek2").toInt());
    Config::i().endGroup();

    ui->audio_lang_edit->setText(Config::i().get("tracks/alang").toString());
    ui->subtitle_lang_edit->setText(Config::i().get("tracks/slang").toString());
}

void PrefMain::save()
{
    Config::i().set("play_mode/next_on_eof", ui->cbAutoPlayNext->isChecked());
    Config::i().set("play_mode/keep_props", ui->cbRememberFileSettings->isChecked());
    Config::i().set("play_mode/keep_timepos", ui->cbRememberTimePos->isChecked());
    Config::i().set("misc/hwdec", ui->cbHwdec->currentData());

    Config::i().beginGroup("steps");
    Config::i().set("seek0", ui->seek0->time());
    Config::i().set("seek1", ui->seek1->time());
    Config::i().set("seek2", ui->seek2->time());
    Config::i().endGroup();

    Config::i().set("tracks/alang", ui->audio_lang_edit->text());
    Config::i().set("tracks/slang", ui->subtitle_lang_edit->text());

}
