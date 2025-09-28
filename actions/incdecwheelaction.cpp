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
#include "incdecwheelaction.h"
#include <QStyleOptionMenuItem>
#include <QMenu>
#include <QStylePainter>
#include <QActionGroup>
#include <QWheelEvent>

IncDecWheelAction::IncDecWheelAction(QString label, QString label_template, QString name_prefix, QWidget *parent)
    : QWidgetAction{parent}
{
    mainText = label;
    labelTemplate = label_template;
    decText = " - ";
    incText = " + ";
    acts = new QActionGroup(this);
    p_incAction = new QAction(this);
    p_decAction = new QAction(this);
    p_incAction->setObjectName(name_prefix+"_plus");
    p_decAction->setObjectName(name_prefix+"_minus");

    acts->addAction(p_incAction);
    acts->addAction(p_decAction);
    connect(acts, &QActionGroup::triggered, this, &IncDecWheelAction::handleActions);

    delta = 1;
    updateAction();

    parent->addAction(this);
    addActionToParent(p_incAction);
    addActionToParent(p_decAction);
}

void IncDecWheelAction::setDelta(double newDelta)
{
    newDelta = abs(newDelta);
    if (newDelta==0)
        newDelta = 1;

    delta = newDelta;
    updateAction();
}

void IncDecWheelAction::setDecOptions(QString newDecText, QKeySequence decShortcut)
{
    p_decAction->setShortcut(decShortcut);
    decText = newDecText;
    updateAction();
}

void IncDecWheelAction::setIncOptions(QString newIncText, QKeySequence incShortcut)
{
    p_incAction->setShortcut(incShortcut);
    incText = newIncText;
    updateAction();
}

const QList<QAction*> IncDecWheelAction::actionsList()
{
    return QList<QAction*> {p_incAction, p_decAction};
}

void IncDecWheelAction::updateAction()
{
    p_incAction->setData(delta);
    p_incAction->setText(labelTemplate.arg(incText));

    p_decAction->setData(-delta);
    p_decAction->setText(labelTemplate.arg(decText));
}

void IncDecWheelAction::handleActions(QAction *a)
{
    emit valueChanged(a->data().toDouble());
}

QWidget *IncDecWheelAction::createWidget(QWidget *parent) {
    IncDecWheelActionWidget *w = new IncDecWheelActionWidget(this, parent);
    connect(this, &QAction::changed, w, &IncDecWheelActionWidget::updateSize);
    w->setEnabled(isEnabled());
    w->setFocusPolicy(Qt::TabFocus);
    return w;
}

void IncDecWheelAction::addActionToParent(QAction *a)
{
    if (parent()) {
        if (parent()->inherits("QWidget")) {
            QWidget *w = static_cast<QWidget*> (parent());
            if (w->parentWidget() && w->parentWidget()->window())
                w->parentWidget()->window()->addAction(a);
        }
    }
}

IncDecWheelActionWidget::IncDecWheelActionWidget(IncDecWheelAction *action, QWidget *parent) : QWidget{parent}, action{action}
{
    setMouseTracking(true);
    if (parent->inherits("QMenu"))
    {
        QMenu *w = static_cast<QMenu*>(parent);
        connect(w, &QMenu::aboutToShow, this, &IncDecWheelActionWidget::updateMenuContext);
        updateMenuContext();
    }

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

void IncDecWheelActionWidget::updateSize()
{
    QStyleOptionMenuItem opt;

    initStyleOption(&opt);
    opt.text = action->mainText;

    QSize mainSz = sizeForMenuItem(&opt);
    qDebug() << "IncDecWheelActionWidget::updateSize()" << action->labelTemplate << ", mainSz w =" << mainSz;

    opt.menuHasCheckableItems = false;
    opt.maxIconWidth = 0;

    opt.text = action->incText;
    QSize plusSz = sizeForMenuItem(&opt);
    qDebug() << "IncDecWheelActionWidget::updateSize()" << action->labelTemplate << ", plusSz =" << plusSz;

    opt.text = action->decText;
    QSize minusSz = sizeForMenuItem(&opt);
    qDebug() << "IncDecWheelActionWidget::updateSize()" << action->labelTemplate << ", minusSz =" << minusSz;

    int maxIncDecW = qMax(plusSz.width(), minusSz.width());
    plusSz.setWidth(maxIncDecW);
    minusSz.setWidth(maxIncDecW);

    setMinimumSize(mainSz.width()+minusSz.width()+plusSz.width(), mainSz.height());

    plusRect   = QRect(rect().bottomRight() - QPoint(plusSz.width()-1, plusSz.height()-1),
                     rect().bottomRight());
    minusRect  = QRect(plusRect.topLeft() - QPoint(minusSz.width(), 0),
                      plusRect.bottomLeft());
    mainTextRect = QRect(rect().topLeft(),
                         minusRect.bottomLeft() - QPoint(1, 0));
}

void IncDecWheelActionWidget::resizeEvent(QResizeEvent *event)
{
    updateSize();
}

void IncDecWheelActionWidget::updateMenuContext()
{
    if (!parent()->inherits("QMenu"))
        return;

    QStyleOption opt;
    opt.initFrom(this);

    const int icone = style()->pixelMetric(QStyle::PM_SmallIconSize, &opt, this);

    QMenu *w = static_cast<QMenu*>(parent());
    for (const auto &action : w->actions())
    {
        if (action->isSeparator() || !action->isVisible())
            continue;
        menuHasCheckableItems |= action->isCheckable();
        QIcon is = action->icon();
        if (!is.isNull()) {
            maxIconWidth = qMax<uint>(maxIconWidth, icone + 4);
        }
    }
    updateSize();
}

void IncDecWheelActionWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    if (plusRect.contains(pos))
        action->p_incAction->trigger();
    if (minusRect.contains(pos))
        action->p_decAction->trigger();
}

