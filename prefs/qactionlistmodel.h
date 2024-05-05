#ifndef QACTIONLISTMODEL_H
#define QACTIONLISTMODEL_H

#include <QAbstractTableModel>

class QAction;

class QActionListModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    QActionListModel(QObject *parent = nullptr);
    QActionListModel(QList<QAction*> list, QObject *parent = nullptr);
    void setQActionList(QList<QAction*> list);

    enum Columns { COL_SHORTCUT, COL_LABEL };
    //enum Roles {CurrentRole = Qt::UserRole +1, URLRole};

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    QStringList columnNames;
    QList<QAction*> values;
};

#endif // QACTIONLISTMODEL_H
