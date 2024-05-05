#ifndef SEEKWIDGET_H
#define SEEKWIDGET_H

#include <QString>
#include <QWidget>

class QLabel;
class QTimeEdit;

class SeekWidget : public QWidget
{
	Q_OBJECT
public:
    SeekWidget(QWidget* parent = 0, Qt::WindowFlags f = Qt::WindowFlags());
	int time() const;

    void setIcon(QIcon icon);
	void setLabel(QString text);
	void setTime(int secs);
private:
    QLabel *icon_label;
    QLabel *text_label;
    QTimeEdit *time_edit;
};

#endif
