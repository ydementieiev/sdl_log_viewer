include(gtest_dependency.pri)

TEMPLATE = app
CONFIG += console c++11
#CONFIG -= app_bundle
CONFIG += thread
#CONFIG -= qt
QT += core gui
#QT += widgets
#QT += gui declarative
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


INCLUDEPATH += ../include

HEADERS += \
#        ..\include\mainwindow.h \
#        ..\include\default_config.h \
        tst_test.h

SOURCES += \
        main.cpp \
#        ..\src\mainwindow.cpp \
        ..\src\table.cpp \
#        src/autologs.cpp \
        ../src/line_parser.cpp \
#        src/default_config.cpp


#FORMS += \
#        ..\ui\mainwindow.ui

## Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target
