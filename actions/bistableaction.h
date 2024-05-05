#ifndef PLAYPAUSEACTION_H
#define PLAYPAUSEACTION_H

#include "actionwrapper.h"

class BistableAction : public ActionWrapper
{
    Q_OBJECT
public:
    explicit BistableAction(QKeySequence defaultShortcut,
                             QObject * parent, const char * name);
    void switchState(bool isPlaying);
    void setActiveState(QString label, QIcon icon = QIcon());
    void setPassiveState(QString label, QIcon icon = QIcon());
private:
    QString p_activeText, p_passiveText;
    QIcon p_activeIcon, p_passiveIcon;
};

#endif // PLAYPAUSEACTION_H
