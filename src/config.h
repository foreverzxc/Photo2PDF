// exportpdfconfig.h
#ifndef EXPORTPDFCONFIG_H
#define EXPORTPDFCONFIG_H

#include <QString>
#include <QSize>

enum class ResizeOption {
    NoResize,
    ResizeShortSide,
    ResizeLongSide
};

struct ExportPDFConfig {
    int dpi = 300;
    QString outputPath;
    ResizeOption resizeOption = ResizeOption::NoResize;
    int configPixel = 0;
    bool noExpand = false;
};

#endif // EXPORTPDFCONFIG_H

