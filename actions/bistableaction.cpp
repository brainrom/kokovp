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
