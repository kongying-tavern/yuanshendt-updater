QT       += core gui
QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
#CONFIG +=console
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
#DESTDIR = ../release/
#多线程
QT += concurrent
QT += core
#引用文件
SOURCES += \
    HTTP.cpp \
    Json.cpp \
    MD5.cpp \
    Start.cpp \
    file.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    HTTP.h \
    Json.h \
    MD5.h \
    Sandefine.h \
    Start.h \
    file.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    YuanshendtUpdater_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    UpdateProgram.qrc

SUBDIRS += \
    Updater/Updater.pro

DISTFILES += \
    Updater/Updater_zh_CN.ts


#引用外部库
TEMPLATE = app
LIBS+= $$PWD"/curllib/libcurl-x64.dll"

win32: LIBS += -L$$PWD/curllib/ -lcurl

INCLUDEPATH += $$PWD/curllib
DEPENDPATH += $$PWD/curllib

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/curllib/curl.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/curllib/libcurl.a



win32:CONFIG(release, debug|release): LIBS += $$PWD/curllib/libcurl.a
else:win32:CONFIG(debug, debug|release): LIBS += $$PWD/curllib/libcurl.a
else:unix:!macx: LIBS += $$PWD/curllib/libcurl.a

INCLUDEPATH += $$PWD/curllib
DEPENDPATH += $$PWD/curllib
