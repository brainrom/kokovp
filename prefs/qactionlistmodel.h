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
