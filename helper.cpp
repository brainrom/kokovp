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
#include "helper.h"
#include <QApplication>
#include <QFileDialog>
#include <QStandardPaths>

#include "extensions.h"
#include "cache.h"
#include "config.h"

QString Helper::formatTime(int secs) {
	bool negative = (secs < 0);
	secs = qAbs(secs);

	int t = secs;
	int hours = (int) t / 3600;
	t -= hours*3600;
	int minutes = (int) t / 60;
	t -= minutes*60;
	int seconds = t;

	return QString("%1%2:%3:%4").arg(negative ? "-" : "").arg(hours, 2, 10, QChar('0')).arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
}

QString Helper::timeForJumps(int secs) {
    int minutes = (int) secs / 60;
	int seconds = secs % 60;

	if (minutes==0) {
        return QApplication::translate("Helper", "%n second(s)", "", seconds);
	} else {
		if (seconds==0) 
            return QApplication::translate("Helper", "%n minute(s)", "", minutes);
		else {
            QString m = QApplication::translate("Helper", "%n minute(s)", "", minutes);
            QString s = QApplication::translate("Helper", "%n second(s)", "", seconds);
            return QApplication::translate("Helper", "%1 and %2").arg(m, s);
		}
	}
}

const QList<QUrl> Helper::openMediaFiles(QWidget *parent)
{
    QList<QUrl> s = QFileDialog::getOpenFileUrls(
    parent, QApplication::translate("Helper", "Choose a file"), Cache::i().get("file_open/last_file_dir", QStandardPaths::standardLocations(QStandardPaths::MoviesLocation)).toString(),
    QApplication::translate("Helper", "Multimedia") + Extensions.allPlayable().forFilter()+";;" +
        QApplication::translate("Helper", "Video") + Extensions.video().forFilter()+";;" +
        QApplication::translate("Helper", "Audio") + Extensions.audio().forFilter()+";;" +
        QApplication::translate("Helper", "All files") +" (*.*)" );

    if (s.count()>0 && QFile::exists(s.at(0).toLocalFile()))
    {
        Cache::i().set("file_open/last_dir", QFileInfo(s.at(0).toLocalFile()).absolutePath());
    }
    return s;
}

const QList<QUrl> Helper::openMediaDirectory(QWidget *parent)
{
    QString dirPath = QFileDialog::getExistingDirectory(parent, QApplication::translate("Helper", "Choose a directory"),
                                                        Cache::i().get("file_open/last_file_dir", QStandardPaths::standardLocations(QStandardPaths::MoviesLocation)).toString());
    if (dirPath.isEmpty())
        return QList<QUrl>();

    QDir dir(dirPath);

    if (!dir.exists())
        return QList<QUrl>();

    Cache::i().set("file_open/last_dir", dir.absolutePath());

    QStringList paths;
    searchWithMaxDepth(paths, Extensions.multimedia().forDirFilter(), dir, Config::i().get("file_open/open_dir_depth", 4).toInt(), true);

    return pathsToUrls(paths);
}

void Helper::searchWithMaxDepth(QStringList &outList, const QStringList &filter, QDir dir, int maxDepth, bool searchForFiles, int depth)
{
    if (depth>maxDepth)
        return;

    dir.setNameFilters(filter);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);

    QFileInfoList fileList = dir.entryInfoList();
    if (fileList.size()>0)
    {
        if (searchForFiles)
        {
            for (auto &f : fileList)
                outList.append(f.absoluteFilePath());
        }
        else
            outList.append(dir.path());
    }

    dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
    QFileInfoList dirList = dir.entryInfoList();
    for (auto &e : dirList)
        searchWithMaxDepth(outList, filter, QDir(e.filePath()), maxDepth, searchForFiles, depth+1);
}

const QList<QUrl> Helper::pathsToUrls(const QStringList &paths)
{
    QList<QUrl> ret;
    for (auto &arg : paths)
    {
        QFileInfo f(arg);
        if (f.exists())
            ret.append(QUrl::fromLocalFile(f.absoluteFilePath()));
    }
    return ret;
}
