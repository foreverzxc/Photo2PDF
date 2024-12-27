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

class PhotoManager : public QObject
{
    Q_OBJECT
public:
    long long photoLength=0;
    QString outputPdfPath;
    QList<Transformers> transformersList;

    PhotoManager(QObject *parent);
    void AddPhotos();
    void ExportPDF();
    void UpdateProgress(int current, int total);
    void OnErrorOccurred(const QString &message);
    void OnExportFinished();

signals:
    void AddFileSignal(QString path,int index);
    void SetProgressValueSignal(int value);
    void CloseProgressDialogSignal();
    void SetProgressMaxValueSignal(int maxValue);



public slots:
    void SwapPhotosSlot(int a,int b);
    void SetRotationSlot(int angle,int index);
    void ExportPDFSlot(QStringList fileNames);
    void SelectedSavePathSlot(QString path);
};

#endif // PHOTOMANAGER_H
