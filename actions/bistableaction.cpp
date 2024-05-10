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
#include "bistableaction.h"
#include <QWidget>

BistableAction::BistableAction(QKeySequence defaultShortcut,
                                 QObject * parent, const char * name)
    : ActionWrapper{"", defaultShortcut, parent, name}
{}

void BistableAction::switchState(bool active)
{
    setText(active ? p_activeText : p_passiveText);
    setIcon(active ? p_activeIcon : p_passiveIcon);
}

void BistableAction::setActiveState(QString label, QIcon icon)
{
    p_activeIcon = icon;
    p_activeText = label;
}

void BistableAction::setPassiveState(QString label, QIcon icon)
{
    p_passiveIcon = icon;
    p_passiveText = label;
    switchState(false);
}
