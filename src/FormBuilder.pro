#-------------------------------------------------
#
# Project created by QtCreator 2014-09-11T13:32:23
#
#-------------------------------------------------

QT       += core gui xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FormBuilder
TEMPLATE = app


SOURCES += main.cpp\
        formbuilder.cpp\
D:\libs\jsoncpp-master\dist\jsoncpp.cpp

HEADERS  += formbuilder.h

LIBS += D:\GitHub\gdal-build\gdal20.lib

INCLUDEPATH += D:\GitHub\gdal\gdal\ogr\ogrsf_frmts
INCLUDEPATH += D:\GitHub\gdal\gdal\ogr
INCLUDEPATH += D:\GitHub\gdal\gdal\port
INCLUDEPATH += D:\GitHub\gdal\gdal\gcore
INCLUDEPATH += D:\GitHub\gdal\gdal\frmts
INCLUDEPATH += D:\GitHub\gdal\gdal\gnm

INCLUDEPATH += D:\GitHub\gdal-build\port # пришлось добавить т.к. почему-то в /port теперь
                                         # отсутствует cpl_congig.h

#INCLUDEPATH += D:\libs\jsoncpp-master\dist

FORMS    += formbuilder.ui

RESOURCES += \
    res.qrc
