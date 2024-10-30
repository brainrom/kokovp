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
#include "playerwidget.h"
#include <QPainter>
#include <QDropEvent>
#include <QMimeData>
#include <QApplication>

PlayerWidget::PlayerWidget(QWidget *parent, Qt::WindowFlags f) : MpvWidget{parent, f}
{
    paintLogo = true;
    logoFont = QFont(QFont().family(), 30);
    setAcceptDrops(true);
    setMouseTracking(true);

    doubleClickTimer.setSingleShot(true);
    clickAct = nullptr;
    doubleClickAct = nullptr;
    wheelUpAct = nullptr;
    wheelDownAct = nullptr;

    mouseMoveTimer = new QTimer(this);
    mouseMoveTimer->setInterval(3000);
    mouseMoveTimer->setSingleShot(true);
    connect(mouseMoveTimer, &QTimer::timeout, this, &PlayerWidget::updateCursorVisibility);

    connect(this, &MpvWidget::fileLoaded, this, &PlayerWidget::hideLogo);
    connect(this, &MpvWidget::endFile, this, &PlayerWidget::showLogo);
}

void PlayerWidget::setPaintLogo(bool on)
{
    paintLogo = on;
    update();
    updateCursorVisibility();
}

void PlayerWidget::setAllowHideCursor(bool on)
{
    allowHideCursor = on;
    updateCursorVisibility();
}

void PlayerWidget::updateCursorVisibility()
{
    if (!paintLogo && allowHideCursor)
    {
        if (sender()==mouseMoveTimer) // A little hack to catch timer's signal
            setCursor(Qt::BlankCursor);
        else
            mouseMoveTimer->start();
    }
    else
        setCursor(Qt::ArrowCursor);
}

void PlayerWidget::setClickAction(QAction *newClickAction)
{
    if (clickAct)
        disconnect(&doubleClickTimer, &QTimer::timeout, clickAct, &QAction::trigger);

    clickAct = newClickAction;
    connect(&doubleClickTimer, &QTimer::timeout, clickAct, &QAction::trigger);
}

void PlayerWidget::setDoubleClickAction(QAction *newDoubleClickAction)
{
    doubleClickAct = newDoubleClickAction;
}

void PlayerWidget::setRightClickAction(QAction *newRightClickAction)
{
    rightClickAct = newRightClickAction;
}

void PlayerWidget::setWheelActions(QAction *newWheelUpAct, QAction *newWheelDownAct)
{
    if (newWheelUpAct && newWheelDownAct)
    {
        wheelUpAct = newWheelUpAct;
        wheelDownAct = newWheelDownAct;
    }
}

void PlayerWidget::paintOverlay(QPainter &painter)
{
    if (!paintLogo)
        return;

    painter.fillRect(rect(), Qt::black);
    painter.setPen(QPen(Qt::white));
    painter.setFont(logoFont);
    painter.drawText(rect(), Qt :: AlignHCenter | Qt :: AlignVCenter,  "KokoVP");
}

void PlayerWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void PlayerWidget::dropEvent(QDropEvent *event)
{
    if (!event->mimeData()->hasUrls())
        return;

    emit draggedURLS(event->mimeData()->urls());
    event->acceptProposedAction();
}

void PlayerWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button()==Qt::LeftButton)
        doubleClickTimer.start(QApplication::doubleClickInterval());
    if (event->button()==Qt::RightButton && rightClickAct)
        rightClickAct->trigger();
}

void PlayerWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button()==Qt::LeftButton && doubleClickAct)
    {
        doubleClickTimer.stop();
        doubleClickAct->trigger();
    }
}

void PlayerWidget::wheelEvent(QWheelEvent *e)
{
    if (wheelUpAct && wheelDownAct)
    {
        if (e->angleDelta().y() >= 0) wheelUpAct->trigger(); else wheelDownAct->trigger();
    }
}

void PlayerWidget::mouseMoveEvent(QMouseEvent *event)
{
    mouseMoveTimer->start();
    setCursor(Qt::ArrowCursor);
}

void PlayerWidget::enterEvent(QEnterEvent *event)
{
    mouseMoveTimer->start();
    setCursor(Qt::ArrowCursor);
}
