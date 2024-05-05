#ifndef PREFINPUT_H
#define PREFINPUT_H

#include "prefsection.h"

namespace Ui {
class PrefInput;
}

class QActionListModel;
class QComboBox;

class PrefInput : public PrefSection
{
    Q_OBJECT

public:
    explicit PrefInput(QWidget *parent = nullptr);
    ~PrefInput();
    void load() override;
    void save() override;

private:
    Ui::PrefInput *ui;
    QActionListModel *actsModel;
    QStringList confOptionsNames;
    QList<QComboBox*> btnsActionsCombo;
};

#endif // PREFINPUT_H