void IncDecWheelActionWidget::mouseMoveEvent(QMouseEvent *event)
{
    mousePos = event->pos();
    update();
}

void IncDecWheelActionWidget::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() >= 0) action->p_incAction->trigger(); else action->p_decAction->trigger();
}

QSize IncDecWheelActionWidget::sizeForMenuItem(QStyleOptionMenuItem *opt)
{
    QSize sz;
    const QFontMetrics &fm = opt->fontMetrics;
    sz.setWidth(fm.boundingRect(QRect(), Qt::TextSingleLine | Qt::TextShowMnemonic, opt->text).width());
    sz.setHeight(qMax(fm.height(), fm.height()));
    /*QIcon is = action->icon();
    if (!is.isNull()) {
        const int icone = style()->pixelMetric(QStyle::PM_SmallIconSize, opt, this);
        QSize is_sz = QSize(icone, icone);
        if (is_sz.height() > sz.height())
            sz.setHeight(is_sz.height());
    }*/
    sz = style()->sizeFromContents(QStyle::CT_MenuItem, opt, sz, this);
    return sz;
}

void IncDecWheelActionWidget::enterEvent(QEnterEvent *event)
{
    // Workaround for Qt's incorrect QActionWidget focus handling
    if (QMenu *menu = qobject_cast<QMenu*>(parentWidget())) {
        menu->setActiveAction(action);
    }
    update();
}

void IncDecWheelActionWidget::leaveEvent(QEvent *event)
{
    mousePos = QPoint();
    update();
}

void IncDecWheelActionWidget::paintEvent(QPaintEvent *event)
{
    QStylePainter p(this);

    QStyleOptionMenuItem opt;
    initStyleOption(&opt);

    opt.text = action->mainText;
    opt.rect = mainTextRect;
    if (!mousePos.isNull() && mainTextRect.contains(mousePos)) {
        opt.state |= QStyle::State_Selected;
    }
    p.drawControl(QStyle::CE_MenuItem, opt);

    opt.state &= ~QStyle::State_Selected;
    opt.menuHasCheckableItems = false;
    opt.maxIconWidth = 0;
    opt.text = "";

    opt.rect = plusRect;
    opt.state &= ~QStyle::State_Selected;
    if (!mousePos.isNull() && plusRect.contains(mousePos)) {
        opt.state |= QStyle::State_Selected;
    }
    p.drawControl(QStyle::CE_MenuItem, opt);
    p.drawItemText(
        opt.rect,
        Qt::AlignCenter,
        opt.palette,
        opt.state & QStyle::State_Enabled,
        action->incText
        );

    opt.rect = minusRect;
    opt.state &= ~QStyle::State_Selected;
    if (!mousePos.isNull() && minusRect.contains(mousePos)) {
        opt.state |= QStyle::State_Selected;
    }
    p.drawControl(QStyle::CE_MenuItem, opt);
    p.drawItemText(
        opt.rect,
        Qt::AlignCenter,
        opt.palette,
        opt.state & QStyle::State_Enabled,
        action->decText
        );
}

void IncDecWheelActionWidget::initStyleOption(QStyleOptionMenuItem *opt)
{
    opt->initFrom(this);
    opt->palette = palette();
    opt->state = QStyle::State_None;

    if (window()->isActiveWindow())
        opt->state |= QStyle::State_Active;
    if (isEnabled() && action->isEnabled()
        && (!action->menu() || action->menu()->isEnabled()))
        opt->state |= QStyle::State_Enabled;
    else
        opt->palette.setCurrentColorGroup(QPalette::Disabled);

    opt->font = font();
    opt->fontMetrics = QFontMetrics(opt->font);

    opt->checkType = QStyleOptionMenuItem::NotCheckable;

    opt->menuHasCheckableItems = menuHasCheckableItems;
    opt->maxIconWidth = maxIconWidth;

    //if (action->isIconVisibleInMenu())
    //    opt->icon = action->icon();

    opt->menuItemType = QStyleOptionMenuItem::Normal;
}
