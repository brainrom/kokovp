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
#ifndef HELPER_H
#define HELPER_H

#include <QString>
#include <QUrl>
#include <QList>
#include <QDir>

class QWidget;

class Helper {
public:
	static QString formatTime(int secs);
    static QString timeForJumps(int secs);
    static const QList<QUrl> openMediaFiles(QWidget *parent);
    static const QList<QUrl> openExternalTracksFiles(QWidget *parent);
    static const QList<QUrl> openMediaDirectory(QWidget *parent);
    static const QList<QUrl> pathsToUrls(const QStringList &paths);
    static void searchWithMaxDepth(QStringList &outList, const QStringList &filter, QDir dir, int maxDepth = 2, bool searchForFiles = false, int depth = 0);
};

#endif
