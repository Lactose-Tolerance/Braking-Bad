# driver.pro

QT       += core gui widgets
CONFIG   += c++17

TARGET = driver
TEMPLATE = app

# List all header files here
HEADERS += \
    carBody.h \
    constants.h \
    intro.h \
    mainwindow.h \
    point.h \
    wheel.h \
    line.h

# List all source files here
SOURCES += \
    carBody.cpp \
    intro.cpp \
    main.cpp \
    mainwindow.cpp \
    point.cpp \
    wheel.cpp \
    line.cpp
FORMS += \
    mainwindow.ui
