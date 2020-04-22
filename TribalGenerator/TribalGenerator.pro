#-------------------------------------------------
#
# Project created by QtCreator 2013-02-11T22:29:40
#
#-------------------------------------------------

QT       += core gui sql widgets

TARGET = TribalGenerator
TEMPLATE = app

RESOURCES += TribalGenerator.qrc

#INCLUDEPATH += D:\User\Frameworks\Boost\boost_1_53_0

#LIBS += -L"D:\User\Frameworks\Boost\boost_1_53_0\libs"

SOURCES += main.cpp\
        mainwindow.cpp \
    Logic.cpp \
    DbController.cpp \
    DataWidget.cpp \
    AdjustDialog.cpp \
    DbControlWidget.cpp \
    DbManager.cpp \
    DbEditWidget.cpp

HEADERS  += mainwindow.h \
    Logic.h \
    DbController.h \
    DataWidget.h \
    AdjustDialog.h \
    DbControlWidget.h \
    DbManager.h \
    DbEditWidget.h \
    DbManagerDefs.h

TRANSLATIONS += translations/TribalGen_ru.ts \
               translations/TribalGen_ua.ts

RC_FILE = TribalGenerator.rc

VERSION = 1.0

DEFINES += APPLICATION_VERSION=\"\\\"$$VERSION\\\"\"
