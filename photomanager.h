#ifndef PHOTOMANAGER_H
#define PHOTOMANAGER_H

#include "QObject"
#include "QStringList"
#include "transformers.h"

class PhotoManager : public QObject
{
    Q_OBJECT
public:
    long long photoLength=0;
    QList<Transformers> transformersList;
public:
    PhotoManager(QObject *parent);

    void AddPhotos();
signals:
    void AddFileSignal(QString path,int index);
public slots:
    void SwapPhotosSlot(int a,int b);
    void SetRotationSlot(int angle,int index);
};

#endif // PHOTOMANAGER_H
