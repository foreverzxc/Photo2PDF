#ifndef TABLEWIDGETUPDOWN_H
#define TABLEWIDGETUPDOWN_H

#include <QWidget>
#include <QTableWidget>
#include <QDropEvent>
#include <QDebug>

class TableWidgetUpDown : public QTableWidget
{
    Q_OBJECT
signals:
    void SwapPhotosSingal(int a,int b);
public:
    explicit TableWidgetUpDown(QWidget *parent = nullptr);
protected:
    void dropEvent(QDropEvent *event);
};

#endif // TABLEWIDGETUPDOWN_H
