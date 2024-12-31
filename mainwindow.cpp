#include "mainwindow.h"
#include "ui_mainwindow.h"

extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    photoListManager = new PhotoManager(this);
    //click a button
    connect(ui->openFileButton,&QPushButton::clicked,this,&MainWindow::ClickOpenFileButtonSlot);
    connect(ui->openPDFButton,&QPushButton::clicked,this,&MainWindow::ClickedOpenPDFButtonSlot);
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
    connect(photoListManager,&PhotoManager::AddPhotoRowSignal,this,&MainWindow::AddPhotoRowSlot);
    connect(photoListManager,&PhotoManager::AddPDFRowSignal,this,&MainWindow::AddPDFRowSlot);
    connect(photoListManager,&PhotoManager::SetProgressValueSignal,this,&MainWindow::SetProgressValueSlot);
    connect(photoListManager,&PhotoManager::SetProgressMaxValueSignal,this,&MainWindow::SetProgressMaxValueSlot);
    connect(photoListManager,&PhotoManager::CloseProgressDialogSignal,this,&MainWindow::CloseProgressDialogSlot);


    /**************** Frontend -> Frontend ****************/
    //photoTable -> MainWindow
    connect(ui->photoTable,&TableWidgetUpDown::ShowIconSignal,this,&MainWindow::ShowPreview);


    /**************** Backend -> Backend ****************/



    InitUI();
}
MainWindow::~MainWindow()
{
    delete ui;
    delete photoListManager;
}

/************************************************ user define function ************************************************/
void MainWindow::InitUI()
{
    setWindowTitle("Photo2PDF ---- by zxc Email:forever__zxc@163.com");
    QIcon right_rotation_icon,left_rotation_icon;
    right_rotation_icon.addPixmap(QPixmap(":/resource/right_rotation_icon.png"), QIcon::Normal, QIcon::On);
    left_rotation_icon.addPixmap(QPixmap(":/resource/left_rotation_icon.png"), QIcon::Normal, QIcon::On);
    ui->rightRotationButton->setIcon(right_rotation_icon);
    ui->leftRotationButton->setIcon(left_rotation_icon);

    //配置table
    auto table = ui->photoTable;
    table->setColumnCount(2);
    table->setColumnWidth(0,table->width()*0.8);
    table->setColumnWidth(1,table->width()*0.1);
    table->setHorizontalHeaderItem(0,new QTableWidgetItem("路径"));
    table->setHorizontalHeaderItem(1,new QTableWidgetItem("页码"));

    ui->pixelLineEdit->setValidator(new QIntValidator(ui->pixelLineEdit));
}

void MainWindow::AddARow(QString path,QString page,int index)
{
    auto table = ui->photoTable;
    table->setRowCount(table->rowCount()+1);
    table->setRowHeight(index,40);
    table->setItem(index,0,new QTableWidgetItem(path));
    if(page=="")
    {
        table->setItem(index,1,new QTableWidgetItem(""));
    }
    else
    {
        table->setItem(index,1,new QTableWidgetItem(page));
    }
}

