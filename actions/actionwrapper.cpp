#include "actionwrapper.h"
#include <QWidget>

ActionWrapper::ActionWrapper(const QString & text, QKeySequence defaultShortcut,
                   QObject * parent, const char * name, const QIcon & icon)
	: QAction(parent)
{
	setObjectName(name);
	setText(text);
    setIcon(icon);
    setShortcut(defaultShortcut);
    addActionToParent();
}

void ActionWrapper::addActionToParent() {
	if (parent()) {
        if (parent()->inherits("QWidget")) {
            QWidget *w = static_cast<QWidget*> (parent());
            w->addAction(this);
            if (w->parentWidget() && w->parentWidget()->window())
                w->parentWidget()->window()->addAction(this);
		}
	}
}
