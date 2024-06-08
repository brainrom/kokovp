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
#include "m3ucodec.h"
#include "playlistmodel.h"

#include <QDebug>
#include <QFileInfo>
#include <QTextStream>
#include <QRegularExpression>
#include <QDir>

// These functions has been copied (and modified a little bit) from SMPlayer (program under GPL license).
// Refactored using early continues and QRegularExpression by Kokokoshka

const QList<PlaylistItem> M3UCodec::load(const QString &file)
{
    static const QRegularExpression m3u_id("^#EXTM3U|^#M3U");
    static const QRegularExpression rx_info("^#EXTINF:([.\\d]+).*tvg-logo=\"(.*)\",(.*)");

    bool utf8 = (QFileInfo(file).suffix().toLower() == "m3u8");

    qDebug() << "Playlist::load_m3u: utf8:" << utf8;
    QList<PlaylistItem> ret;

    QFile f(file);
    if (!f.open(QIODevice::ReadOnly))
        return ret;

    QTextStream stream(&f);

    QString filename;
    QString name;
    double duration=0;
    QStringList extra_params;
    QString icon_url;

    QString playlist_path = QFileInfo(file).path();


    stream.setEncoding(utf8 ? QStringConverter::Utf8 : QStringConverter::System);

    QString line;
    while (!stream.atEnd())
    {
        line = stream.readLine().trimmed();
        if (line.isEmpty()) continue;

        qDebug() << "Playlist::load_m3u: line:" << line;
        if (m3u_id.match(line).hasMatch())
            continue;

        auto match = rx_info.match(line);
        if (match.hasMatch())
        {
            duration = match.captured(1).toDouble();
            name = match.captured(3);
            icon_url = match.captured(2);
            qDebug() << "Playlist::load_m3u: name:" << name << "duration:" << duration << "icon_url:" << icon_url;
            continue;
        }

        if (line.startsWith("#EXTINF:"))
        {
            QStringList fields = line.mid(8).split(",");
            //qDebug() << "Playlist::load_m3u: fields:" << fields;
            if (fields.count() >= 1) duration = fields[0].toDouble();
            if (fields.count() >= 2) name = fields[1];
            continue;
        }
        if (line.startsWith("#EXTVLCOPT:"))
        {
            QString par = line.mid(11);
            qDebug() << "Playlist::load_m3u: EXTVLCOPT:" << par;
            extra_params << par;
            continue;
        }
        if (line.startsWith("#"))
            continue; // Ignore comment

        filename = line;
        QFileInfo fi(filename);
        if (fi.exists()) {
            filename = fi.absoluteFilePath();
        }
        if (!fi.exists()) {
            if (QFileInfo::exists(playlist_path + "/" + filename)) {
                filename = playlist_path + "/" + filename;
            }
        }
        name.replace("&#44;", ",");
        //qDebug() << "Playlist::load_m3u: extra_params:" << extra_params;
        ret.append(PlaylistItem{filename, name, duration}); //TODO: extra_params, icon_url
        name = "";
        duration = 0;
        extra_params.clear();
        icon_url = "";
    }
    f.close();

    return ret;
}

bool M3UCodec::save(const QString &file, const QList<PlaylistItem> &list)
{
    qDebug() << "Playlist::save_m3u:" << file;

    QString dir_path = QFileInfo(file).path();
    if (!dir_path.endsWith("/")) dir_path += "/";

    dir_path = QDir::toNativeSeparators(dir_path);
    qDebug() << "Playlist::save_m3u: dir_path:" << dir_path;

    bool utf8 = (QFileInfo(file).suffix().toLower() == "m3u8");

    QFile f(file);
    if (!f.open(QIODevice::WriteOnly))
        return false;

    QTextStream stream(&f);
    stream.setEncoding(utf8 ? QStringConverter::Utf8 : QStringConverter::System);

    QString filename;
    QString name;

    stream << "#EXTM3U" << "\n";
    stream << "# Playlist created by KokoVP " << /*Version::printable() <<*/ " \n";  //TODO: version

    for (const auto &i : list) {
        filename = QDir::toNativeSeparators(i.path.path());
        name = i.label;
        name.replace(",", "&#44;");
        QString icon_url; //TODO icon_url
        stream << "#EXTINF:";
        stream << i.duration;
        if (!icon_url.isEmpty()) stream << " tvg-logo=\"" + icon_url + "\"";
        stream << ",";
        stream << name << "\n";

        // Save extra params
        QStringList extra_params; //TODO: extra_params
        foreach(QString par, extra_params) {
            stream << "#EXTVLCOPT:" << par << "\n";
        }

        // Try to save the filename as relative instead of absolute
        if (filename.startsWith(dir_path)) {
            filename = filename.mid(dir_path.length());
        }
        stream << filename << "\n";
    }
    f.close();
    return true;
}
