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
#ifndef INCDECWHEELACTION_H
#define INCDECWHEELACTION_H

#include <QWidgetAction>
#include <QWidget>


class QStyleOptionMenuItem;
class IncDecWheelAction;
class QStyleOptionMenuItem;

class IncDecWheelActionWidget : public QWidget
{
    Q_OBJECT
    friend class IncDecWheelAction;
public:
    explicit IncDecWheelActionWidget(IncDecWheelAction *action, QWidget *parent = nullptr);
protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
private:
    QPoint mousePos;
    QRect mainTextRect, plusRect, minusRect;

    int maxIconWidth = 0;
    bool menuHasCheckableItems = false;

    QSize sizeForMenuItem(QStyleOptionMenuItem *opt);
    void initStyleOption(QStyleOptionMenuItem *opt);
    void updateSize();
    void updateMenuContext();
    IncDecWheelAction *action;
};

class IncDecWheelAction : public QWidgetAction
{
    Q_OBJECT
    friend class IncDecWheelActionWidget;
public:
    explicit IncDecWheelAction(QString label, QString label_template, QString name_prefix, QWidget *parent = nullptr);
    void setDelta(double delta);
    void setDecOptions(QString decText, QKeySequence decShortcut);
    void setIncOptions(QString incText, QKeySequence incShortcut);
    QList<QAction*> const actionsList();
    QAction *incAction() const { return p_incAction; }
    QAction *decAction() const { return p_decAction; }
protected:
    QWidget *createWidget(QWidget *parent);
signals:
    void valueChanged(double delta);
private:
    void addActionToParent(QAction *a);
    void updateAction();
    void handleActions(QAction *a);
    double delta;
    QString decText, incText, mainText, labelTemplate;
    QAction *p_incAction;
    QAction *p_decAction;
    QActionGroup *acts;
};

#endif // INCDECWHEELACTION_H
