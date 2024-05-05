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

    connect(this, &MpvWidget::fileLoaded, this, &PlayerWidget::hideLogo);
    connect(this, &MpvWidget::endFile, this, &PlayerWidget::showLogo);
}

void PlayerWidget::setPaintLogo(bool on)
{
    paintLogo = on;
    update();
}

void PlayerWidget::setClickAction(QAction *newClickAction)
{
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
    painter.drawText(rect().center(), "KokoVP");
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
