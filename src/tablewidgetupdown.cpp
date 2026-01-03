#include "tablewidgetupdown.h"

TableWidgetUpDown::TableWidgetUpDown(QWidget *parent) : QTableWidget(parent)
{
    setColumnCount(1);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDropIndicatorShown(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
}

void TableWidgetUpDown::mousePressEvent(QMouseEvent *event)
{
    QTableWidgetItem *item = itemAt(event->pos());
    m_dragStartRow = item ? item->row() : -1;
    QTableWidget::mousePressEvent(event);
}

void TableWidgetUpDown::dropEvent(QDropEvent *event)
{
    int rowSrc = m_dragStartRow;
    m_dragStartRow = -1;

    if (rowSrc < 0) {
        event->ignore();
        return;
    }

    QTableWidgetItem *item = itemAt(event->position().toPoint());
    int rowDst = item ? item->row() : rowCount();

    if (rowSrc == rowDst) {
        event->acceptProposedAction();
        return;
    }

    emit swapPhotosSignal(rowSrc, rowDst);
    emit showIconSignal();
    event->acceptProposedAction();
}
