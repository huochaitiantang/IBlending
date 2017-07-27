#-------------------------------------------------
#
# Project created by QtCreator 2017-07-22T20:57:02
#
#-------------------------------------------------

INCLUDEPATH += /home/sensetime/libs/include
#INCLUDEPATH += /home/sensetime/gradient_fusion

LIBS += -L/home/sensetime/libs/lib
# add down error!!!!
# -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_photo -lopencv_imgcodecs
LIBS += /home/sensetime/gradient_fusion/lmath.o \
        /home/sensetime/gradient_fusion/poisson.o \
        /home/sensetime/gradient_fusion/tools.o \
        /home/sensetime/gradient_fusion/test.o \
        /home/sensetime/gradient_fusion/api.o \
        /home/sensetime/libs/lib/libopencv_core.so \
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
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui



