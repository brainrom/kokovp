#include "seekinterface.h"
#include "actionwrapper.h"
#include <QActionGroup>
#include "helper.h"

SeekInterface::SeekInterface(QMenu *parent)
    : QObject{parent}
{
    seekGroup = new QActionGroup(this);
    connect(seekGroup, &QActionGroup::triggered, this, &SeekInterface::handleSeekAction);

    actsCount = 0;

    QList<QKeySequence> rewindShortcuts = { Qt::Key_Left, Qt::Key_Down,  Qt::Key_PageDown };
    actsCount = qMax(actsCount, rewindShortcuts.size());
    QList<QKeySequence> forwardShortcuts = { Qt::Key_Right, Qt::Key_Up,  Qt::Key_PageUp };
    actsCount = qMax(actsCount, forwardShortcuts.size());

    QList<QIcon> rewindIcons = { QIcon(":/icons/default/rewind10s"), QIcon(":/icons/default/rewind1m"), QIcon(":/icons/default/rewind10m")  };
    actsCount = qMax(actsCount, rewindIcons.size());

    QList<QIcon> forwardIcons = { QIcon(":/icons/default/forward10s"), QIcon(":/icons/default/forward1m"), QIcon(":/icons/default/forward10m")  };
    actsCount = qMax(actsCount, forwardIcons.size());

    // All these lists should be same size
    assert(rewindShortcuts.length()==actsCount);
    assert(forwardShortcuts.length()==actsCount);
    assert(rewindIcons.length()==actsCount);
    assert(forwardIcons.length()==actsCount);

    for (int i=0; i<actsCount; i++)
    {
        QAction *rewindAct = new ActionWrapper("", rewindShortcuts.at(i), parent, QString("seek%1_minus").arg(i).toUtf8(), rewindIcons.at(i));
        QAction *forwardAct = new ActionWrapper("", forwardShortcuts.at(i), parent, QString("seek%1_plus").arg(i).toUtf8(), forwardIcons.at(i) );
        seekGroup->addAction(rewindAct);
        seekGroup->addAction(forwardAct);
    }
}

void SeekInterface::setStepTime(int step, int s)
{
    s = abs(s);
    if (step>=actsCount)
        return;

    const QList<QAction*> seekActs = seekGroup->actions();

    seekActs.at(step*2)->setData(-s);
    seekActs.at(step*2)->setText("- "+Helper::timeForJumps(s));
    seekActs.at((step*2)+1)->setData(s);
    seekActs.at((step*2)+1)->setText("+ "+Helper::timeForJumps(s));
}

void SeekInterface::handleSeekAction(QAction *action)
{
    bool ok = false;
    int seek_val = action->data().toInt(&ok);
    if (ok)
        emit seek(seek_val);
}
