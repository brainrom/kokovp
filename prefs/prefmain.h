#ifndef PREFMAIN_H
#define PREFMAIN_H

#include "prefsection.h"

namespace Ui {
class PrefMain;
}

class PrefMain : public PrefSection
{
    Q_OBJECT

public:
    explicit PrefMain(QWidget *parent = nullptr);
    ~PrefMain();
    void load() override;
    void save() override;

private:
    Ui::PrefMain *ui;
};

#endif // PREFMAIN_H
