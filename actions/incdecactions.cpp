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
