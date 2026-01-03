QT       += core gui widgets printsupport pdf

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/photomanager.cpp \
    src/tablewidgetupdown.cpp \
    src/photoitem.cpp \
    src/photomodel.cpp \
    src/settings.cpp

HEADERS += \
    src/config.h \
    src/exportpdfworker.h \
    src/mainwindow.h \
    src/photomanager.h \
    src/tablewidgetupdown.h \
    src/transformers.h \
    src/photoitem.h \
    src/photomodel.h \
    src/settings.h

RESOURCES += \
    resources/icon.qrc \
    resources/translations.qrc

FORMS += \
    ui/mainwindow.ui

# Include path for custom widgets in UI files
INCLUDEPATH += $$PWD/src

TRANSLATIONS += \
    translations/Photo2PDF_zh_CN.ts \
    translations/Photo2PDF_en.ts

TRANSLATIONS_DIR = $$PWD/translations
CONFIG += lrelease

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
