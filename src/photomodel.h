#ifndef PHOTOMODEL_H
#define PHOTOMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QPdfDocument>

#include "photoitem.h"
#include "config.h"

class PhotoModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit PhotoModel(QObject *parent = nullptr);

    // Basic model operations
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    // Editing operations
    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool moveRows(const QModelIndex& sourceParent, int sourceRow, int count,
                  const QModelIndex& destinationParent, int destinationChild) override;

    // File dialog actions
    void addImages();
    void addPdfs();

    // Photo management
    void addImage(const QString& filePath);
    void addPdfPages(const QString& pdfPath, int pageCount);
    void removeAt(int index);
    void clear();
    void reverse();

    // Rotation operations
    void rotateItem(int index, int angle);
    int getRotation(int index) const;

    // Data access
    PhotoItem getItem(int index) const;
    QList<PhotoItem> getAllItems() const { return m_items; }

    // Export helpers
    QStringList getAllFilePaths() const;
    QStringList getAllPageNumbers() const;
    QList<int> getAllRotations() const;

signals:
    void itemsChanged();

private:
    QList<PhotoItem> m_items;
};

#endif // PHOTOMODEL_H
