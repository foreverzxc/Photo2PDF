#ifndef PHOTOMANAGER_H
#define PHOTOMANAGER_H

#include <QObject>
#include <QStringList>
#include <QList>
#include <QThread>

#include "config.h"
#include "photoitem.h"
#include "exportpdfworker.h"

class PhotoModel;

class PhotoManager : public QObject
{
    Q_OBJECT
public:
    explicit PhotoManager(QObject *parent = nullptr);

    // Export configuration
    void setExportConfig(const ExportPDFConfig& config);
    void setOutputPath(const QString& path);
    void setResizeOption(ResizeOption option);
    void setConfigPixel(int pixel);
    void setNoExpand(bool noExpand);

    // Export from model
    void exportFromModel(PhotoModel* model);

signals:
    // Export progress signals
    void exportProgressSignal(int current, int total);
    void exportFinishedSignal();
    void exportErrorSignal(const QString& message);

private:
    ExportPDFConfig m_config;
};

#endif // PHOTOMANAGER_H
