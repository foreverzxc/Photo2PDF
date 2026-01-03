#ifndef TABLEWIDGETUPDOWN_H
#define TABLEWIDGETUPDOWN_H

#include <QWidget>
#include <QTableWidget>
#include <QDropEvent>

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
};

#endif // TABLEWIDGETUPDOWN_H
