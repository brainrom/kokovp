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
#ifndef PREFDIALOG_H
#define PREFDIALOG_H

class QGridLayout;
class QStackedWidget;
class QListWidget;
class QDialogButtonBox;
class PrefSection;

class QPushButton;

#include <QDialog>

class PrefDialog : public QDialog
{
	Q_OBJECT
public:
    PrefDialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    void addSection(PrefSection *w);

public slots:
    void accept() override; // Reimplemented to send a signal
    void reject() override;

signals:
	void applied();


private:
    void apply();
    //void showHelp();

    QVariantMap configBak;
    QList<PrefSection*> sectionsList;

    QGridLayout *gridLayout;
    QStackedWidget *pages;
    QListWidget *sections;
    QDialogButtonBox *buttonBox;

    void showEvent(QShowEvent *event) override;
};

#endif
