// exportpdfconfig.h
#ifndef EXPORTPDFCONFIG_H
#define EXPORTPDFCONFIG_H

#include <QString>
#include <QSize>

enum class ResizeOption {
    NoResize,         // 不改变图片大小
    ResizeShortSide,  // 改变短边
    ResizeLongSide    // 改变长边
};

struct ExportPDFConfig {
    int dpi = 300;          // 输出的DPI
    QString outputPath;
    ResizeOption resizeOption = ResizeOption::NoResize;  // 改变图片大小的选项
    int configPixel = 0;
    bool noExpand = false;
};

#endif // EXPORTPDFCONFIG_H

