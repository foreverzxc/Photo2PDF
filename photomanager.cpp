#include "photomanager.h"
#include "QFileDialog.h"
#include "transformers.h"

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