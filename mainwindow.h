#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDropEvent>
#include <QListWidgetItem>
#include <QSize>
#include <QTableWidgetItem>
#include <QProgressDialog>
//user
#include "photomanager.h"

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
    void AddARow(QString path,QString page,int index);
    void InitUI();
    void ShowPreview();
    void ShowProgressDialog(int maxValue);
    void resizeEvent(QResizeEvent * re);

signals:
    void SetRotationSignal(int angle,int index);
    void SwapPhotosSignal(int a,int b);
    void ExportPDFSignal(QStringList fileNames,QStringList pages);
    void SelectedSavePathSignal(QString path);
    void SetConfigDpiSignal(int dpi);
    void SetConfigOutputPathSignal(QString path);
    void SetConfigResizeOptionSignal(ResizeOption resizeOption);
    void SetConfigPixelSignal(int value);
    void SetConfigNoExpandSignal(bool value);

private slots:
    //click a button
    void ClickOpenFileButtonSlot();
    void ClickedOpenPDFButtonSlot();
    void ClickDeleteButtonSlot();
    void ClickRightRotationButtonSlot();
    void ClickLeftRotationButtonSlot();
    void ClickClearButtonSlot();
    void ClickReverseButtonSlot();
    void ClickedExportPDFButtonSlot();
    void ClickedSelectSavePathButtonSlot();
    //Qt default
    void on_photoTable_currentItemChanged(QTableWidgetItem *current);
    void on_comboBox_currentIndexChanged(int index);
    void on_savePathEdit_textChanged();
    void on_pixelLineEdit_textChanged(const QString &arg1);
    void on_noExpandCheckBox_checkStateChanged(const Qt::CheckState &arg1);
    //user
    void AddPhotoRowSlot(QString path,int index);
    void AddPDFRowSlot(QString path,QString page,int index);
    void SetProgressValueSlot(int value);
    void SetProgressMaxValueSlot(int maxValue);
    void CloseProgressDialogSlot();



private:
    Ui::MainWindow *ui;
    PhotoManager* photoListManager;
    QSize iconLabelSize;
    bool iconLabelFlag = false;

    QProgressDialog *progressDialog;

};
#endif // MAINWINDOW_H
