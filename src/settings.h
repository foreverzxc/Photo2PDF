#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QString>
#include "config.h"

class Settings : public QObject
{
    Q_OBJECT

public:
    static Settings& instance();
    static const QString kOrganizationName;
    static const QString kApplicationName;
    static const QString kSettingsFileName;

    // Export config
    ExportPDFConfig exportConfig() const;
    void setExportConfig(const ExportPDFConfig& config);

    // Individual settings
    QString outputPath() const;
    void setOutputPath(const QString& path);

    int dpi() const;
    void setDpi(int dpi);

    ResizeOption resizeOption() const;
    void setResizeOption(ResizeOption option);

    int configPixel() const;
    void setConfigPixel(int pixel);

    bool noExpand() const;
    void setNoExpand(bool noExpand);

    // Window settings
    QByteArray windowGeometry() const;
    void setWindowGeometry(const QByteArray& geometry);

    QByteArray windowState() const;
    void setWindowState(const QByteArray& state);

    // Recent directories
    QString lastOpenDirectory() const;
    void setLastOpenDirectory(const QString& dir);

    QString lastSaveDirectory() const;
    void setLastSaveDirectory(const QString& dir);

    // Utility
    void sync();
    void load();
    void save();

signals:
    void configChanged(const ExportPDFConfig& config);

private:
    Settings();
    ~Settings() = default;
    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;

    QSettings m_settings;
};

#endif // SETTINGS_H
