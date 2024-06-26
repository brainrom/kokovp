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
