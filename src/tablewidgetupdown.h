#ifndef TABLEWIDGETUPDOWN_H
#define TABLEWIDGETUPDOWN_H

#include <QWidget>
#include <QTableWidget>
#include <QDropEvent>
#include <QMouseEvent>

class TableWidgetUpDown : public QTableWidget
{
    Q_OBJECT
signals:
    void showIconSignal();
    void swapPhotosSignal(int a, int b);

public:
    explicit TableWidgetUpDown(QWidget *parent = nullptr);

protected:
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    int m_dragStartRow = -1;  // Record the row where drag started
};

#endif // TABLEWIDGETUPDOWN_H
