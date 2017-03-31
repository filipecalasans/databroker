QT += gui core network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = ModuleMaster
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += /usr/local/include
LIBPATH += /usr/local/lib

LIBS += -lprotobuf
LIBS += -pthread
LIBS += -lpthread

CCFLAG += -pthread

SOURCES += main.cpp \
    ../ModuleExample/communication/tcpdataconnection.cpp \
    ../protocol/data.pb.cc \
    ../ModuleExample/communication/communication.cpp \
    ../ModuleExample/communication/udpdataconnection.cpp \
    ../ModuleExample/communication/tcpcontrolconnection.cpp \
    ../ModuleExample/communication/abstractcontrolconnection.cpp \
    ../ModuleExample/communication/abstractdatacontrol.cpp \
    ../protocol/control.pb.cc \
    mainwindow.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

#TEst disfferent socket type TCP/UDP.
#DEFINES += TCP_CONNECTION

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    ../ModuleExample/communication/abstractdataconnection.h \
    ../ModuleExample/communication/tcpdataconnection.h \
    ../protocol/data.pb.h \
    ../ModuleExample/communication/communication.h \
    ../ModuleExample/communication/udpdataconnection.h \
    ../ModuleExample/communication/tcpcontrolconnection.h \
    ../ModuleExample/communication/abstractcontrolconnection.h \
    ../protocol/control.pb.h \
    mainwindow.h

FORMS    += mainwindow.ui

DISTFILES += \
    ../protocol/data.proto \
    ../protocol/control.proto
