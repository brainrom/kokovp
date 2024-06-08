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
#include "xspfcodec.h"
#include "playlistmodel.h"

#include <QDomDocument>
#include <QDebug>
#include <QFileInfo>
#include <QUrl>


// These functions has been copied (and modified a little bit) from SMPlayer (program under GPL license).
const QList<PlaylistItem> XSPFCodec::load(const QString &file)
{
    QList<PlaylistItem> ret;
    qDebug() << "Playlist::loadXSPF:" << file;

    QFile f(file);
    if (!f.open(QIODevice::ReadOnly))
        return ret;

    QDomDocument dom_document;
    bool ok = (bool)dom_document.setContent(f.readAll());
    qDebug() << "Playlist::loadXSPF: success:" << ok;
    if (!ok)
        return ret;

    QDomNode root = dom_document.documentElement();
    qDebug() << "Playlist::loadXSPF: tagname:" << root.toElement().tagName();

    QDomNode child = root.firstChildElement("trackList");
    if (!child.isNull())
    {
        qDebug() << "Playlist::loadXSPF: child:" << child.nodeName();
        QDomNode track = child.firstChildElement("track");
        while (!track.isNull()) {
            QString location = QUrl::fromPercentEncoding(track.firstChildElement("location").text().toLatin1());
            QString title = track.firstChildElement("title").text();
            int duration = track.firstChildElement("duration").text().toInt();

            qDebug() << "Playlist::loadXSPF: location:" << location;
            qDebug() << "Playlist::loadXSPF: title:" << title;
            qDebug() << "Playlist::loadXSPF: duration:" << duration;

            ret.append(PlaylistItem{location, title, (double) duration / 1000});
            track = track.nextSiblingElement("track");
        }
    }
    return ret;
}

bool XSPFCodec::save(const QString &file, const QList<PlaylistItem> &list)
{
    qDebug() << "Playlist::saveXSPF:" << file;

    QFile f(file);
    if (!f.open(QIODevice::WriteOnly))
        return false;

    QTextStream stream(&f);
    stream.setEncoding(QStringConverter::Utf8);

    stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    stream << "<playlist version=\"1\" xmlns=\"http://xspf.org/ns/0/\">\n";
    stream << "\t<trackList>\n";

    for (int n = 0; n < list.count(); n++)
    {
        const auto &i = list.at(n);
        QString location = i.path.toEncoded();
        qDebug() << "Playlist::saveXSPF:" << location;

        QString title = i.label;
        int duration = i.duration * 1000;

        location = location.toHtmlEscaped();
        title = title.toHtmlEscaped();

        stream << "\t\t<track>\n";
        stream << "\t\t\t<location>" << location << "</location>\n";
        stream << "\t\t\t<title>" << title << "</title>\n";
        stream << "\t\t\t<duration>" << duration << "</duration>\n";
        stream << "\t\t</track>\n";
    }

    stream << "\t</trackList>\n";
    stream << "</playlist>\n";
    return true;

}
