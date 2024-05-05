#include "volumeslideraction.h"

VolumeSliderAction::VolumeSliderAction( QWidget * parent )
    : QWidgetAction(parent)
{
    tick_position = QSlider::TicksBelow;
}

void VolumeSliderAction::setValue(int v) {
    for (auto &w: createdWidgets())
    {
        QSlider *s = (QSlider*) w;
        bool was_blocked = s->blockSignals(true);
        s->setValue(v);
        s->blockSignals(was_blocked);
    }
}

int VolumeSliderAction::value() {
    QList<QWidget *> l = createdWidgets();
    if (l.count() >= 1) {
        QSlider *s = (QSlider*)l.at(0);
        return s->value();
    } else {
        return 0;
    }
}

void VolumeSliderAction::setTickPosition(QSlider::TickPosition position) {
    // For new widgets
    tick_position = position;

    // Propagate changes to all existing widgets
    QList<QWidget *> l = createdWidgets();
    for (auto &w: l)
    {
        QSlider *s = (QSlider*) w;
        s->setTickPosition(tick_position);
    }
}

QWidget * VolumeSliderAction::createWidget ( QWidget * parent ) {
    QSlider *t = new QSlider(parent);

    t->setMinimum(0);
    t->setMaximum(100);
    t->setValue(50);
    t->setOrientation( Qt::Horizontal );
    t->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    t->setFocusPolicy( Qt::NoFocus );
    t->setTickPosition( tick_position );
    t->setTickInterval( 10 );
    t->setSingleStep( 1 );
    t->setPageStep( 10 );
    t->setToolTip( tr("Volume") );
    t->setEnabled( isEnabled() );
    t->setAttribute(Qt::WA_NoMousePropagation);

    connect(t, &QSlider::valueChanged, this, &VolumeSliderAction::valueChanged);

    return t;
}
