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

IncDecWheelAction::IncDecWheelAction(QString label, QWidget *parent)
    : QWidgetAction{parent}
{
    mainText = label;
    decText = " - ";
    incText = " + ";
    acts = new QActionGroup(this);
    p_incAction = new QAction(this);
    p_decAction = new QAction(this);
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

void IncDecWheelAction::setLabelTemplate(QString l)
{
    labelTemplate = l;
}

void IncDecWheelAction::setNamePrefix(QString name_prefix)
{
    p_incAction->setObjectName(name_prefix+"_plus");
    p_decAction->setObjectName(name_prefix+"_minus");
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
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    updateSize();
}

void IncDecWheelActionWidget::updateSize()
{
    QStyleOptionMenuItem opt;

    initStyleOption(&opt);
    opt.text = action->mainText;

    QSize mainSz = sizeForMenuItem(&opt);

    opt.text = action->incText;
    QSize plusSz = sizeForMenuItem(&opt);

    opt.text = action->decText;
    QSize minusSz = sizeForMenuItem(&opt);

    mainTextRect = QRect(0, 0, mainSz.width(), mainSz.height());
    minusRect = QRect(width()-minusSz.width(), 0, minusSz.width(), minusSz.height());
    plusRect = QRect(minusRect.left()-plusSz.width(), 0, plusSz.width(), plusSz.height());
    if (mainTextRect.right()<plusRect.left())
        mainTextRect.setRight(plusRect.left());

    setMinimumSize(mainSz.width()+minusSz.width()+plusSz.width(), mainSz.height());
}

void IncDecWheelActionWidget::resizeEvent(QResizeEvent *event)
{
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

    QStyleOptionMenuItem option;
    initStyleOption(&option);

    option.text = action->mainText;
    option.rect = mainTextRect;
    if (!mousePos.isNull() && mainTextRect.contains(mousePos)) {
        option.state |= QStyle::State_Selected;
    }
    p.drawControl(QStyle::CE_MenuItem, option);

    initStyleOption(&option);
    option.text = action->incText;
    option.rect = plusRect;
    if (!mousePos.isNull() && plusRect.contains(mousePos)) {
        option.state |= QStyle::State_Selected;
    }
    p.drawControl(QStyle::CE_MenuItem, option);

    initStyleOption(&option);
    option.text = action->decText;
    option.rect = minusRect;
    if (!mousePos.isNull() && minusRect.contains(mousePos)) {
        option.state |= QStyle::State_Selected;
    }
    p.drawControl(QStyle::CE_MenuItem, option);
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
    opt->menuHasCheckableItems = false; // TODO: d->hasCheckableItems;

    //if (action->isIconVisibleInMenu())
    //    opt->icon = action->icon();

    opt->menuItemType = QStyleOptionMenuItem::Normal;
}
