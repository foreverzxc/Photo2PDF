#include "settings.h"
#include <QByteArray>
#include <QRect>

const QString Settings::kOrganizationName = "Photo2PDF";
const QString Settings::kApplicationName = "Photo2PDF";
const QString Settings::kSettingsFileName = "photo2pdf.ini";

Settings& Settings::instance()
{
    static Settings instance;
    return instance;
}

Settings::Settings()
    : QObject(nullptr)
    , m_settings(QSettings::IniFormat, QSettings::UserScope,
                 kOrganizationName, kApplicationName)
{
    load();
}

ExportPDFConfig Settings::exportConfig() const
{
    ExportPDFConfig config;
    config.outputPath = m_settings.value("Export/outputPath", "").toString();
    config.dpi = m_settings.value("Export/dpi", 300).toInt();
    config.resizeOption = static_cast<ResizeOption>(
        m_settings.value("Export/resizeOption", 0).toInt());
    config.configPixel = m_settings.value("Export/configPixel", 0).toInt();
    config.noExpand = m_settings.value("Export/noExpand", false).toBool();
    return config;
}

void Settings::setExportConfig(const ExportPDFConfig& config)
{
    m_settings.setValue("Export/outputPath", config.outputPath);
    m_settings.setValue("Export/dpi", config.dpi);
    m_settings.setValue("Export/resizeOption", static_cast<int>(config.resizeOption));
    m_settings.setValue("Export/configPixel", config.configPixel);
    m_settings.setValue("Export/noExpand", config.noExpand);
    emit configChanged(config);
}

QString Settings::outputPath() const
{
    return m_settings.value("Export/outputPath", "").toString();
}

void Settings::setOutputPath(const QString& path)
{
    m_settings.setValue("Export/outputPath", path);
}

int Settings::dpi() const
{
    return m_settings.value("Export/dpi", 300).toInt();
}

void Settings::setDpi(int dpi)
{
    m_settings.setValue("Export/dpi", dpi);
}

ResizeOption Settings::resizeOption() const
{
    return static_cast<ResizeOption>(
        m_settings.value("Export/resizeOption", 0).toInt());
}

void Settings::setResizeOption(ResizeOption option)
{
    m_settings.setValue("Export/resizeOption", static_cast<int>(option));
}

int Settings::configPixel() const
{
    return m_settings.value("Export/configPixel", 0).toInt();
}

void Settings::setConfigPixel(int pixel)
{
    m_settings.setValue("Export/configPixel", pixel);
}

bool Settings::noExpand() const
{
    return m_settings.value("Export/noExpand", false).toBool();
}

void Settings::setNoExpand(bool noExpand)
{
    m_settings.setValue("Export/noExpand", noExpand);
}

QByteArray Settings::windowGeometry() const
{
    return m_settings.value("Window/geometry").toByteArray();
}

void Settings::setWindowGeometry(const QByteArray& geometry)
{
    m_settings.setValue("Window/geometry", geometry);
}

QByteArray Settings::windowState() const
{
    return m_settings.value("Window/state").toByteArray();
}

void Settings::setWindowState(const QByteArray& state)
{
    m_settings.setValue("Window/state", state);
}

QString Settings::lastOpenDirectory() const
{
    return m_settings.value("Paths/lastOpenDir", ".").toString();
}

void Settings::setLastOpenDirectory(const QString& dir)
{
    m_settings.setValue("Paths/lastOpenDir", dir);
}

QString Settings::lastSaveDirectory() const
{
    return m_settings.value("Paths/lastSaveDir", ".").toString();
}

void Settings::setLastSaveDirectory(const QString& dir)
{
    m_settings.setValue("Paths/lastSaveDir", dir);
}

QString Settings::language() const
{
    return m_settings.value("Language", "").toString();
}

void Settings::setLanguage(const QString& language)
{
    m_settings.setValue("Language", language);
}

void Settings::sync()
{
    m_settings.sync();
}

void Settings::load()
{
    // Settings are automatically loaded from QSettings constructor
}

void Settings::save()
{
    sync();
}
