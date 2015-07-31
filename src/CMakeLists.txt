#Note that setting the minimum required CMake version to 2.8.11 
#is required for automatic linking to the qtmain.lib library on Windows.
cmake_minimum_required(VERSION 2.8.11)

project(fb)

#if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(CMAKE_PREFIX_PATH "D:\\software\\qt\\qt-5.4.0-build")
#else(... MATCHES ...)
#    set(...)
#endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

#set (FILES_TO_TRANSLATE ) #http://www.itk.org/Wiki/CMake:How_To_Build_Qt4_Software

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)
  
# Widgets finds its own dependencies (QtGui and QtCore).  
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5LinguistTools)

# The Qt5Widgets_INCLUDES also includes the include directories for
# dependencies QtCore and QtGui
include_directories(${Qt5Widgets_INCLUDES})
include_directories(${Qt5Network_INCLUDES})
include_directories(${Qt5LinguistTools_INCLUDES})

#TODO:
#find_package(GDAL REQUIRED)
#if(GDAL_FOUND)
#    include_directories(${GDAL_INCLUDE_DIR})
#    FOREACH(file_path ${GDAL_INCLUDE_DIR})
#        include_directories(${file_path}/ogr)
#        include_directories(${file_path}/ogr/ogrsf_frmts)
#        include_directories(${file_path}/port)
#        include_directories(${file_path}/gcore)
#        include_directories(${file_path}/alg)
#    ENDFOREACH()
#endif(GDAL_FOUND)

#TODO:
# find CURL for gdal required

####################### TEMP GDAL 2.0 ###################################################
#find_library(GDAL_LIBRARY NAMES "gdal20.lib" PATHS "D:\\libs\\gdal_2.0_for_use-build\\Release")
#include_directories("D:\\libs\\gdal_2.0_for_use-build\\install\\include")
#########################################################################################

####################### TEMP GDAL 2.1.0 #################################################
find_library(GDAL_LIBRARY NAMES "gdal_i.lib" PATHS "D:\\libs\\gdal_2.1.0_for_use-build\\lib")
include_directories("D:\\libs\\gdal_2.1.0_for_use-build\\include")
#########################################################################################

set(APP_NAME fb)

qt5_wrap_ui(fb_ui fb.ui)
set(UIS_HDRS ${fb_ui})

qt5_add_resources(fb_res res.qrc)
set(RESOURCES ${fb_res})

set(PROJECT_SOURCES
jsoncpp.cpp
main.cpp
fb.cpp
attributes.cpp
elements.cpp
factories.cpp
project.cpp
insertwidget.cpp
ngwdialog.cpp
listvaluesdialog.cpp
dlistvaluesdialog.cpp
progressdialog.cpp
aboutdialog.cpp)

# TODO: make automatic updating *.ts files via qt5_create_translation()
# Use lupdate utility directly for now.
set(TS_FILES translations/fb_tr_en.ts) # manually written .ts files
qt5_add_translation(QM_FILES ${TS_FILES}) # no lupdate calling.

if(WIN32)
    add_executable(${APP_NAME} WIN32 ${PROJECT_SOURCES} ${RESOURCES} ${UIS_HDRS} ${QM_FILES})
else(WIN32)
    add_executable(${APP_NAME} ${PROJECT_SOURCES} ${RESOURCES} ${UIS_HDRS} ${QM_FILES})
endif(WIN32)

target_link_libraries(${APP_NAME} ${GDAL_LIBRARY})

qt5_use_modules(${APP_NAME} Widgets Network LinguistTools)
