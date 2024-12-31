#ifndef PHOTOMANAGER_H
#define PHOTOMANAGER_H

#include <QPdfWriter>
#include <QPainter>
#include <QTextDocument>
#include <QFileDialog.h>
#include <QObject>
#include <QStringList>
#include <QMessageBox>
#include <QPrinter>
#include <QPdfDocument>
#include <QMarginsF>
#include <QThread>
//user
#include "transformers.h"
#include "exportpdfworker.h"
#include "config.h"

class PhotoManager : public QObject
{
    Q_OBJECT
public:
    long long photoLength=0;
    QList<Transformers> transformersList;
    ExportPDFConfig config;

    PhotoManager(QObject *parent);
    void AddPhotos();
    void AddPDFs();
    void ExportPDF();
    void UpdateProgress(int current, int total);
    void OnErrorOccurred(const QString &message);
    void OnExportFinished();

signals:
    void AddPhotoRowSignal(QString path,int index);
    void AddPDFRowSignal(QString path,QString page,int index);
    void SetProgressValueSignal(int value);
    void CloseProgressDialogSignal();
    void SetProgressMaxValueSignal(int maxValue);



public slots:
    void SwapPhotosSlot(int a,int b);
    void SetRotationSlot(int angle,int index);
    void ExportPDFSlot(QStringList fileNames,QStringList pages);
    void SetConfigDpiSlot(int dpi);
    void SetConfigOutputPathSlot(QString path);
    void SetConfigResizeOptionSlot(ResizeOption resizeOption);
    void SetConfigPixelSlot(int value);
    void SetConfigNoExpandSlot(bool value);
};

#endif // PHOTOMANAGER_H
