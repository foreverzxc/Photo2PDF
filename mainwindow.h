#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "photomanager.h"
#include "QListWidgetItem"
#include "Qsize"
#include "QTableWidgetItem"

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
    void ShowIcon();
    void resizeEvent(QResizeEvent * re);

signals:
    void SetRotationSignal(int angle,int index);
private slots:
    void ClickOpenFileButtonSlot();

    void AddPhotoSlot(QString path,int index);

    void ClickDeleteButtonSlot();

    void ClickRightRotationButtonSlot();

    void ClickLeftRotationButtonSlot();

    void on_photoTable_currentItemChanged(QTableWidgetItem *current);

    void ClickClearButtonSlot();

    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    PhotoManager* photoListManager;
    QSize iconLabelSize;
    bool iconLabelFlag = false;

};
#endif // MAINWINDOW_H
