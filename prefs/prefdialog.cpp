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
#include "prefdialog.h"
#include <QDialogButtonBox>

#include "prefappearance.h"
#include "prefextfile.h"
#include "prefinput.h"
#include "prefmain.h"
#include "prefscreenshots.h"
#include "prefadvanced.h"
#include "config.h"

#include <QVBoxLayout>
#include <QListWidget>
#include <QStackedWidget>
#include <QPushButton>

PrefDialog::PrefDialog(QWidget * parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(6);
    gridLayout->setContentsMargins(9, 9, 9, 9);

    pages = new QStackedWidget(this);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(pages->sizePolicy().hasHeightForWidth());
    pages->setSizePolicy(sizePolicy);
    gridLayout->addWidget(pages, 0, 1, 1, 1);

    sections = new QListWidget(this);
    QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(sections->sizePolicy().hasHeightForWidth());
    sections->setSizePolicy(sizePolicy1);
    sections->setMinimumSize(QSize(150, 0));
    sections->setUniformItemSizes(true);
    sections->setResizeMode(QListView::Adjust);
    sections->setMovement(QListView::Static);
    sections->setViewMode(QListView::ListMode);
    gridLayout->addWidget(sections, 0, 0, 1, 1);

    buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    gridLayout->addWidget(buttonBox, 1, 0, 1, 2);

	// Setup buttons
    connect(buttonBox, &QDialogButtonBox::accepted, this, &PrefDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &PrefDialog::reject);

    connect(sections, &QListWidget::currentRowChanged, pages, &QStackedWidget::setCurrentIndex);
    connect(buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &PrefDialog::apply );
    //connect(buttonBox->button(QDialogButtonBox::Help), &QPushButton::clicked,  this, &PreferencesDialog::showHelp);

    addSection(new PrefMain(this));
    addSection(new PrefInput(this));
    addSection(new PrefExtFile(this));
    addSection(new PrefScreenshots(this));
    addSection(new PrefAppearance(this));
    addSection(new PrefAdvanced(this));
}

void PrefDialog::accept() {
    qDebug() << "The preferences were accepted.";
    apply();
    hide();
    setResult(QDialog::Accepted);
    configBak.clear();
	emit applied();
}

void PrefDialog::apply() {
    qDebug() << "The preferences were applied.";
    for (auto &section : sectionsList)
        section->save();

    setResult(QDialog::Accepted);
    emit applied();
}

void PrefDialog::reject() {
    qDebug() << "The preferences were rejected.";
    hide();
    for (auto &section : sectionsList)
        section->load();

    // Restore Config up to state, when PrefDialog was opened
    for (auto [key, value] : configBak.asKeyValueRange())
        Config::i().set(key, value);

    setResult(QDialog::Rejected);
    configBak.clear();
    emit applied();
}

void PrefDialog::addSection(PrefSection *w) {
    QListWidgetItem *i = new QListWidgetItem(w->windowIcon(), w->windowTitle());
    sections->addItem(i);
	pages->addWidget(w);
    sectionsList.append(w);
}

void PrefDialog::showEvent(QShowEvent *event)
{
    // Make a config backup, for restoring on Cancel
    for (auto &key : Config::i().allKeys())
        configBak.insert(key, Config::i().value(key));

    for (auto &section : sectionsList)
        section->load();
}
