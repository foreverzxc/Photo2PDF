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

void TableWidgetUpDown::dropEvent(QDropEvent *event)
{
    int rowSrc = currentRow();
    QTableWidgetItem *item = itemAt(event->position().toPoint());

    if (rowSrc < 0) {
        event->ignore();
        return;
    }

    int rowDst;
    if (item != nullptr) {
        rowDst = item->row();
    } else {
        rowDst = rowCount();
    }

    // 发出移动信号，由外部（MainWindow）处理同步
    emit swapPhotosSignal(rowSrc, rowDst);

    // 发出刷新预览信号
    emit showIconSignal();

    // 接受事件但不做其他操作
    event->acceptProposedAction();
}


