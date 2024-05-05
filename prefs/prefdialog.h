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
