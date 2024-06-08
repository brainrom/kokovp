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
#include "plscodec.h"
#include "playlistmodel.h"

#include <QDebug>
#include <QFileInfo>
#include <QSettings>
#include <QDir>

// These functions has been copied (and modified a little bit) from SMPlayer (program under GPL license).
const QList<PlaylistItem> PLSCodec::load(const QString &file)
{
    QList<PlaylistItem> ret;

    qDebug("Playlist::load_pls");

    if (!QFile::exists(file)) {
        qDebug("Playlist::load_pls: '%s' doesn't exist, doing nothing", file.toUtf8().constData());
        return ret;
    }

    QString playlist_path = QFileInfo(file).path();

    QSettings set(file, QSettings::IniFormat);
    set.beginGroup("playlist");

    if (set.status() == QSettings::NoError) {
        QString filename;
        QString name;
        double duration;

        int num_items = set.value("NumberOfEntries", 0).toInt();
        if (num_items == 0) num_items = set.value("numberofentries", 0).toInt();

        for (int n=0; n < num_items; n++) {
            filename = set.value("File"+QString::number(n+1), "").toString();
            name = set.value("Title"+QString::number(n+1), "").toString();
            duration = (double) set.value("Length"+QString::number(n+1), 0).toInt();

            QFileInfo fi(filename);
            if (fi.exists()) {
                filename = fi.absoluteFilePath();
            }
            if (!fi.exists()) {
                if (QFileInfo::exists(playlist_path + "/" + filename)) {
                    filename = playlist_path + "/" + filename;
                }
            }
            ret.append(PlaylistItem{filename, name, duration});
        }
    }

    set.endGroup();
    return ret;
}

bool PLSCodec::save(const QString &file, const QList<PlaylistItem> &list)
{
    qDebug() << "Playlist::save_pls:" << file;

    QString dir_path = QFileInfo(file).path();
    if (!dir_path.endsWith("/")) dir_path += "/";

    dir_path = QDir::toNativeSeparators(dir_path);

    qDebug() << "Playlist::save_pls: dir_path:" << dir_path;

    QSettings set(file, QSettings::IniFormat);
    set.beginGroup("playlist");

    QString filename;

    for (int n = 0; n < list.count(); n++)
    {
        const auto &i = list.at(n);
        filename = QDir::toNativeSeparators(i.path.path());

        // Try to save the filename as relative instead of absolute
        if (filename.startsWith(dir_path)) {
            filename = filename.mid(dir_path.length());
        }

        set.setValue("File"+QString::number(n+1), filename);
        set.setValue("Title"+QString::number(n+1), i.label);
        set.setValue("Length"+QString::number(n+1), (int) i.duration);
    }

    set.setValue("NumberOfEntries", list.count());
    set.setValue("Version", 2);

    set.endGroup();
    set.sync();

    return set.status() == QSettings::NoError;
}
