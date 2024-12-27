#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    photoListManager = new PhotoManager(this);
    //click a button
    connect(ui->openFileButton,&QPushButton::clicked,this,&MainWindow::ClickOpenFileButtonSlot);
    connect(ui->deleteButton,&QPushButton::clicked,this,&MainWindow::ClickDeleteButtonSlot);
    connect(ui->clearButton,&QPushButton::clicked,this,&MainWindow::ClickClearButtonSlot);
    connect(ui->rightRotationButton,&QPushButton::clicked,this,&MainWindow::ClickRightRotationButtonSlot);
    connect(ui->leftRotationButton,&QPushButton::clicked,this,&MainWindow::ClickLeftRotationButtonSlot);
    connect(ui->reverseButton,&QPushButton::clicked,this,&MainWindow::ClickReverseButtonSlot); //反转photo tab
    connect(ui->exportPDFButton,&QPushButton::clicked,this,&MainWindow::ClickedExportPDFButtonSlot);
    connect(ui->selectSavePathButton,&QPushButton::clicked,this,&MainWindow::ClickedSelectSavePathButtonSlot);


    /**************** Frontend -> Backend ****************/
    //MainWindow -> photoListManager
    connect(this,&MainWindow::SetRotationSignal,photoListManager,&PhotoManager::SetRotationSlot);
    connect(this,&MainWindow::SwapPhotosSignal,photoListManager,&PhotoManager::SwapPhotosSlot);
    connect(this,&MainWindow::ExportPDFSignal,photoListManager,&PhotoManager::ExportPDFSlot);
    connect(this,&MainWindow::SetConfigDpiSignal,photoListManager,&PhotoManager::SetConfigDpiSlot);
    connect(this,&MainWindow::SetConfigOutputPathSignal,photoListManager,&PhotoManager::SetConfigOutputPathSlot);
    connect(this,&MainWindow::SetConfigResizeOptionSignal,photoListManager,&PhotoManager::SetConfigResizeOptionSlot);
    connect(this,&MainWindow::SetConfigPixelSignal,photoListManager,&PhotoManager::SetConfigPixelSlot);
    connect(this,&MainWindow::SetConfigNoExpandSignal,photoListManager,&PhotoManager::SetConfigNoExpandSlot);
    //photoTable -> PhotoManager
    connect(ui->photoTable,&TableWidgetUpDown::SwapPhotosSingal,photoListManager,&PhotoManager::SwapPhotosSlot);


    /**************** Backend -> Frontend ****************/
    //photoListManager -> MainWindow
    connect(photoListManager,&PhotoManager::AddFileSignal,this,&MainWindow::AddPhotoSlot);
    connect(photoListManager,&PhotoManager::SetProgressValueSignal,this,&MainWindow::SetProgressValueSlot);
    connect(photoListManager,&PhotoManager::SetProgressMaxValueSignal,this,&MainWindow::SetProgressMaxValueSlot);
    connect(photoListManager,&PhotoManager::CloseProgressDialogSignal,this,&MainWindow::CloseProgressDialogSlot);




    /**************** Frontend -> Frontend ****************/
    //photoTable -> MainWindow
    connect(ui->photoTable,&TableWidgetUpDown::ShowIconSignal,this,&MainWindow::ShowIcon);


    /**************** Backend -> Backend ****************/



    InitUI();

}

void MainWindow::InitUI()
{
    setWindowTitle("Photo2PDF ---- by zxc Email:forever__zxc@163.com");
    QIcon right_rotation_icon,left_rotation_icon;
    right_rotation_icon.addPixmap(QPixmap(":/resource/right_rotation_icon.png"), QIcon::Normal, QIcon::On);
    left_rotation_icon.addPixmap(QPixmap(":/resource/left_rotation_icon.png"), QIcon::Normal, QIcon::On);
    ui->rightRotationButton->setIcon(right_rotation_icon);
    ui->leftRotationButton->setIcon(left_rotation_icon);

    ui->pixelLineEdit->setValidator(new QIntValidator(ui->pixelLineEdit));
}

