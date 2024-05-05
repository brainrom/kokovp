#ifndef SEEKINTERFACE_H
#define SEEKINTERFACE_H

#include <QMenu>

class SeekInterface : public QObject
{
    Q_OBJECT
public:
    explicit SeekInterface(QMenu *parent = nullptr);
    void setStepTime(int step, int s);
signals:
    void seek(int s);
private:
    int actsCount;
    void handleSeekAction(QAction *action);
    QActionGroup *seekGroup;
};

#endif // SEEKINTERFACE_H
