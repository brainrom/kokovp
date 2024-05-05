#ifndef VOLUMESLIDERACTION_H
#define VOLUMESLIDERACTION_H

#include <QWidgetAction>
#include <QSlider>

class VolumeSliderAction : public QWidgetAction
{
    Q_OBJECT

public:
    VolumeSliderAction( QWidget * parent );

    // Really don't know if I ever want this
    void setTickPosition(QSlider::TickPosition position);
    QSlider::TickPosition tickPosition() { return tick_position; };

public slots:
    void setValue(int);
    int value();

signals:
    void valueChanged(int value);

protected:
    QWidget * createWidget ( QWidget * parent );

private:
    QSlider::TickPosition tick_position;
};
#endif // VOLUMESLIDERACTION_H
