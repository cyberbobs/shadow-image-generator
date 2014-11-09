#-------------------------------------------------
#
# Project created by QtCreator 2014-11-09T01:53:20
#
#-------------------------------------------------

QT     += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ShadowImageGenerator
TEMPLATE = app

SOURCES += src/main.cpp\
    src/MainWindow.cpp \
    src/ShadowSettings.cpp \
    src/GraphicsRoundedRectItem.cpp

HEADERS  += src/MainWindow.h \
    src/ShadowSettings.h \
    src/GraphicsRoundedRectItem.h

FORMS    += src/MainWindow.ui \
    src/ShadowSettings.ui

RESOURCES += \
    resources/resources.qrc
