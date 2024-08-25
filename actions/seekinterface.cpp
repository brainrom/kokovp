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

    QList<QIcon> rewindIcons = { QIcon::fromTheme("rewind10s"), QIcon::fromTheme("rewind1m"), QIcon::fromTheme("rewind10m")  };
    actsCount = qMax(actsCount, rewindIcons.size());

    QList<QIcon> forwardIcons = { QIcon::fromTheme("forward10s"), QIcon::fromTheme("forward1m"), QIcon::fromTheme("forward10m")  };
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
