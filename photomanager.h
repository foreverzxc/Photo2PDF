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

#include "transformers.h"

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

signals:
    void AddFileSignal(QString path,int index);

public slots:
    void SwapPhotosSlot(int a,int b);
    void SetRotationSlot(int angle,int index);
    void ExportPDFSlot(QStringList fileNames);
    void SelectedSavePathSlot(QString path);
};

#endif // PHOTOMANAGER_H
