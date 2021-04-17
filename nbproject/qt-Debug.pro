# This file is generated automatically. Do not edit.
# Use project properties -> Build -> Qt -> Expert -> Custom Definitions.
TEMPLATE = app
DESTDIR = dist/Debug/MinGW-Windows
TARGET = sdm_demo_qt6_thesaurus
VERSION = 1.0.0
CONFIG -= debug_and_release app_bundle lib_bundle
CONFIG += debug 
PKGCONFIG +=
QT = core gui widgets
SOURCES += DataStore.cpp MainForm.cpp main.cpp
HEADERS += DataStore.h MainForm.h
FORMS += MainForm.ui
RESOURCES += Qt_Thesaurus.qrc
TRANSLATIONS +=
OBJECTS_DIR = build/Debug/MinGW-Windows
MOC_DIR = 
RCC_DIR = 
UI_DIR = 
QMAKE_CC = gcc
QMAKE_CXX = g++
DEFINES += 
INCLUDEPATH += /C/Qt/6.0.3/mingw81_64/include 
LIBS += C:/Qt/6.0.3/mingw81_64/lib/libQt6Core.a C:/Qt/6.0.3/mingw81_64/lib/libQt6Gui.a C:/Qt/6.0.3/mingw81_64/lib/libQt6Sql.a C:/Qt/6.0.3/mingw81_64/lib/libQt6Widgets.a  
equals(QT_MAJOR_VERSION, 4) {
QMAKE_CXXFLAGS += -std=c++11
}
equals(QT_MAJOR_VERSION, 5) {
CONFIG += c++11
}
