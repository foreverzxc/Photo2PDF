#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    photoListManager = new PhotoListManager(this);
    connect(ui->openFileButton,&QPushButton::clicked,this,&MainWindow::ClickOpenFileButtonSlot);
    connect(photoListManager,&PhotoListManager::AddFileSignal,this,&MainWindow::AddPhotoSlot);
    connect(ui->deleteButton,&QPushButton::clicked,this,&MainWindow::ClickDeleteButtonSlot);
}


void MainWindow::resizeEvent(QResizeEvent * re)
{
    iconLabelSize = ui->iconLabel->size();
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
    ui->fileList->insertItem(index,path);
}

void MainWindow::ClickDeleteButtonSlot()
{
    int currentRow = ui->fileList->currentRow();
    if (currentRow != -1)
    {
        QListWidgetItem *item = ui->fileList->takeItem(currentRow);
        delete item;
    }
}

void MainWindow::on_fileList_currentItemChanged(QListWidgetItem *current)
{
    if(!iconLabelFlag)
    {
        iconLabelFlag=true;
        iconLabelSize = ui->iconLabel->size();
    }

    if(current!=nullptr)
    {
        QString fileName = current->text();
        QImage image(fileName);
        if (!image.isNull())
        {
            QPixmap pixmap = QPixmap::fromImage(image);
            QPixmap scaledPixmap = pixmap.scaled(iconLabelSize, Qt::KeepAspectRatio, Qt::FastTransformation);
            ui->iconLabel->setPixmap(scaledPixmap);
        }
        else
        {
            QMessageBox::information(this, tr("Open Image"), tr("Cannot load the image."));
        }
    }
    else
    {
        ui->iconLabel->clear();
    }
}

