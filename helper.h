#ifndef HELPER_H
#define HELPER_H

#include <QString>
#include <QUrl>
#include <QList>

class QWidget;

class Helper {
public:
	static QString formatTime(int secs);
    static QString timeForJumps(int secs);
    static QList<QUrl> openMediaFiles(QWidget *parent);
};

#endif
