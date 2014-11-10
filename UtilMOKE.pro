#-------------------------------------------------
#
# Project created by QtCreator 2014-05-29T17:13:08
#
#-------------------------------------------------

QT       += core gui webkit axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = UtilMOKE
TEMPLATE = app

CONFIG += axcontainer

SOURCES += main.cpp\
        utilmoke.cpp \
    daqmagcontrol.cpp \
    pximirroraxes.cpp \
    qcustomplot.cpp \
    lockin7265.cpp \
    mokedata.cpp \
    keithley2k.cpp \
    lockin7270.cpp \
    aptangle.cpp \
    APT/APT.cpp

HEADERS  += \
    utilmoke.h \
    NIDAQmx.h \
    daqmagcontrol.h \
    pximirroraxes.h \
    qcustomplot.h \
    lockin7265.h \
    gpib.h \
    ni488.h \
    mokedata.h \
    keithley2k.h \
    lockin7270.h \
    aptangle.h \
    APT/APT.h



FORMS    += utilmoke.ui

win32: LIBS += -L$$PWD/ -lNIDAQmx
LIBS += $$PWD/libgpib.a

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/



