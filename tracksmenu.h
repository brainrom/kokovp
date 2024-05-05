#ifndef TRACKSMENU_H
#define TRACKSMENU_H

#include <QMenu>
#include <QObject>

#include "playercontroller.h"


class TracksMenu : public QMenu
{
    Q_OBJECT
public:
    typedef QString (*RewriteRuleFunc)(const PlayerController::Track &t);

    TracksMenu(const QString &title, QWidget *parent = nullptr);
    void clear();
    void addTrack(const PlayerController::Track &t);
    void setCurrentId(int id);
    void setRewriteRule(RewriteRuleFunc f) { rRule = f; }
signals:
    void trackSwitched(int id);
private:
    static QString defRule(const PlayerController::Track &t);
    RewriteRuleFunc rRule;
    QActionGroup *trackAG;
    void handleAction(QAction *a);
    void addNoTrack();
    int lastId;
};

#endif // TRACKSMENU_H
