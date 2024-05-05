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
