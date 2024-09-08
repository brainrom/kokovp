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

#include "autohidewidget.h"
#include <QTimer>
#include <QEvent>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QAction>
#include <QMenu>
#include <QDebug>

#include <QPropertyAnimation>

#define HANDLE_TAP_EVENT

#include <QGestureEvent>

AutohideWidget::AutohideWidget(QWidget * parent)
	: QWidget(parent)
	, turned_on(false)
	, auto_hide(false)
	, use_animation(false)
	, spacing(0)
	, perc_width(100)
	, activation_area(Anywhere)
	, internal_widget(0)
	, timer(0)
    , animation(0)
{
    setBackgroundRole(QPalette::Window);
    setAutoFillBackground(true);
    setLayoutDirection(Qt::LeftToRight);

    parent->installEventFilter(this);
    installFilter(parent);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &AutohideWidget::checkUnderMouse);
    timer->setInterval(3000);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);
}

AutohideWidget::~AutohideWidget() {
    if (animation) delete animation;
}

void AutohideWidget::setInternalWidget(QWidget *w) {
    //qDebug() << "AutohideWidget::setInternalWidget:" << w;
    layout()->addWidget(w);
    internal_widget = w;
}

void AutohideWidget::setHideDelay(int ms) {
    timer->setInterval(ms);
}

int AutohideWidget::hideDelay() {
    return timer->interval();
}

void AutohideWidget::installFilter(QObject *o) {
    for (auto &children : o->children())
    {
        if (!children->isWidgetType())
            continue;

        qDebug() << "AutohideWidget::installFilter: child name:" << children->objectName();
        QWidget *w = static_cast<QWidget*>(children);
        w->setMouseTracking(true);
        w->installEventFilter(this);
        installFilter(children);
        #ifdef HANDLE_TAP_EVENT
        //w->grabGesture(Qt::TapGesture);
        #endif
    }
}

bool AutohideWidget::visiblePopups() {
    //qDebug() << "AutohideWidget::visiblePopups: internal_widget:" << internal_widget;
    if (!internal_widget) return false;

    // Check if any of the menus in the internal widget is visible
    for (auto &child : internal_widget->children())
    {
        if (!child->isWidgetType())
            continue;

        //qDebug() << "AutohideWidget::visiblePopups:" << child << "child name:" << child->objectName();
        QWidget *w = static_cast<QWidget*>(child);

        for (auto &action : w->actions())
        {
            //qDebug() << "AutohideWidget::visiblePopups: action:" << action;
            //qDebug() << "AutohideWidget::visiblePopups: aw:" << aw;

            QMenu *menu = nullptr;
            for (auto &widget : action->associatedObjects())
            {
                //qDebug() << "AutohideWidget::visiblePopups: widget:" << widget;
                if ((menu = qobject_cast<QMenu*>(widget)))
                {
                        //qDebug() << "AutohideWidget::visiblePopups: menu:" << menu << "visible:" << menu->isVisible();
                        if (menu->isVisible()) return true;
                }
            }

            menu = action->menu();
            if (menu)
            {
                //qDebug() << "AutohideWidget::visiblePopups: menu:" << menu << "visible:" << menu->isVisible();
                if (menu->isVisible()) return true;
            }
        }
    }
    return false;
}

void AutohideWidget::activate() {
    turned_on = true;
    timer->start();
}

void AutohideWidget::deactivate() {
    turned_on = false;
    timer->stop();
    hide();
}

void AutohideWidget::show() {
    qDebug() << "AutohideWidget::show";
    resizeAndMove();

    if (use_animation)
        showAnimated();
    else
        QWidget::show();

    // Restart timer
    if (timer->isActive()) timer->start();
}

void AutohideWidget::setAutoHide(bool b) {
    auto_hide = b;
}

void AutohideWidget::checkUnderMouse() {
    if (!auto_hide)
        return;

    //qDebug("AutohideWidget::checkUnderMouse");
    if (isVisible() && !underMouse() && !visiblePopups())
            hide();
}

void AutohideWidget::resizeAndMove() {
    QWidget * widget = parentWidget();
    int w = widget->width() * perc_width / 100;
    int h = height();
    resize(w, h);

    int x = (widget->width() - width() ) / 2;
    int y = widget->height() - height() - spacing;
    move(x, y);
}

bool AutohideWidget::eventFilter(QObject * obj, QEvent * event) {
    if (turned_on)
    {
        //qDebug() << "AutohideWidget::eventFilter: obj:" << obj << "type:" << event->type();
        #ifdef HANDLE_TAP_EVENT
        if (event->type() == QEvent::Gesture) {
                qDebug() << "AutohideWidget::eventFilter: obj:" << obj << "gesture:" << event;
                QGestureEvent * gesture_event = static_cast<QGestureEvent*>(event);
                if (gesture_event->gesture(Qt::TapGesture)) {
                        qDebug() << "AutohideWidget::eventFilter: tap event detected";
                        if (!isVisible()) show(); //else hide();
                        event->setAccepted(true);
                        return true;
                }
        }
        else
        #endif
        if (event->type() == QEvent::MouseMove)
        {
            //qDebug() << "AutohideWidget::eventFilter: mouse move" << obj;
            if (!isVisible())
            {
                if (activation_area == Anywhere) {
                        show();
                } else {
                        QMouseEvent *mouse_event = dynamic_cast<QMouseEvent*>(event);
                        QWidget * parent = parentWidget();
                        QPoint p = parent->mapFromGlobal(mouse_event->globalPosition().toPoint());
                        //qDebug() << "AutohideWidget::eventFilter: y:" << p.y();
                        if (p.y() > (parent->height() - height() - spacing)) {
                                show();
                        }
                }
            }
        }
        else if (event->type() == QEvent::MouseButtonRelease && obj == this) {
                event->setAccepted(true);
                return true;
        }
        else if (event->type() == QEvent::Resize && isVisible()) // Resize and move widget when base widget is resized and internal widget is visible
        {
            show(); // Using show() instead of resizeAndMove() to restart animation, when using animation
        }
    }

    return QWidget::eventFilter(obj, event);
}

void AutohideWidget::showAnimated()
{
    if (!animation)
        animation = new QPropertyAnimation(this, "pos");

    QPoint initial_position = QPoint(pos().x(), parentWidget()->size().height());
    QPoint final_position = pos();
    move(initial_position);

    QWidget::show();

    animation->setDuration(300);
    animation->setEasingCurve(QEasingCurve::OutBounce);
    animation->setEndValue(final_position);
    animation->setStartValue(initial_position);
    animation->start();
}
