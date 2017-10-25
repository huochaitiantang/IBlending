#-------------------------------------------------
#
# Project created by QtCreator 2017-07-22T20:57:02
#
#-------------------------------------------------

INCLUDEPATH += include
INCLUDEPATH += /home/liuliang/opencv3.2/include

LIBS += -L/home/liuliang/opencv3.2/lib

LIBS += /home/liuliang/opencv3.2/lib/libopencv_core.so \
        /home/liuliang/opencv3.2/lib/libopencv_imgproc.so \
        /home/liuliang/opencv3.2/lib/libopencv_highgui.so \
        /home/liuliang/opencv3.2/lib/libopencv_imgcodecs.so \
        /home/liuliang/opencv3.2/lib/libopencv_photo.so \

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
    src/main.cpp \
    src/mainwindow.cpp \
    src/util.cpp \
    src/imglabel.cpp \
    src/srcimglabel.cpp \
    src/desimglabel.cpp \
    src/editwindow.cpp \
    src/editimglabel.cpp \
    src/algorithms.cpp

HEADERS += \
    include/mainwindow.h \
    include/util.h \
    include/imglabel.h \
    include/srcimglabel.h \
    include/desimglabel.h \
    include/editwindow.h \
    include/editimglabel.h \
    include/algorithms.h

FORMS += \
    ui/mainwindow.ui \
    ui/editwindow.ui

