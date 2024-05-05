#include "tracksmenu.h"
#include <QActionGroup>

TracksMenu::TracksMenu(const QString &title, QWidget *parent) : QMenu{title, parent} {
    trackAG = new QActionGroup(this);
    trackAG->setExclusive(true);
    connect(trackAG, &QActionGroup::triggered, this, &TracksMenu::handleAction);
    addNoTrack();
    setCurrentId(-1);
    rRule = defRule;
}

void TracksMenu::clear()
{
    QList<QAction*> acts = trackAG->actions();
    for (auto &a: acts)
        delete a;
    addNoTrack();
    setCurrentId(-1);
}

void TracksMenu::addTrack(const PlayerController::Track &t)
{
    QString rRule_res = rRule(t);
    QAction *trackAction = new QAction(rRule_res.isNull() ?  defRule(t) : rRule_res);
    trackAction->setData(t.id);
    trackAction->setCheckable(true);

    trackAG->addAction(trackAction);
    addAction(trackAction);
}

void TracksMenu::setCurrentId(int id)
{
    lastId = id;
    for (auto &a : trackAG->actions())
    {
        if (a->data()==id)
            a->trigger();
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

void TracksMenu::handleAction(QAction *a)
{
    int newId = a->data().toInt();
    if (newId!=lastId)
    {
        lastId=newId;
        emit trackSwitched(newId);
    }
}

void TracksMenu::addNoTrack()
{
    QAction *trackAction = new QAction(tr("No"));
    trackAction->setData(-1);
    trackAction->setCheckable(true);
    trackAction->setChecked(true);

    trackAG->addAction(trackAction);
    addAction(trackAction);
}
