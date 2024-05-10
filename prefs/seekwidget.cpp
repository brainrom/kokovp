/*  This is part of KokoVP

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

#include "seekwidget.h"
#include <QLabel>
#include <QDateTimeEdit>
#include <QBoxLayout>

SeekWidget::SeekWidget( QWidget* parent, Qt::WindowFlags f)
	: QWidget(parent, f)
{
    QHBoxLayout *hboxLayout = new QHBoxLayout(this);
    hboxLayout->setSpacing(4);
    hboxLayout->setContentsMargins(0, 0, 0, 0);
    icon_label = new QLabel(this);
    icon_label->setWordWrap(false);
    hboxLayout->addWidget(icon_label);

    text_label = new QLabel(this);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(text_label->sizePolicy().hasHeightForWidth());
    text_label->setSizePolicy(sizePolicy);
    text_label->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
    text_label->setWordWrap(false);
    hboxLayout->addWidget(text_label);

    time_edit = new QTimeEdit(this);
    hboxLayout->addWidget(time_edit);

	time_edit->setDisplayFormat("mm:ss");
	time_edit->setMinimumTime(QTime(0,0,1));
	time_edit->setMaximumTime(QTime(0,59,59));
}

void SeekWidget::setIcon(QIcon icon) {
	icon_label->setText("");
    icon_label->setPixmap(icon.pixmap(QSize(32,32)));
}

void SeekWidget::setLabel(QString text) {
	text_label->setText(text);
}

void SeekWidget::setTime(int secs) {
	QTime t(0,0);
	time_edit->setTime(t.addSecs(secs));
}

int SeekWidget::time() const {
	QTime t = time_edit->time();
	return (t.minute() * 60) + t.second();
}
