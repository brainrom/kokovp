#ifndef INCDECACTIONS_H
#define INCDECACTIONS_H

#include <QObject>
#include <QIcon>

class QAction;
class QActionGroup;

class IncDecActionsPair : public QObject
{
    Q_OBJECT
public:
    explicit IncDecActionsPair(QObject *parent = nullptr);

    void setDelta(double delta);
    void setIncAction(QAction *a);
    void setDecAction(QAction *a);

    const QList<QAction*> actions();
    const QAction *incAction() const { return p_incAction; }
    const QAction *decAction() const { return p_decAction; }

signals:
    void valueChanged(double delta);
private:
    void handleActions(QAction *a);
    double delta;
    QAction *p_incAction;
    QAction *p_decAction;
    QActionGroup *acts;
};

#endif // INCDECACTIONS_H
