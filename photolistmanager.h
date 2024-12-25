#ifndef PHOTOLISTMANAGER_H
#define PHOTOLISTMANAGER_H

#include "QObject"
#include "QStringList"

class PhotoListManager : public QObject
{
    Q_OBJECT
private:
    long long listLength=0;
public:
    PhotoListManager(QObject *parent);

    void AddPhotos();
signals:
    void AddFileSignal(QString path,int index);
};

#endif // PHOTOLISTMANAGER_H
