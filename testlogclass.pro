# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

TEMPLATE = app
TARGET = testlogclass
DESTDIR = ../Debug
QT += core gui
CONFIG += debug \
		console
DEFINES += QT_LARGEFILE_SUPPORT
INCLUDEPATH += ./GeneratedFiles \
    ./GeneratedFiles/Debug \
    .
DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/debug
OBJECTS_DIR += debug
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
HEADERS += testlogclass.h \
			DeviceDebug.inl \
			RecordMng.h
			
SOURCES +=  testlogclass.cpp \
			RecordMng.cpp \
			main.cpp
FORMS   +=  testlogclass.ui

include(testlogclass.pri)
