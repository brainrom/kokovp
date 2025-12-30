#include "optionsmodel.h"
#include <QDataStream>
#include <QIODevice>
#include <qmimedata.h>

OptionsModel::OptionsModel(QObject *parent)
    : QAbstractTableModel{parent}
{
    columnNames = {tr("Name"), tr("Value"), tr("Enabled") };
    addEmptyRow();
}

int OptionsModel::rowCount(const QModelIndex &parent) const
{
    return values.count();
}

int OptionsModel::columnCount(const QModelIndex &parent) const
{
    return columnNames.count();
}

QVariant OptionsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return columnNames.at(section);

    return QVariant();
}

QMimeData *OptionsModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData;
    QByteArray encodedData;

    QList<int> rows;
    QString textFormat;

    for (const QModelIndex &index : indexes) {
        if (index.isValid() && !rows.contains(index.row())) {
            int row = index.row();
            rows.append(row);
            const OptionItem &kv = values.at(row);
            textFormat.append(QString("%1%2=%3\n").arg(kv.enabled ? "" : "#", kv.name, kv.value));
        }
    }

    mimeData->setText(textFormat);
    return mimeData;
}

QStringList OptionsModel::mimeTypes() const
{
    return QStringList{"text/plain"};
}


Qt::DropActions OptionsModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

bool OptionsModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(action);
    Q_UNUSED(row);
    Q_UNUSED(parent);

    if (data->hasText())
    {
        return true;
    }

    return false;
}

bool OptionsModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (!canDropMimeData(data, action, row, column, parent))
        return false;

    if (action == Qt::IgnoreAction)
        return true;

    int beginRow;

    if (row != -1 && row != rowCount(QModelIndex()))
        beginRow = row;
    else if (parent.isValid())
        beginRow = parent.row();
    else
        beginRow = rowCount(QModelIndex()) - 1;


    QList<OptionItem> newItems;
    QString text = data->text();
    QStringList lines = text.split('\n');
    for (auto &l : lines)
    {
        if (l.isEmpty())
            continue;
        OptionItem item { .enabled = true };
        if (l.startsWith('#'))
        {
            item.enabled = false;
            l.removeFirst();
        }
        QStringList kv = l.split('=');
        if (kv.length()!=2)
            continue;
        item.name = kv.at(0);
        item.value = kv.at(1);
        newItems.append(item);
    }
    addItems(newItems, beginRow);

    return true;
}

bool OptionsModel::addItems(const QList<OptionItem> &items, int row)
{
    if (items.count()==0)
        return false;

    if (row<0 || row > rowCount(QModelIndex()))
        row = rowCount();

    int count = items.count();

    beginInsertRows(QModelIndex(), row, row+count-1);
    for (auto &item : items)
        values.insert(row++, item);
    endInsertRows();
    return true;
}

void OptionsModel::addEmptyRow()
{
    beginInsertRows(QModelIndex(), values.size(), values.size());
    values.push_back({});
    endInsertRows();
}

bool OptionsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole || !index.isValid() || index.row() >= static_cast<int>(values.size()))
        return false;

    OptionItem &kv = values[index.row()];
    int column = index.column();

    QString oldName = kv.name;

    switch (column) {
        case OptionsModel_Name:
            kv.name = value.toString();
            break;
        case OptionsModel_Value:
            kv.value = value.toString();
            break;
        case OptionsModel_Enabled:
            kv.enabled = value.toBool();
            break;
    }

    // Row removal condition
    if (kv.name.isEmpty() && !oldName.isEmpty()) {
        removeRows(index.row(), 1, QModelIndex());
        return true;
    }

    // Notify full row changed for reinterpretation recalculation
    emit dataChanged(this->index(index.row(), 0),
                     this->index(index.row(), columnCount() - 1));

    if (index.row() == rowCount() - 1 && !kv.name.isEmpty()) {
        addEmptyRow();
    }
    return true;
}

QVariant OptionsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(values.size()))
        return QVariant();

    const OptionItem &kv = values[index.row()];
    int column = index.column();

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (column) {
            case OptionsModel_Name: return kv.name;
            case OptionsModel_Value: return kv.value;
            case OptionsModel_Enabled: return kv.enabled;
        }
    }

    return QVariant();
}

Qt::ItemFlags OptionsModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags ret = QAbstractTableModel::flags(index);

    bool notLastRow = index.row() < rowCount() - 1;

    if (!index.isValid() || !notLastRow)
        ret |= Qt::ItemIsDropEnabled;

    if (notLastRow)
        ret |= Qt::ItemIsDragEnabled;

    if (index.column()==0 || notLastRow)
    {
        ret |= Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }

    return ret;
}

bool OptionsModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);

    if (row < 0 || row >= values.size()-1)
        return false;

    beginRemoveRows(QModelIndex(), row, row+count-1);
    values.remove(row, count);
    endRemoveRows();
    return true;
}
