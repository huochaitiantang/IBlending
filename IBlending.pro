#-------------------------------------------------
#
# Project created by QtCreator 2017-07-22T20:57:02
#
#-------------------------------------------------
OPENCV_DIR = /home/liuliang/opencv3.2

INCLUDEPATH += include
INCLUDEPATH += $$OPENCV_DIR/include

LIBS += -L$$OPENCV_DIR/lib
LIBS += $$OPENCV_DIR/lib/libopencv_core.so \
        $$OPENCV_DIR/lib/libopencv_imgproc.so \
        $$OPENCV_DIR/lib/libopencv_highgui.so \
        $$OPENCV_DIR/lib/libopencv_imgcodecs.so \
        $$OPENCV_DIR/lib/libopencv_photo.so \

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IBlending
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
    src/frontend/main.cpp \
    src/frontend/mainwindow.cpp \
    src/frontend/util.cpp \
    src/frontend/imglabel.cpp \
    src/frontend/srcimglabel.cpp \
    src/frontend/desimglabel.cpp \
    src/frontend/editwindow.cpp \
    src/frontend/editimglabel.cpp \
    src/backend/api.cpp \
    src/backend/tools.cpp \
    src/backend/solve_equation.cpp \
    src/backend/poisson_rect.cpp \
    src/backend/poisson_poly.cpp \
    src/backend/drag_drop.cpp \
    src/backend/poisson_fr.cpp


HEADERS += \
    include/mainwindow.h \
    include/util.h \
    include/imglabel.h \
    include/srcimglabel.h \
    include/desimglabel.h \
    include/editwindow.h \
    include/editimglabel.h \
    include/algorithms.h \
    src/backend/poisson_fr.h

FORMS += \
    ui/mainwindow.ui \
    ui/editwindow.ui

