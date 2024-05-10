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
