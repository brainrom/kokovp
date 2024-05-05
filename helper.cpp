#include "helper.h"
#include <QApplication>
#include <QFileDialog>
#include <QStandardPaths>

#include "extensions.h"
#include "cache.h"

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

QList<QUrl> Helper::openMediaFiles(QWidget *parent)
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
