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
#include "incdecactions.h"
#include <QAction>
#include <QActionGroup>

IncDecActionsPair::IncDecActionsPair(QObject *parent)
    : QObject{parent}
{
    acts = new QActionGroup(this);
    connect(acts, &QActionGroup::triggered, this, &IncDecActionsPair::handleActions);
    p_incAction = nullptr;
    p_decAction = nullptr;
    delta = 1;
}

void IncDecActionsPair::setDelta(double newDelta)
{
    newDelta = abs(newDelta);
    if (newDelta==0)
        newDelta = 1;

    delta = newDelta;

    if (p_incAction && p_decAction)
    {
        p_incAction->setData(delta);
        p_decAction->setData(-delta);
    }
}

void IncDecActionsPair::setIncAction(QAction *a)
{
    if (p_incAction)
        return;
    p_incAction = a;
    p_incAction->setData(delta);
    acts->addAction(p_incAction);
}

void IncDecActionsPair::setDecAction(QAction *a)
{
    if (p_decAction)
        return;
    p_decAction = a;
    p_decAction->setData(-delta);
    acts->addAction(p_decAction);
}

const QList<QAction*> IncDecActionsPair::actions()
{
    return QList<QAction*> {p_incAction, p_decAction};
}

void IncDecActionsPair::handleActions(QAction *a)
{
    emit valueChanged(a->data().toDouble());
}
