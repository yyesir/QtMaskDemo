#-------------------------------------------------
#
# Project created by QtCreator 2022-12-13T09:10:09
#
#-------------------------------------------------

QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtMaskDemo
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    myvideosurface.cpp \
    convert.cpp \
    FaceDetectionAlgo.cpp

HEADERS  += mainwindow.h \
    myvideosurface.h \
    convert.h \
    FaceDetectionAlgo.h

FORMS    += mainwindow.ui


win32 {
    #顾名思义，这里就是设置Windows下的连接库，以及Visual C++2010编译器的的设置选项
    INCLUDEPATH += 3rdParty\opencv\include
    DEPENDPATH += 3rdParty\opencv\include
    LIBS += $$PWD\3rdParty\libaeyefacedetect.lib -L$$PWD\3rdParty\opencv\lib -lopencv_imgproc243 -lopencv_core243 -lopencv_highgui243 -lopencv_video243
}

unix {

}