/**************************** show progress ****************************/
void MainWindow::ShowProgressDialog(int maxValue)
{
    progressDialog = new QProgressDialog("任务进行中...", "取消", 0, maxValue, this);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setAutoClose(true);
    progressDialog->setCancelButton(nullptr);
    // progressDialog->setRange(0,maxValue);//设置进度条范围
    progressDialog->show();
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

/**************************** show preview ****************************/
void MainWindow::ShowPreview()
{
    auto table = ui->photoTable;
    int index = table->currentRow();
    QString fileName = table->item(index,0)->text();
    QImage image;

    if(table->item(index,1)->text() == "")
    {
        image = QImage(fileName);
    }
    else
    {
        int page = ui->photoTable->item(index,1)->text().toInt()-1;
        QPdfDocument* document = new QPdfDocument;
        document->load(fileName);
        QSizeF size = document->pagePointSize(page);
        image = document->render(page, QSize(size.width(), size.height()));
        document->close();
    }
    if (!image.isNull())
    {
        QPixmap pixmap = QPixmap::fromImage(image);
        ui->photoPixelInfoLabel->setText("当前图片大小为：" + QString::number(pixmap.width()) + "×" + QString::number(pixmap.height()));
        QTransform transform;
        transform.rotate(photoListManager->transformersList[index].rotation);
        QPixmap transformedPixmap = pixmap.transformed(transform);
        QPixmap scaledPixmap = transformedPixmap.scaled(iconLabelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        // qDebug()<<scaledPixmap.size();
        ui->iconLabel->setPixmap(scaledPixmap);
    }
    else
    {
        QMessageBox::information(this, tr("Open Image"), tr("Cannot load the image."));
    }
}

/************************************************ click a button slot ************************************************/
void MainWindow::ClickOpenFileButtonSlot()
{
    auto table = ui->photoTable;
    table->setColumnWidth(0,table->width()*0.9);
    table->setColumnWidth(1,table->width()*0.1);
    photoListManager->AddPhotos();
}

void MainWindow::ClickedOpenPDFButtonSlot()
{
    auto table = ui->photoTable;
    table->setColumnWidth(0,table->width()*0.9);
    table->setColumnWidth(1,table->width()*0.1);
    photoListManager->AddPDFs();
}

void MainWindow::AddPhotoRowSlot(QString path,int index)
{
    AddARow(path,"",index);
}

void MainWindow::AddPDFRowSlot(QString path,QString page,int index)
{
    AddARow(path,QString::number(page.toInt() + 1),index);
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

void MainWindow::ClickClearButtonSlot()
{
    QMessageBox::StandardButton box;
    box = QMessageBox::question(this, "提示", "确实要清空吗?", QMessageBox::Yes|QMessageBox::No);
    if(box==QMessageBox::No)
        return;

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
    QList<QString> pathList,pageList;
    for (int i = rowCount - 1; i >= 0; -- i)
    {
        pageList.push_back(table->item(i, 1)->text());
        pathList.push_back(table->item(i, 0)->text());
        // qDebug() << "current row is:" << i << "->>" << table->item(i, 0)->text();
        table->removeRow(i);
    }

    // 重新添加
    for (int i = 0; i < rowCount; ++ i)
        AddARow(pathList[i],pageList[i], i);

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
        ShowPreview();
    }
}

void MainWindow::ClickLeftRotationButtonSlot()
{
    if(ui->photoTable->currentItem()!=nullptr)
    {
        emit SetRotationSignal(-90,ui->photoTable->currentRow());
        ShowPreview();
    }
}

bool IsDirWritable(const QString& dir)
{
    qDebug() << dir+"\XXXXXX";
    auto testFile = QTemporaryFile::createNativeFile(dir+"\XXXXXX");
    return testFile->open();
}

void MainWindow::ClickedExportPDFButtonSlot()
{
    QNtfsPermissionCheckGuard permissionGuard;
    QFileInfo fileInfo(ui->savePathEdit->text());
    fileInfo = QFileInfo(fileInfo.dir().path());
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
    if(!fileInfo.isWritable())
    {
        QMessageBox::information(this, "Error", "该路径不正确或没有写入权限");
        return;
    }
    QStringList fileNames,pages;
    auto table = ui->photoTable;
    for (int row = 0; row < table->rowCount(); ++row)
    {
        QTableWidgetItem *item = table->item(row, 0);
        if (item)
        {
            QString cellText = item->text();
            fileNames.append(cellText);
            if(table->item(row,1) != 0)
            {
                pages.append(table->item(row,1)->text());
            }
            else
            {
                pages.append("");
            }
        }
    }
    ShowProgressDialog(photoListManager->photoLength);
    emit ExportPDFSignal(fileNames,pages);
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

/************************************************ Qt define methods ************************************************/
void MainWindow::resizeEvent(QResizeEvent * re)
{
    // iconLabelFlag = false;
    // iconLabelSize = ui->iconLabel->size();
    // qDebug()<<ui->photoTable->width()<<" "<<ui->photoTable->columnWidth(0)<< " "<< ui->photoTable->columnWidth(1);
    auto table = ui->photoTable;
    table->setColumnWidth(0,table->width()*0.9);
    table->setColumnWidth(1,table->width()*0.1);
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
        ShowPreview();
    }
    else
    {
        ui->iconLabel->clear();
        ui->photoPixelInfoLabel->setText("当前图片大小为：");
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

