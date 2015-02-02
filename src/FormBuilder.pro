#-------------------------------------------------
#
# Project created by QtCreator 2014-09-11T13:32:23
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FormBuilder
TEMPLATE = app


SOURCES += main.cpp \
           formbuilder.cpp \
           jsoncpp.cpp \
           formbuilderproject.cpp

HEADERS  += formbuilder.h 

# ----------- static gdal --------------------------------------
LIBS += D:/libs/libiconv-build/install/lib/x86/libiconv.lib
LIBS += D:/libs/curl-master-build/install/lib/libcurl.lib
LIBS += D:/Projects/FormBuilder/gdal_svn-cmake4gdal-build/install/lib/x86/gdal20.lib
INCLUDEPATH += D:/Projects/FormBuilder/gdal_svn-cmake4gdal-build/install/include
INCLUDEPATH += D:/libs/curl-master-build/install/include
# --------------------------------------------------------------

# ----------- dynamic gdal debug ---------------------------------------
#INCLUDEPATH += D:/GitHub/gdal-build/include
#LIBS += D:/GitHub/gdal-build/gdal20d.lib
## note: compile this gdal project with Qt Creator
# ----------------------------------------------------------------------

FORMS    += formbuilder.ui

RC_FILE = forappicon.rc

RESOURCES += \
    res.qrc

