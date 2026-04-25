#ifndef MANIPULATIONTABLEVIEW_H
#define MANIPULATIONTABLEVIEW_H

#include <QTableView>

class ManipulationTableView : public QTableView
{
    Q_OBJECT
public:
    explicit ManipulationTableView(QWidget *parent = nullptr);
    void removeSelectedRows();
    void customMenu(const QPoint &pos);
private:
    QMenu *contextMenu;
    void paste();
    void cut();
    void clear();
    void copy();
};

#endif // MANIPULATIONTABLEVIEW_H
