#-------------------------------------------------
#
# Project created by QtCreator 2014-05-29T17:13:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UtilMOKE
TEMPLATE = app


SOURCES += main.cpp\
        utilmoke.cpp \
    daqmirroraxis.cpp

HEADERS  += \
    daqmirroraxis.h \
    utilmoke.h \
    NIDAQmx.h



FORMS    += utilmoke.ui

win32: LIBS += -L$$PWD/ -lNIDAQmx

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/
