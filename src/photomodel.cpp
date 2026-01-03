#include "photomodel.h"
#include <QPdfDocument>
#include <QFileDialog>
#include <QDebug>

PhotoModel::PhotoModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int PhotoModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_items.count();
}

QVariant PhotoModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.count()) {
        return QVariant();
    }

    const PhotoItem& item = m_items.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
        return item.displayName();
    case Qt::UserRole + 1:
        return item.filePath();
    case Qt::UserRole + 2:
        return item.displayPage();
    case Qt::UserRole + 3:
        return item.rotation();
    case Qt::UserRole + 4:
        return static_cast<int>(item.type());
    default:
        return QVariant();
    }
}

bool PhotoModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || index.row() >= m_items.count()) {
        return false;
    }

    PhotoItem& item = m_items[index.row()];

    switch (role) {
    case Qt::UserRole + 3:  // rotation
        item.setRotation(value.toInt());
        emit dataChanged(index, index, {role});
        return true;
    default:
        return false;
    }
}

bool PhotoModel::insertRows(int row, int count, const QModelIndex& parent)
{
    Q_UNUSED(parent);
    beginInsertRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        m_items.insert(row, PhotoItem());
    }
    endInsertRows();
    return true;
}

bool PhotoModel::removeRows(int row, int count, const QModelIndex& parent)
{
    Q_UNUSED(parent);
    if (row < 0 || row + count > m_items.count()) {
        return false;
    }
    beginRemoveRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        m_items.removeAt(row);
    }
    endRemoveRows();
    return true;
}

bool PhotoModel::moveRows(const QModelIndex& sourceParent, int sourceRow, int count,
                          const QModelIndex& destinationParent, int destinationChild)
{
    Q_UNUSED(sourceParent);
    Q_UNUSED(destinationParent);

    if (sourceRow < 0 || sourceRow + count > m_items.count()) {
        return false;
    }
    if (destinationChild < 0 || destinationChild > m_items.count()) {
        return false;
    }

    // 使用 beginResetModel/endResetModel 避免复杂的 beginMoveRows 语义问题
    beginResetModel();

    // Save items to move
    QList<PhotoItem> itemsToMove;
    for (int i = 0; i < count; ++i) {
        itemsToMove.append(m_items.at(sourceRow + i));
    }

    // Remove from source (in reverse order)
    for (int i = count - 1; i >= 0; --i) {
        m_items.removeAt(sourceRow + i);
    }

    // Calculate actual insert position (adjust for removed items)
    int insertPos = destinationChild;
    if (sourceRow < destinationChild) {
        // Moving down, destinationChild was calculated after source was removed
        insertPos = destinationChild - count;
    }

    // Insert at calculated position
    for (const PhotoItem& item : itemsToMove) {
        m_items.insert(insertPos++, item);
    }

    endResetModel();
    return true;
}

void PhotoModel::addImages()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(
        nullptr,
        tr("打开图片"),
        ".",
        tr("Images(*.png *.jpg *.jpeg *.bmp)")
    );

    if (fileNames.isEmpty()) {
        return;
    }

    for (const QString& filePath : fileNames) {
        addImage(filePath);
    }
}

void PhotoModel::addPdfs()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(
        nullptr,
        tr("打开PDF"),
        ".",
        tr("PDF(*.pdf *.PDF)")
    );

    if (fileNames.isEmpty()) {
        return;
    }

    for (const QString& pdfPath : fileNames) {
        QPdfDocument document;
        document.load(pdfPath);
        int pageCount = document.pageCount();
        addPdfPages(pdfPath, pageCount);
        document.close();
    }
}

void PhotoModel::addImage(const QString& filePath)
{
    beginInsertRows(QModelIndex(), m_items.count(), m_items.count());
    m_items.append(PhotoItem(filePath, -1));
    endInsertRows();
}

void PhotoModel::addPdfPages(const QString& pdfPath, int pageCount)
{
    beginInsertRows(QModelIndex(), m_items.count(), m_items.count() + pageCount - 1);
    for (int i = 0; i < pageCount; ++i) {
        m_items.append(PhotoItem(pdfPath, i));
    }
    endInsertRows();
}

void PhotoModel::removeAt(int index)
{
    if (index < 0 || index >= m_items.count()) {
        return;
    }
    beginRemoveRows(QModelIndex(), index, index);
    m_items.removeAt(index);
    endRemoveRows();
}

void PhotoModel::clear()
{
    if (m_items.isEmpty()) {
        return;
    }
    beginRemoveRows(QModelIndex(), 0, m_items.count() - 1);
    m_items.clear();
    endRemoveRows();
}

void PhotoModel::reverse()
{
    if (m_items.count() <= 1) {
        return;
    }
    beginResetModel();
    std::reverse(m_items.begin(), m_items.end());
    endResetModel();
}

void PhotoModel::rotateItem(int index, int angle)
{
    if (index < 0 || index >= m_items.count()) {
        return;
    }
    m_items[index].setRotation(m_items[index].rotation() + angle);
    QModelIndex idx = this->index(index);
    emit dataChanged(idx, idx, {Qt::UserRole + 3});
}

int PhotoModel::getRotation(int index) const
{
    if (index < 0 || index >= m_items.count()) {
        return 0;
    }
    return m_items[index].rotation();
}

PhotoItem PhotoModel::getItem(int index) const
{
    if (index < 0 || index >= m_items.count()) {
        return PhotoItem();
    }
    return m_items.at(index);
}

QStringList PhotoModel::getAllFilePaths() const
{
    QStringList paths;
    for (const PhotoItem& item : m_items) {
        paths.append(item.filePath());
    }
    return paths;
}

QStringList PhotoModel::getAllPageNumbers() const
{
    QStringList pages;
    for (const PhotoItem& item : m_items) {
        if (item.type() == PhotoItem::Type::PdfPage) {
            pages.append(QString::number(item.pageNumber()));
        } else {
            pages.append(QString());
        }
    }
    return pages;
}

QList<int> PhotoModel::getAllRotations() const
{
    QList<int> rotations;
    for (const PhotoItem& item : m_items) {
        rotations.append(item.rotation());
    }
    return rotations;
}
