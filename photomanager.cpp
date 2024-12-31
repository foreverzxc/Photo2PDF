#include "photomanager.h"


PhotoManager::PhotoManager(QObject *parent): QObject(parent)
{

}

void PhotoManager::SwapPhotosSlot(int a,int b)
{
    transformersList.swapItemsAt(a,b);
}

void PhotoManager::SetRotationSlot(int angle,int index)
{
    transformersList[index].rotation += angle;
    transformersList[index].rotation %= 360;
}

void PhotoManager::AddPhotos()
{
    QStringList fileNames;
    //定义文件对话框类
    QFileDialog *fileDialog = new QFileDialog();
    //定义文件对话框标题
    fileDialog->setWindowTitle(QFileDialog::tr("打开图片"));
    //设置默认文件路径
    fileDialog->setDirectory(".");
    //设置文件过滤器
    fileDialog->setNameFilter(QFileDialog::tr("Images(*.png *.jpg *.jpeg *.bmp)"));
    //设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFiles
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    //设置视图模式
    fileDialog->setViewMode(QFileDialog::Detail);
    //打印所有选择的文件的路径
    if(fileDialog->exec())
    {
        fileNames = fileDialog->selectedFiles();
    }
    if(fileNames.length()>0)
    {
        for(auto item:fileNames)
        {
            emit AddFileSignal(item,photoLength);
            photoLength++;
            transformersList.append(Transformers());
        }
    }
}

void PhotoManager::AddPDF()
{
    QStringList fileNames;
    //定义文件对话框类
    QFileDialog *fileDialog = new QFileDialog();
    //定义文件对话框标题
    fileDialog->setWindowTitle(QFileDialog::tr("打开图片"));
    //设置默认文件路径
    fileDialog->setDirectory(".");
    //设置文件过滤器
    fileDialog->setNameFilter(QFileDialog::tr("PDF(*.pdf *.PDF)"));
    //设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFiles
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    //设置视图模式
    fileDialog->setViewMode(QFileDialog::Detail);
    //打印所有选择的文件的路径
    if(fileDialog->exec())
    {
        fileNames = fileDialog->selectedFiles();
    }
    if(fileNames.length()>0)
    {
        for(auto pdfFile:fileNames)
        {
            QPdfDocument* document = new QPdfDocument;
            document->load(pdfFile);
            int totalPage = document->pageCount();
            for (int page = 0; page < totalPage; page++)
            {
                emit AddPDFSignal(pdfFile,page,photoLength);
                photoLength++;
                transformersList.append(Transformers());
            }
            document->close();
        }
    }
}

void PhotoManager::ExportPDFSlot(QStringList fileNames,QStringList pages)
{
    // 发送信号告知主线程操作开始
    emit SetProgressValueSignal(0);  // 可以初始化进度为0

    // 创建新的线程来执行导出PDF
    ExportPDFWorker *worker = new ExportPDFWorker(fileNames, pages, transformersList, config);
    connect(worker, &ExportPDFWorker::ProgressUpdatedSignal, this, &PhotoManager::UpdateProgress);
    connect(worker, &ExportPDFWorker::FinishedSignal, this, &PhotoManager::OnExportFinished);
    connect(worker, &ExportPDFWorker::ErrorOccurredSignal, this, &PhotoManager::OnErrorOccurred);

    // 启动线程
    worker->start();
}

// 更新进度条
void PhotoManager::UpdateProgress(int current, int total)
{
    emit SetProgressValueSignal(current);
    emit SetProgressMaxValueSignal(total);
}

// 导出完成
void PhotoManager::OnExportFinished()
{
    emit CloseProgressDialogSignal();
    qDebug() << "PDF export completed successfully.";
}

// 处理错误
void PhotoManager::OnErrorOccurred(const QString &message)
{
    emit CloseProgressDialogSignal();
    qWarning() << "Error during PDF export: " << message;
}

void PhotoManager::SetConfigDpiSlot(int dpi)
{
    config.dpi = dpi;
}
void PhotoManager::SetConfigOutputPathSlot(QString path)
{
    config.outputPath = path;
}
void PhotoManager::SetConfigResizeOptionSlot(ResizeOption resizeOption)
{
    config.resizeOption = resizeOption;
}
void PhotoManager::SetConfigPixelSlot(int value)
{
    config.configPixel = value;
}
void PhotoManager::SetConfigNoExpandSlot(bool value)
{
    config.noExpand = value;
}
