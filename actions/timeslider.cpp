/*  smplayer, GUI front-end for mplayer.
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

#include "timeslider.h"
#include "helper.h"

#include <QWheelEvent>
#include <QTimer>
#include <QToolTip>
#include <QStyleOption>
#include <QDebug>

TimeSlider::TimeSlider( QWidget * parent ) : QSlider(parent)
    , dragging(false)
    , totalTime(0)
{
	setMinimum(0);
    setMaximum(1000);
    wheelDelta = 0;

    setOrientation(Qt::Horizontal);
    setFocusPolicy(Qt::NoFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    connect(this, &TimeSlider::sliderPressed, this, &TimeSlider::sliderPressed_slot);
    connect(this, &TimeSlider::sliderReleased, this, &TimeSlider::sliderReleased_slot);
    connect(this, &TimeSlider::valueChanged, this, &TimeSlider::valueChanged_slot);
}

void TimeSlider::sliderPressed_slot() {
    dragging = true;
}

void TimeSlider::sliderReleased_slot() {
    emit timeChanged( time() );
    dragging = false;
}

void TimeSlider::valueChanged_slot(int v) {
    if (dragging) {
        emit draggingTime(time());
    }
}

void TimeSlider::setTime(double s) {
    if (!dragging) {
        int range = maximum() - minimum();
        setValue(s * range / totalTime);
    }
}

double TimeSlider::time() {
    int range = maximum() - minimum();
    return value() * totalTime / range;
}

void TimeSlider::wheelEvent(QWheelEvent * e) {
    if (wheelDelta>0 && e->angleDelta().y() >= 0)
        emit timeChanged(time()+wheelDelta);
    else
        emit timeChanged(time()-wheelDelta);
}

int TimeSlider::pixelPosToRangeValue(int pos) const {
    assert(orientation() == Qt::Horizontal);
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    QRect gr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);
    QRect sr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
    int sliderMin, sliderMax, sliderLength;

    sliderLength = sr.width();
    sliderMin = gr.x();
    sliderMax = gr.right() - sliderLength + 1;

    return QStyle::sliderValueFromPosition(
        minimum(), maximum(), pos - sliderMin,
        sliderMax - sliderMin, opt.upsideDown);
}

bool TimeSlider::event(QEvent *event) {
	if (event->type() == QEvent::ToolTip) {
		QHelpEvent * help_event = static_cast<QHelpEvent *>(event);

		QStyleOptionSlider opt;
		initStyleOption(&opt);
		const QRect sliderRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
		const QPoint center = sliderRect.center() - sliderRect.topLeft();

		int value = pixelPosToRangeValue(help_event->x() - center.x());
		int range = maximum() - minimum();
        qreal time = value * totalTime / range;

        if (time >= 0 && time <= totalTime) {
			QToolTip::showText(help_event->globalPos(), Helper::formatTime(time), this);
		} else {
			QToolTip::hideText();
			event->ignore();
		}
		return true;
	}
	return QWidget::event(event);
}
void TimeSlider::setDuration(double t) {
    totalTime = t;
    setEnabled(t>0);
};
