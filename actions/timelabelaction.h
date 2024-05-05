#ifndef TIMELABELACTION_H
#define TIMELABELACTION_H

#include <QWidgetAction>

class TimeLabelAction : public QWidgetAction
{
    Q_OBJECT
public:
    enum TimeLabelType { CurrentTime = 0, TotalTime = 1, CurrentAndTotalTime = 2, RemainingTime = 3 };

    TimeLabelAction(TimeLabelType type, QWidget * parent);

public slots:
    void setText(QString s);
    void setCurrentTime(double);
    void setTotalTime(double);

signals:
    void newText(QString s);

protected:
    QWidget *createWidget(QWidget *parent);
    void updateText();

private:
    QString current_text;
    double current_time, total_time;
    TimeLabelType label_type;
};

#endif // TIMELABELACTION_H
