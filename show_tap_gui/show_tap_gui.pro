QT += sensors gui core widgets


CONFIG += debug
TARGET = show_tap_gui
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    tap.cpp
HEADERS += mainwindow.h \
    tap.h
FORMS += mainwindow.ui
