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
#ifndef INCDECACTIONS_H
#define INCDECACTIONS_H

#include <QObject>
#include <QIcon>

class QAction;
class QActionGroup;

class IncDecActionsPair : public QObject
{
    Q_OBJECT
public:
    explicit IncDecActionsPair(QObject *parent = nullptr);

    void setDelta(double delta);
    void setIncAction(QAction *a);
    void setDecAction(QAction *a);

    const QList<QAction*> actions();
    const QAction *incAction() const { return p_incAction; }
    const QAction *decAction() const { return p_decAction; }

signals:
    void valueChanged(double delta);
private:
    void handleActions(QAction *a);
    double delta;
    QAction *p_incAction;
    QAction *p_decAction;
    QActionGroup *acts;
};

#endif // INCDECACTIONS_H
