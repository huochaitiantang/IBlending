#-------------------------------------------------
#
# Project created by QtCreator 2017-07-22T20:57:02
#
#-------------------------------------------------

INCLUDEPATH += /home/sensetime/libs/include

LIBS += /home/sensetime/libs/lib/libopencv_core.so \
        /home/sensetime/libs/lib/libopencv_imgproc.so \
        /home/sensetime/libs/lib/libopencv_highgui.so \
        /home/sensetime/libs/lib/libopencv_imgcodecs.so \
        /home/sensetime/libs/lib/libopencv_photo.so \


QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qttest
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    main.cpp \
    mainwindow.cpp \
    util.cpp \
    imglabel.cpp \
    srcimglabel.cpp \
    desimglabel.cpp \
    editwindow.cpp \
    editimglabel.cpp \
    algorithms.cpp

HEADERS += \
    mainwindow.h \
    util.h \
    imglabel.h \
    srcimglabel.h \
    desimglabel.h \
    editwindow.h \
    editimglabel.h \
    algorithms.h

FORMS += \
    mainwindow.ui \
    editwindow.ui



