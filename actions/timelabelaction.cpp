#include "timelabelaction.h"
#include <QLabel>
#include "helper.h"

TimeLabelAction::TimeLabelAction(TimeLabelType type, QWidget * parent)
    : QWidgetAction{parent}
{
    label_type = type;
    current_time = 0;
    total_time = 0;
    updateText();
}

void TimeLabelAction::setText(QString s)
{
    current_text = s;
    emit newText(s);
}

void TimeLabelAction::setCurrentTime(double t)
{
    current_time = t;
    updateText();
}

void TimeLabelAction::setTotalTime(double t)
{
    total_time = t;
    updateText();
}

void TimeLabelAction::updateText()
{
    QString ct = Helper::formatTime(current_time);
    QString tt = Helper::formatTime(total_time);
    QString rt;
    if (total_time < 1) rt = "00:00:00"; else rt = Helper::formatTime(total_time - current_time);

    switch (label_type) {
    case CurrentTime: setText(ct); break;
    case TotalTime: setText(tt); break;
    case CurrentAndTotalTime: setText(ct + " / " + tt); break;
    case RemainingTime: setText(rt); break;
    }
}

QWidget * TimeLabelAction::createWidget ( QWidget * parent ) {
    QLabel * time_label = new QLabel(parent);
    time_label->setObjectName("time_label");
    time_label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    if (current_text.isEmpty()) current_text = "00:00:00 / 00:00:00";
    time_label->setText(current_text);

    connect(this, &TimeLabelAction::newText, time_label, &QLabel::setText);

    return time_label;
}
