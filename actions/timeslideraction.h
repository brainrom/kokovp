/*  This file was part of smplayer and now part of KokoVP
    smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2023 Ricardo Villalba <ricardo@smplayer.info>

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
#ifndef TIMESLIDERACTION_H
#define TIMESLIDERACTION_H

#include <QWidgetAction>
#include <QWidget>

class TimeSliderAction : public QWidgetAction
{
    Q_OBJECT
public:
    TimeSliderAction(QWidget * parent) : QWidgetAction{parent} {};

public slots:
    void setTime(double t);
    double time();
    void setDuration(double t);
    double duration() { return totalTime; };

    void setWheelDelta(double t);

private:
    double totalTime;
    double wheelDelta;
    QWidget *createWidget (QWidget * parent) override;

signals:
    void timeChanged(double value);
    void draggingTime(double value);
};


#endif // TIMESLIDERACTION_H
