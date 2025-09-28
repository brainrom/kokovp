/*  This file was part of smplayer and now part of KokoVP
    smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2023 Ricardo Villalba <ricardo@smplayer.info>

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

#include "filesettingshash.h"
#include "filehash.h" // hash function
#include "playercontroller.h"
#include "mpvwidget.h"

#include <QSettings>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QRandomGenerator>

FileSettingsHash::FileSettingsHash(QString directory,  PlayerController *player, QObject *parent): QObject{parent}, p_player{player}
{
    base_dir = directory + "/file_settings";
    persistentProps = { "volume", "aid", "vid", "secondary-sid", "sid", "sub-scale", "sub-pos", "sub-delay", "audio-delay" };

    for (auto &p : persistentProps)
        connect(p_player->prop(p), &PropertyObserver::changed, this, &FileSettingsHash::updateCurrentProps);

    connect(p_player->prop("time-pos"), &PropertyObserver::changed, this, &FileSettingsHash::updateCurrentProps);
    salt = loadOrCreateSalt();
}

QString FileSettingsHash::configFile(const QString & filename) {
    QString hash = FileHash::calculateHash(filename);
    if (hash.isEmpty())
        return QString();
    return base_dir +"/"+ hash[0] +"/"+ hash + ".ini";
}

QString FileSettingsHash::configFileSHA256(const QString & filename) {
    QFile file(filename);

    if (!file.exists()) {
        qWarning("FileSettingsHash:configFileSHA256: error hashing file. File doesn't exist.");
        return QString();
    }
    file.open(QIODevice::ReadOnly);

    QString hash = FileHash::calculateHashSHA256(&file, salt);
    if (hash.isEmpty())
        return QString();
    return base_dir +"/"+ hash[0] +"/"+ hash + ".ini";
}

QByteArray FileSettingsHash::loadOrCreateSalt()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "KokoVP", "salt");

    QByteArray salt = settings.value("salt").toByteArray();
    if (salt.size() == 16)
        return salt;

    salt.resize(16);
    QRandomGenerator::global()->generate(salt.begin(), salt.end());
    settings.setValue("salt", salt);
    settings.sync();
    return salt;
}

bool FileSettingsHash::loadSettingsFor(QString filename, bool loadTimepos) {
    QString config_file = configFileSHA256(filename);
    if (!QFile::exists(config_file))
    {
        QString config_file_old = configFile(filename);
        if (!QFile::exists(config_file_old))
            return false;
        QFile::rename(config_file_old, config_file);
    }

    if (config_file.isEmpty())
        return false;

    QSettings settings(config_file, QSettings::IniFormat);

    settings.beginGroup("props");
    for (auto &p : persistentProps)
        p_player->prop(p)->set(settings.value(p));

    if (loadTimepos)
        p_player->seekAbsolute(settings.value("time-pos").toDouble());
    settings.endGroup();
    return true;
}

bool FileSettingsHash::saveSettingsFor(QString filename, bool saveTimepos) {
    if (filename.isEmpty())
        return false;

    QString config_file = configFileSHA256(filename);
    QString output_dir = QFileInfo(config_file).absolutePath();

    if (config_file.isEmpty())
        return false;

    QDir d(base_dir);
    if (!d.exists(output_dir)) {
        if (!d.mkpath(output_dir)) {
            qWarning() << "FileSettingsHash::saveSettingsFor: can't create directory" << QString(base_dir + "/" + output_dir);
            return false;
        }
    }

    QSettings settings(config_file, QSettings::IniFormat);

    settings.beginGroup("props");
    for (auto &p : persistentProps)
        settings.setValue(p, currentProps.value(p));
    settings.setValue("time-pos", saveTimepos ? currentProps.value("time-pos") : QVariant(0));
    settings.endGroup();
    settings.sync();
    return true;
}

void FileSettingsHash::updateCurrentProps(QVariant value)
{
    PropertyObserver *p = static_cast<PropertyObserver*>(sender());
    if (!value.isValid())
    {
        qWarning() << "returned " << p->name() << "value is invalid";
        return;
    }

    currentProps.insert(p->name(), value);
}
