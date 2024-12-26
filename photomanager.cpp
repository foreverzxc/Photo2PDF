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

void PhotoManager::SelectedSavePathSlot(QString path)
{
    outputPdfPath = path;
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

void PhotoManager::ExportPDFSlot(QStringList fileNames)
{
    QPdfWriter pdfWriter(outputPdfPath);
    pdfWriter.setResolution(300);  // 设置 DPI 为 300
    pdfWriter.setPageMargins(QMarginsF(0, 0, 0, 0));

    QPainter painter;

    // 遍历每张图片
    for (int i = 0; i < fileNames.size(); ++i) {
        QString imagePath = fileNames[i];
        QImage image(imagePath);
        if (image.isNull()) {
            qWarning() << "Failed to load image:" << imagePath;
            continue;
        }
        QTransform transform;
        transform.rotate(transformersList[i].rotation);
        QImage transformedImage = image.transformed(transform);

        // 将图片的尺寸转化为毫米 (1 inch == 25.4 mm)
        qreal dpi = 300.0;
        qreal widthInMm = transformedImage.width() * 25.4 / dpi;  // 转换为毫米
        qreal heightInMm = transformedImage.height() * 25.4 / dpi;  // 转换为毫米

        // 设置当前页面大小为图片尺寸，单位为毫米
        pdfWriter.setPageSize(QPageSize(QSizeF(widthInMm, heightInMm), QPageSize::Millimeter));

        // 对于第一张图片，初始化 QPainter
        if (i == 0) {
            if (!painter.begin(&pdfWriter)) {
                qWarning() << "Failed to begin painting PDF.";
                return;
            }
            // 设置渲染提示，提升画图效果
            painter.setRenderHint(QPainter::Antialiasing);
        } else {
            // 后续图片需要新建页面
            pdfWriter.newPage();
        }
        painter.drawImage(0, 0, transformedImage);
    }

    painter.end();

    qDebug() << "PDF created successfully at:" << outputPdfPath;

}
