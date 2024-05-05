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
