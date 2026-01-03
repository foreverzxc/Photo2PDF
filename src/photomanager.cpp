#include "photomanager.h"
#include "photomodel.h"
#include <QDebug>

PhotoManager::PhotoManager(QObject *parent)
    : QObject(parent)
{
}

void PhotoManager::setExportConfig(const ExportPDFConfig& config)
{
    m_config = config;
}

void PhotoManager::setOutputPath(const QString& path)
{
    m_config.outputPath = path;
}

void PhotoManager::setResizeOption(ResizeOption option)
{
    m_config.resizeOption = option;
}

void PhotoManager::setConfigPixel(int pixel)
{
    m_config.configPixel = pixel;
}

void PhotoManager::setNoExpand(bool noExpand)
{
    m_config.noExpand = noExpand;
}

void PhotoManager::exportFromModel(PhotoModel* model)
{
    if (!model || model->rowCount() == 0) {
        emit exportErrorSignal("没有可导出的图片");
        return;
    }

    if (m_config.outputPath.isEmpty()) {
        emit exportErrorSignal("请先设置保存路径");
        return;
    }

    // Get data from model
    QStringList fileNames = model->getAllFilePaths();
    QStringList pages = model->getAllPageNumbers();
    QList<int> rotations = model->getAllRotations();

    // Create transformers list from rotations
    QList<Transformers> transformersList;
    for (int rotation : rotations) {
        Transformers t;
        t.rotation = rotation;
        transformersList.append(t);
    }

    // Create worker thread
    QThread* workerThread = new QThread(this);
    ExportPDFWorker* worker = new ExportPDFWorker(fileNames, pages, transformersList, m_config);
    worker->moveToThread(workerThread);

    // Connect signals - use lambda to call run() since it's protected
    connect(workerThread, &QThread::started, worker, [worker]() {
        worker->run();
    });
    connect(worker, &ExportPDFWorker::finished, workerThread, &QThread::quit);
    connect(worker, &ExportPDFWorker::finished, worker, &QObject::deleteLater);
    connect(workerThread, &QThread::finished, workerThread, &QObject::deleteLater);

    // Connect to our signals for progress updates
    connect(worker, &ExportPDFWorker::ProgressUpdatedSignal,
            this, &PhotoManager::exportProgressSignal, Qt::QueuedConnection);
    connect(worker, &ExportPDFWorker::FinishedSignal,
            this, &PhotoManager::exportFinishedSignal, Qt::QueuedConnection);
    connect(worker, &ExportPDFWorker::ErrorOccurredSignal,
            this, &PhotoManager::exportErrorSignal, Qt::QueuedConnection);

    qDebug() << "Starting PDF export to:" << m_config.outputPath;
    workerThread->start();
}
