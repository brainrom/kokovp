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

#ifndef TIMESLIDER_H
#define TIMESLIDER_H

#include <QSlider>

class TimeSlider : public QSlider
{
	Q_OBJECT

public:
	TimeSlider( QWidget * parent );

public slots:
    void setWheelDelta(double s) { wheelDelta = s; }
    void setTime(double s); // Don't use setValue!
    double time();
    void setDuration(double t);
    double duration() { return totalTime; };

signals:
    void timeChanged(double s);
    void draggingTime(double s);

protected slots:
	void sliderPressed_slot();
	void sliderReleased_slot();
	void valueChanged_slot(int);

protected:
	virtual void wheelEvent(QWheelEvent * e);
	virtual bool event(QEvent *event);

private:
    int pixelPosToRangeValue(int pos) const;

    bool dragging;
    double wheelDelta;
    double totalTime;
};

#endif

