#-------------------------------------------------
#
# Project created by QtCreator 2020-01-12T22:47:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Log_viewer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#CONFIG += c++11

INCLUDEPATH +=  \
        ./src/config/include \
        ./src/utils/include \
        ./src/widgets/include \
        ./src/windows/include


HEADERS += \
    src/windows/include/mainwindow.h \
    src/windows/include/configwindow.h \
    src/windows/include/consolescreen.h \
    src/utils/include/search_hightlight.h \

SOURCES += \
    main.cpp \
    src/windows/src/mainwindow.cpp \
    src/windows/src/configwindow.cpp \
    src/windows/src/consolescreen.cpp \
    src/config/src/default_config.cpp \
    src/widgets/src/table.cpp \
    src/widgets/src/button_hide_all_opened_rows.cpp \
    src/utils/src/logger.cpp \
    src/utils/src/line_parser.cpp \
    src/utils/src/search_hightlight.cpp \
    src/widgets/src/progress_bar.cpp \
    src/widgets/src/main_table_atf.cpp \
    src/widgets/src/filter_tree.cpp \


FORMS += \
        src/ui/mainwindow.ui \
        src/ui/configwindow.ui \
        src/ui/consolescreen.ui \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    src/config/config.txt
