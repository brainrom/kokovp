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
