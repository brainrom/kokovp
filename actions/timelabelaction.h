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
#ifndef TIMELABELACTION_H
#define TIMELABELACTION_H

#include <QWidgetAction>

class TimeLabelAction : public QWidgetAction
{
    Q_OBJECT
public:
    enum TimeLabelType { CurrentTime = 0, TotalTime = 1, CurrentAndTotalTime = 2, RemainingTime = 3 };
    TimeLabelAction(TimeLabelType type, QWidget * parent);

public slots:
    void setText(QString s);
    void setCurrentTime(double);
    void setTotalTime(double);

signals:
    void newText(QString s);

protected:
    QWidget *createWidget(QWidget *parent);
    void updateText();

private:
    QString current_text;
    double current_time, total_time;
    TimeLabelType label_type;
};

#endif // TIMELABELACTION_H
