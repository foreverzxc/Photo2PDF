#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "photolistmanager.h"
#include "QListWidgetItem"
#include "Qsize"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void AddPhoto(QString path,int index);
    void resizeEvent(QResizeEvent * re);

signals:

private slots:
    void ClickOpenFileButtonSlot();
    void AddPhotoSlot(QString path,int index);
    void ClickDeleteButtonSlot();
    void on_fileList_currentItemChanged(QListWidgetItem *current);

private:
    Ui::MainWindow *ui;
    PhotoListManager* photoListManager;
    QSize iconLabelSize;
    bool iconLabelFlag = false;

};
#endif // MAINWINDOW_H
