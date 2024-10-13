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
#include "tracksmenu.h"
#include <QActionGroup>

TracksMenu::TracksMenu(const QString &title, QWidget *parent) : QMenu{title, parent} {
    trackAG = new QActionGroup(this);
    trackAG->setExclusive(true);
    connect(trackAG, &QActionGroup::triggered, this, &TracksMenu::handleAction);

    noAction = new QAction(tr("No"));
    noAction->setData(-1);
    noAction->setCheckable(true);
    noAction->setChecked(true);

    trackAG->addAction(noAction);
    addAction(noAction);

    rRule = defRule;
}

void TracksMenu::clearTracks()
{
    QList<QAction*> acts = trackAG->actions();
    for (auto &a: acts)
    {
        if (a==noAction)
            continue;
        delete a;
    }
    trackAG->blockSignals(true);
    noAction->trigger();
    trackAG->blockSignals(false);
}

void TracksMenu::addTrack(const PlayerController::Track &t)
{
    QString rRule_res = rRule(t);
    QAction *trackAction = new QAction();
    // & is used for mnemonics in QAction, replace to display actual ampersand
    trackAction->setText((rRule_res.isNull() ?  defRule(t) : rRule_res).replace("&", "&&"));
    trackAction->setData(t.id);
    trackAction->setCheckable(true);

    trackAG->addAction(trackAction);
    addAction(trackAction);
    if (t.id==lastId)
        trackAction->trigger();
}

void TracksMenu::setCurrentId(int id)
{
    lastId = id;
    for (auto &a : trackAG->actions())
    {
        if (a->data().toInt()==id)
            return a->trigger();
    }
}

void TracksMenu::handleAction(QAction *a)
{
    int newId = a->data().toInt();
    if (newId!=lastId)
    {
        lastId=newId;
        qDebug() << "trackSwitched" << newId;
        emit trackSwitched(newId);
    }
}

QString TracksMenu::defRule(const PlayerController::Track &t)
{
    if (!t.title.isEmpty())
        return t.title;

    if (!t.lang.isEmpty())
        return t.lang;

    return QString::number(t.id);
}
