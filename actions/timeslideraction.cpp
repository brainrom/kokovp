#include "timeslideraction.h"
#include "timeslider.h"

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

QWidget * TimeSliderAction::createWidget ( QWidget * parent ) {
    TimeSlider *t = new TimeSlider(parent);
    t->setEnabled(isEnabled());
    t->setWheelDelta(wheelDelta);

    connect(t, &TimeSlider::timeChanged, this, &TimeSliderAction::timeChanged);
    connect(t, &TimeSlider::draggingTime, this, &TimeSliderAction::draggingTime);

    return t;
}