void MainWindow::ShowProgressDialog(int maxValue)
{
    progressDialog = new QProgressDialog("任务进行中...", "取消", 0, maxValue, this);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setAutoClose(true);
    progressDialog->setCancelButton(nullptr);
    // progressDialog->setRange(0,maxValue);//设置进度条范围
    progressDialog->show();
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
        photoListManager->transformersList.removeAt(currentRow);
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
        ui->photoPixelInfoLabel->setText("当前图片大小为：" + QString::number(pixmap.width()) + "×" + QString::number(pixmap.height()));
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
        ui->photoPixelInfoLabel->setText("当前图片大小为：");
    }
}


void MainWindow::ClickClearButtonSlot()
{
    auto table = ui->photoTable;
    table->clearContents();
    table->setRowCount(0);
    photoListManager->photoLength = 0;
    photoListManager->transformersList.clear();
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
        // qDebug() << "current row is:" << i << "->>" << table->item(i, 0)->text();
        table->removeRow(i);
    }

    // 重新添加
    for (int i = 0; i < rowCount; ++ i)
        this->AddPhotoSlot(pathList[i], i);

    for(int i=0;i<rowCount/2;++i)
    {
        emit SwapPhotosSignal(i,rowCount-i-1);
    }
}

void MainWindow::ClickRightRotationButtonSlot()
{
    if(ui->photoTable->currentItem()!=nullptr)
    {
        emit SetRotationSignal(90,ui->photoTable->currentRow());
        ShowIcon();
    }
}

void MainWindow::ClickLeftRotationButtonSlot()
{
    if(ui->photoTable->currentItem()!=nullptr)
    {
        emit SetRotationSignal(-90,ui->photoTable->currentRow());
        ShowIcon();
    }
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
        emit SetConfigResizeOptionSignal(ResizeOption::NoResize);
        break;
    case 1://设置短边
        pixelLineEdit->setEnabled(true);
        noExpandCheckBox->setEnabled(true);
        emit SetConfigResizeOptionSignal(ResizeOption::ResizeShortSide);
        break;
    case 2://设置长边
        pixelLineEdit->setEnabled(true);
        noExpandCheckBox->setEnabled(true);
        emit SetConfigResizeOptionSignal(ResizeOption::ResizeLongSide);
        break;
    }
}

void MainWindow::ClickedExportPDFButtonSlot()
{
    QDir dir(ui->savePathEdit->text());
    if(ui->savePathEdit->text().isEmpty())
    {
        QMessageBox::information(this, "Error", "请输入保存地址");
        return;
    }
    if(ui->photoTable->rowCount()==0)
    {
        QMessageBox::information(this, "Error", "请打开至少一个文件");
        return;
    }
    if(!dir.Writable)
    {
        QMessageBox::information(this, "Error", "该路径不正确或没有写入权限");
        return;
    }
    QStringList fileNames;
    auto table = ui->photoTable;
    for (int row = 0; row < table->rowCount(); ++row)
    {
        QTableWidgetItem *item = table->item(row, 0);
        if (item)
        {
            QString cellText = item->text();
            fileNames.append(cellText);
        }
    }
    ShowProgressDialog(photoListManager->photoLength);
    emit ExportPDFSignal(fileNames);
}

void MainWindow::SetProgressValueSlot(int value)
{
    progressDialog->setValue(value);
}

void MainWindow::SetProgressMaxValueSlot(int maxValue)
{
    progressDialog->setMaximum(maxValue);
}

void MainWindow::CloseProgressDialogSlot()
{
    progressDialog->close();
}

void MainWindow::ClickedSelectSavePathButtonSlot()
{
    QString path = QFileDialog::getSaveFileName(this, tr("选择保存路径"), ".", tr("*.pdf"));
    if (path.isEmpty())
    {
        return;
    }
    if(path.right(4)!=".pdf")
    {
        path += ".pdf";
    }
    ui->savePathEdit->setText(path);
}

void MainWindow::on_savePathEdit_textChanged()
{
    emit SelectedSavePathSignal(ui->savePathEdit->text());
    emit SetConfigOutputPathSignal(ui->savePathEdit->text());
}


void MainWindow::on_pixelLineEdit_textChanged(const QString &arg1)
{
    emit SetConfigPixelSignal(arg1.toInt());
}


void MainWindow::on_noExpandCheckBox_checkStateChanged(const Qt::CheckState &arg1)
{
    if(arg1 == Qt::Checked)
        emit SetConfigNoExpandSignal(true);
    else if(arg1 == Qt::Unchecked)
    {
        emit SetConfigNoExpandSignal(false);
    }
}

