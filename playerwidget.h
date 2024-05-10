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
#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include "mpvwidget.h"
#include <QUrl>
#include <QTimer>

class PlayerWidget : public MpvWidget
{
    Q_OBJECT
public:
    PlayerWidget(QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());
    void setClickAction(QAction *newClickAction);
    void setDoubleClickAction(QAction *newDoubleClickAction);
    void setRightClickAction(QAction *newRightClickAction);
    void setWheelActions(QAction *newWheelUpAct, QAction *newWheelDownAct);

signals:
    void draggedURLS(const QList<QUrl> &urls);
private:

    void showLogo() { setPaintLogo(true); }
    void hideLogo() { setPaintLogo(false); }
    void setPaintLogo(bool on);

    bool paintLogo;
    QFont logoFont;
    QTimer doubleClickTimer;

    QAction *clickAct;
    QAction *doubleClickAct;
    QAction *rightClickAct;

    QAction *wheelUpAct;
    QAction *wheelDownAct;

    void paintOverlay(QPainter &painter) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
};

#endif // PLAYERWIDGET_H
