/*  This is part of KokoVP

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
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
