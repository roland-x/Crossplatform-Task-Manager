#-------------------------------------------------
#
# Project created by QtCreator 2017-06-27T10:50:17
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = taskmgr
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


SOURCES += main.cpp\
    proclist.cpp \
    win_taskmgr.cpp \
    taskmgr.cpp \
    linux_taskmgr.cpp \
    view.cpp \
    dialog_priory.cpp

HEADERS  += taskmgr.h \
    proclist.h \
    win_taskmgr.h \
    linux_taskmgr.h \
    view.h \
    dialog_priory.h

FORMS    += taskmgr.ui \
    view.ui \
    dialog_priory.ui

win32 {
    LIBS +=-lpsapi
    //LIBS +="..\build-taskmgr-Desktop_Qt_5_8_0_MinGW_32bit-Debug\debug\info_proc.dll"
}

