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
#include "timeslideraction.h"
#include "timeslider.h"

TimeSliderAction::TimeSliderAction(QWidget *parent) : QWidgetAction{parent}, changeTimeOnDrag(false), totalTime(0), wheelDelta(0)
{
};

void TimeSliderAction::setTime(double v) {
    for (auto &w: createdWidgets())
    {
        TimeSlider *s = (TimeSlider*) w;
        bool was_blocked = s->blockSignals(true);
        s->setTime(v);
        s->blockSignals(was_blocked);
    }
}

double TimeSliderAction::time() {
    QList<QWidget *> l = createdWidgets();
    if (l.count() >= 1) {
        TimeSlider *s = (TimeSlider*) l.at(0);
        return s->time();
    } else {
        return -1;
    }
}

void TimeSliderAction::setDuration(double t) {
    totalTime = t;

    for (auto &w: createdWidgets())
    {
        TimeSlider *s = (TimeSlider*) w;
        s->setDuration(t);
    }
}

void TimeSliderAction::setWheelDelta(double t)
{
    wheelDelta = t;

    for (auto &w: createdWidgets())
    {
        TimeSlider *s = (TimeSlider*) w;
        s->setWheelDelta(t);
    }
}

QWidget *TimeSliderAction::createWidget(QWidget *parent) {
    TimeSlider *t = new TimeSlider(parent);
    t->setEnabled(isEnabled());
    t->setWheelDelta(wheelDelta);

    connect(t, &TimeSlider::timeChanged, this, &TimeSliderAction::onTimeChanged);
    connect(t, &TimeSlider::draggingTime, this, &TimeSliderAction::onDraggingTime);
    connect(t, &TimeSlider::draggingTime, this, &TimeSliderAction::draggingTime);

    return t;
}

void TimeSliderAction::setChangeTimeOnDrag(bool on)
{
    changeTimeOnDrag = on;
}

void TimeSliderAction::onTimeChanged(double value)
{
    if (!changeTimeOnDrag)
        emit timeChanged(value);
}

void TimeSliderAction::onDraggingTime(double value)
{
    if (changeTimeOnDrag)
        emit timeChanged(value);
}
