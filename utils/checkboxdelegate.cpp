#include "checkboxdelegate.h"
#include <QPainter>
#include <QStyle>
#include <QMouseEvent>

#include <QApplication>

bool CheckBoxDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Qt::ItemFlags isEditable = index.model()->flags(index);
    if (!(isEditable & Qt::ItemIsEditable))
        return QStyledItemDelegate::editorEvent(event,model,option,index);

    if ( event->type() == QEvent::MouseButtonRelease )
    {
        QMouseEvent * e = (QMouseEvent *)event;
        if (option.rect.contains(e->pos()))
        {
            bool modelVal = index.data().toBool();
            int value = (!modelVal)? 1 : 0;
            model->setData(index, value, Qt::EditRole);
        }
    }
    return QStyledItemDelegate::editorEvent(event,model,option,index);
}

QWidget *CheckBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(parent)
    Q_UNUSED(option)
    Q_UNUSED(index)
    return nullptr;
}

void CheckBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    bool data = index.model()->data(index, Qt::DisplayRole).toBool();
    Qt::ItemFlags isEditable = index.model()->flags(index);
    if (!(isEditable & Qt::ItemIsEditable))
        return;

    // CheckBox Style
    QStyleOptionButton checkboxstyle;
    QRect checkbox_rect = QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator, &checkboxstyle);

    // Centring
    checkboxstyle.rect = option.rect;
    checkboxstyle.rect.setLeft(option.rect.x() +
                               option.rect.width()/2 - checkbox_rect.width()/2);
    // Checkbox state
    checkboxstyle.state = option.state | (data ? QStyle::State_On : QStyle::State_Off);

    QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkboxstyle, painter);
}
