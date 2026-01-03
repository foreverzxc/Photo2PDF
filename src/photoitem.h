#ifndef PHOTOITEM_H
#define PHOTOITEM_H

#include <QString>
#include <QImage>

class PhotoItem
{
public:
    enum class Type {
        Image,
        PdfPage
    };

    PhotoItem() = default;
    explicit PhotoItem(const QString& filePath, int pageNumber = -1);

    // Getters
    QString filePath() const { return m_filePath; }
    int pageNumber() const { return m_pageNumber; }
    int rotation() const { return m_rotation; }
    Type type() const { return m_type; }
    bool isValid() const { return !m_filePath.isEmpty(); }

    // Setters
    void setRotation(int rotation) { m_rotation = rotation % 360; }
    void setFilePath(const QString& path) { m_filePath = path; }
    void setPageNumber(int page) { m_pageNumber = page; }

    // Utilities
    QString displayName() const;
    QString displayPage() const;

private:
    QString m_filePath;
    int m_pageNumber = -1;  // -1 for images, 0-based for PDF pages
    int m_rotation = 0;
    Type m_type = Type::Image;
};

#endif // PHOTOITEM_H
