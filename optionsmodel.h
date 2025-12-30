#ifndef OPTIONSMODEL_H
#define OPTIONSMODEL_H

#include <QAbstractTableModel>

class OptionsModel : public QAbstractTableModel
{
    Q_OBJECT
  public:

    struct OptionItem
    {
        QString name;
        QString value;
        bool enabled;
        friend QDataStream &operator<<(QDataStream &out, const OptionItem &rhs) {
            out << rhs.name;
            out << rhs.value;
            out << rhs.enabled;
            return out;
        }
        friend QDataStream &operator>>(QDataStream &in, OptionItem &rhs){
            in >> rhs.name;
            in >> rhs.value;
            in >> rhs.enabled;
            return in;
        }
    };


    enum OptionsModelCols {
        OptionsModel_Name,
        OptionsModel_Value,
        OptionsModel_Enabled
    };

    explicit OptionsModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QStringList mimeTypes() const;
    Qt::DropActions supportedDropActions() const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool removeRows(int row, int count, const QModelIndex &parent);
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
  private:
    QList<OptionItem> values;
    QStringList columnNames;
    void addEmptyRow();

    bool addItems(const QList<OptionItem> &items, int row);
};

#endif // OPTIONSMODEL_H
