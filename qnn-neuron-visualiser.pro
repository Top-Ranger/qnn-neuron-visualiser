#-------------------------------------------------
#
# Project created by QtCreator 2015-10-26T12:27:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = qnn-neuron-visualiser
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += src/main.cpp \
    src/qnnneuronvisualiser.cpp \
    src/output.cpp

HEADERS  += \
    src/additionalsimulationfunctions.hpp \
    src/qnnneuronvisualiser.h \
    src/output.h

FORMS    += \
    src/qnnneuronvisualiser.ui \
    src/output.ui

unix: LIBS += -L$$PWD/../qnn/ -lqnn
win32: LIBS += -L$$PWD/../qnn/ -lqnn0

unix|win32: LIBS += -lqcustomplot

INCLUDEPATH += $$PWD/../qnn/src
DEPENDPATH += $$PWD/../qnn/src

OTHER_FILES += \
    LICENSE.GPL3

DISTFILES += \
    README
