#ifndef PREFSECTION_H
#define PREFSECTION_H

#include <QWidget>

class PrefSection : public QWidget
{
    Q_OBJECT
public:
    explicit PrefSection(QWidget *parent = nullptr) {};
    virtual void load() {};
    virtual void save() {};
};

#endif // PREFSECTION_H
