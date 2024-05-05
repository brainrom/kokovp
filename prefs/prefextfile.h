#ifndef PREFEXTFILE_H
#define PREFEXTFILE_H

#include "prefsection.h"

namespace Ui {
class PrefExtFile;
}

class PrefExtFile : public PrefSection
{
    Q_OBJECT

public:
    explicit PrefExtFile(QWidget *parent = nullptr);
    ~PrefExtFile();
    void load() override;
    void save() override;

private:
    Ui::PrefExtFile *ui;
};

#endif // PREFEXTFILE_H
