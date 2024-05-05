#ifndef PREFSCREENSHOTS_H
#define PREFSCREENSHOTS_H

#include "prefsection.h"

namespace Ui {
class PrefScreenshots;
}

class PrefScreenshots : public PrefSection
{
    Q_OBJECT

public:
    explicit PrefScreenshots(QWidget *parent = nullptr);
    ~PrefScreenshots();
    void load() override;
    void save() override;

private:
    Ui::PrefScreenshots *ui;
    void selectScreenshotsFolder();
};

#endif // PREFSCREENSHOTS_H
