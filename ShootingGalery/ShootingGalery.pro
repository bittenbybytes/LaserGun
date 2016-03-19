#-------------------------------------------------
#
# Project created by QtCreator 2016-03-18T20:22:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ShootingGalery
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp\
        TargetController.cpp

HEADERS  += mainwindow.h\
        TargetController.h

FORMS    += mainwindow.ui

INCLUDEPATH += /usr/local/include/opencv

LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_highgui
