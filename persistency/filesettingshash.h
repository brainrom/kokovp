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

#ifndef FILESETTINGS_HASH_H
#define FILESETTINGS_HASH_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QVariant>

class PlayerController;

class FileSettingsHash : public QObject
{
    Q_OBJECT
public:
    FileSettingsHash(QString directory, PlayerController *player, QObject *parent = nullptr);
    bool loadSettingsFor(QString filename, bool loadTimepos = true);
    bool saveSettingsFor(QString filename, bool saveTimepos = true);
private:
    void updateCurrentProps(QVariant value);
    QString configFile(const QString & filename);

    PlayerController *p_player;

    QString base_dir;
    QString currentFile;
    QStringList persistentProps;
    QVariantMap currentProps;
};

#endif

