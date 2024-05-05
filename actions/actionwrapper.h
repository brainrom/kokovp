#ifndef ACTIONWRAPPER_H
#define ACTIONWRAPPER_H

#include <QAction>
#include <QString>
#include <QIcon>
#include <QKeySequence>

class ActionWrapper : public QAction
{
public:
    ActionWrapper (const QString & text, QKeySequence defaultShortcut,
                  QObject * parent, const char * name = "", const QIcon & icon = QIcon());
protected:
	void addActionToParent();
};

#endif

