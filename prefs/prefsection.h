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
#ifndef PREFSECTION_H
#define PREFSECTION_H

#include <QWidget>

class PrefSection : public QWidget
{
    Q_OBJECT
public:
    explicit PrefSection(QWidget *parent = nullptr) {}
    virtual void load() {}
    virtual void save() {}
};

#endif // PREFSECTION_H
