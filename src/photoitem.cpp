#include "photoitem.h"
#include <QFileInfo>

PhotoItem::PhotoItem(const QString& filePath, int pageNumber)
    : m_filePath(filePath)
    , m_pageNumber(pageNumber)
{
    m_type = (pageNumber >= 0) ? Type::PdfPage : Type::Image;
}

QString PhotoItem::displayName() const
{
    QFileInfo info(m_filePath);
    QString name = info.fileName();

    if (m_type == Type::PdfPage) {
        name += QString(" (Page %1)").arg(m_pageNumber + 1);
    }

    return name;
}

QString PhotoItem::displayPage() const
{
    return QString();  // Now combined with displayName
}
