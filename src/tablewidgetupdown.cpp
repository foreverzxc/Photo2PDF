#include "tablewidgetupdown.h"

TableWidgetUpDown::TableWidgetUpDown(QWidget *parent) : QTableWidget(parent)
{
    setColumnCount(1);

    // 拖拽设置
    setDragDropMode(QAbstractItemView::InternalMove);
    setDropIndicatorShown(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
}

void TableWidgetUpDown::mousePressEvent(QMouseEvent *event)
{
    // Record the starting row when mouse is pressed
    QTableWidgetItem *item = itemAt(event->pos());
    if (item) {
        m_dragStartRow = item->row();
    } else {
        m_dragStartRow = -1;
    }
    QTableWidget::mousePressEvent(event);
}

void TableWidgetUpDown::dropEvent(QDropEvent *event)
{
    int rowSrc = m_dragStartRow;
    m_dragStartRow = -1;  // Reset

    if (rowSrc < 0) {
        event->ignore();
        return;
    }

    QTableWidgetItem *item = itemAt(event->position().toPoint());

    int rowDst;
    if (item != nullptr) {
        rowDst = item->row();
    } else {
        rowDst = rowCount();
    }

    // 如果源行和目标行相同，不做操作
    if (rowSrc == rowDst) {
        event->acceptProposedAction();
        return;
    }

    // 发出移动信号，由外部（MainWindow）处理同步
    emit swapPhotosSignal(rowSrc, rowDst);

    // 发出刷新预览信号
    emit showIconSignal();

    // 接受事件但不做其他操作
    event->acceptProposedAction();
}
