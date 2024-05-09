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
    QSize sizeForMenuItem(QStyleOptionMenuItem *opt);
    void initStyleOption(QStyleOptionMenuItem *opt);
    void updateSize();
    IncDecWheelAction *action;
};

class IncDecWheelAction : public QWidgetAction
{
    Q_OBJECT
    friend class IncDecWheelActionWidget;
public:
    explicit IncDecWheelAction(QString label, QWidget *parent = nullptr);
    void setDelta(double delta);
    void setNamePrefix(QString name_prefix);
    void setLabelTemplate(QString labelTemplate);
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
