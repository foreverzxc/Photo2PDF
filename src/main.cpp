#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Set application metadata
    a.setApplicationName("Photo2PDF");
    a.setApplicationVersion("1.0");

    // Load translation based on saved language preference
    QTranslator translator;
    QString language = Settings::instance().language();

    if (language == "en") {
        if (translator.load(":/i18n/translations/Photo2PDF_en.qm")) {
            a.installTranslator(&translator);
        }
    } else if (language == "zh_CN") {
        if (translator.load(":/i18n/translations/Photo2PDF_zh_CN.qm")) {
            a.installTranslator(&translator);
        }
    } else {
        // Auto-detect system language
        const QStringList uiLanguages = QLocale::system().uiLanguages();
        for (const QString &locale : uiLanguages) {
            const QString baseName = "translations/Photo2PDF_" + QLocale(locale).name();
            if (translator.load(":/i18n/" + baseName + ".qm")) {
                a.installTranslator(&translator);
                break;
            }
        }
    }

    MainWindow w;
    w.show();
    return a.exec();
}
