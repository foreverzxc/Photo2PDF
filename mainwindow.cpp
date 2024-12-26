#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox"
#include "QDropEvent"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    photoListManager = new PhotoManager(this);
    connect(ui->openFileButton,&QPushButton::clicked,this,&MainWindow::ClickOpenFileButtonSlot);
    connect(photoListManager,&PhotoManager::AddFileSignal,this,&MainWindow::AddPhotoSlot);
    connect(ui->deleteButton,&QPushButton::clicked,this,&MainWindow::ClickDeleteButtonSlot);
    connect(ui->clearButton,&QPushButton::clicked,this,&MainWindow::ClickClearButtonSlot);
    connect(ui->photoTable,&TableWidgetUpDown::SwapPhotosSingal,photoListManager,&PhotoManager::SwapPhotosSlot);
    connect(ui->rightRotationButton,&QPushButton::clicked,this,&MainWindow::ClickRightRotationButtonSlot);
    connect(ui->leftRotationButton,&QPushButton::clicked,this,&MainWindow::ClickLeftRotationButtonSlot);
    connect(ui->reverseButton,&QPushButton::clicked,this,&MainWindow::ClickReverseButtonSlot); //反转photo tab
    connect(this,&MainWindow::SetRotationSignal,photoListManager,&PhotoManager::SetRotationSlot);
}


void MainWindow::resizeEvent(QResizeEvent * re)
{
    iconLabelSize = ui->iconLabel->size();
    // qDebug()<<ui->photoTable->width()<<" "<<ui->photoTable->columnWidth(0)<< " "<< ui->photoTable->columnWidth(1);
    ui->photoTable->setColumnWidth(0,ui->photoTable->width()-5);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete photoListManager;
}

void MainWindow::ClickOpenFileButtonSlot()
{
    photoListManager->AddPhotos();
}

void MainWindow::AddPhotoSlot(QString path,int index)
{
    auto table = ui->photoTable;
    table->setRowCount(table->rowCount()+1);
    table->setRowHeight(index,40);
    table->setColumnWidth(0,table->width()-5);
    table->setItem(index,0,new QTableWidgetItem(path));
    // table->insertItem(index,path);
}

void MainWindow::ClickDeleteButtonSlot()
{
    int currentRow = ui->photoTable->currentRow();
    if (currentRow != -1)
    {
        ui->photoTable->removeRow(currentRow);
        photoListManager->photoLength--;
    }
}

void MainWindow::ShowIcon()
{
    int index = ui->photoTable->currentRow();
    QString fileName = ui->photoTable->item(index,0)->text();
    QImage image(fileName);
    if (!image.isNull())
    {
        QPixmap pixmap = QPixmap::fromImage(image);
        QTransform transform;
        transform.rotate(photoListManager->transformersList[index].rotation);
        QPixmap transformedPixmap = pixmap.transformed(transform);
        QPixmap scaledPixmap = transformedPixmap.scaled(iconLabelSize, Qt::KeepAspectRatio, Qt::FastTransformation);
        ui->iconLabel->setPixmap(scaledPixmap);
    }
    else
    {
        QMessageBox::information(this, tr("Open Image"), tr("Cannot load the image."));
    }

}

void MainWindow::on_photoTable_currentItemChanged(QTableWidgetItem *current)
{
    if(!iconLabelFlag)
    {
        iconLabelFlag=true;
        iconLabelSize = ui->iconLabel->size();
    }
    if(current!=nullptr)
    {
        ShowIcon();
    }
    else
    {
        ui->iconLabel->clear();
    }
}


void MainWindow::ClickClearButtonSlot()
{
    auto table = ui->photoTable;
    table->clearContents();
    table->setRowCount(0);
    photoListManager->photoLength = 0;
}

void MainWindow::ClickReverseButtonSlot()
{
    qDebug() << "into ClickReverseButtonSlot";

    // 清空ab
    auto table = ui->photoTable;
    int rowCount = table->rowCount();
    QList<QString> pathList;
    for (int i = rowCount - 1; i >= 0; -- i){
        pathList.push_back(table->item(i, 0)->text());
        qDebug() << "current row is:" << i << "->>" << table->item(i, 0)->text();
        table->removeRow(i);
    }

    // 重新添加
    for (int i = 0; i < rowCount; ++ i)
        this->AddPhotoSlot(pathList[i], i);
}

void MainWindow::ClickRightRotationButtonSlot()
{
    emit SetRotationSignal(90,ui->photoTable->currentRow());
    ShowIcon();
}

void MainWindow::ClickLeftRotationButtonSlot()
{
    emit SetRotationSignal(-90,ui->photoTable->currentRow());
    ShowIcon();
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    auto pixelLineEdit = ui->pixelLineEdit;
    auto noExpandCheckBox = ui->noExpandCheckBox;
    switch (index)
    {
    case 0://不改变分辨率
        pixelLineEdit->setEnabled(false);
        noExpandCheckBox->setEnabled(false);
        break;
    case 1://设置短边
        pixelLineEdit->setEnabled(true);
        noExpandCheckBox->setEnabled(true);
        break;
    case 2://设置长边
        pixelLineEdit->setEnabled(true);
        noExpandCheckBox->setEnabled(true);
        break;
    }
}

