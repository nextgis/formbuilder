#***************************************************************************
# Project:  NextGIS Form Builder
# Purpose:  cmake script
#***************************************************************************
# Copyright (c) 2014, 2015, 2016 NextGIS
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
#***************************************************************************

#Note that setting the minimum required CMake version to 2.8.11 
#is required for automatic linking to the qtmain.lib library on Windows.
cmake_minimum_required(VERSION 2.8.11)
project(fb)

# Settings for nextgis-extra
# set path to additional CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

#if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
# path example: <qt core dir>/qtbase
if(WIN32)
    set(QT_DIR_PREFIX_PATH "C:/Qt" CACHE PATH "This path will be set to CMAKE_PREFIX_PATH variable")
    set(CMAKE_PREFIX_PATH ${QT_DIR_PREFIX_PATH})
endif(WIN32)

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# To correctly build for Win XP
#set(CMAKE_EXE_LINKER_FLAGS "/SUBSYSTEM:WINDOWS,5.01")
if(WIN32)
  add_definitions(-D_WIN32_WINNT=0x0501)
endif(WIN32)


###############################################################################
## includes & find ############################################################
###############################################################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/json)

include(FindAnyProject)

# path example: <qt core dir>/qtbase/lib/cmake/Qt5Core
find_package(Qt5Widgets REQUIRED) # finds its own dependencies (QtGui and QtCore)
find_package(Qt5Network REQUIRED)
find_package(Qt5LinguistTools) # TODO: do we need it here?

include_directories(${Qt5Widgets_INCLUDES}) # also includes QtCore and QtGui
include_directories(${Qt5Network_INCLUDES})
include_directories(${Qt5LinguistTools_INCLUDES})

#find_package(GDAL REQUIRED)
find_anyproject(GDAL REQUIRED)# VERSION 2.1)

if(GDAL_FOUND)
    include_directories(${GDAL_INCLUDE_DIR})
    FOREACH(FILE_PATH ${GDAL_INCLUDE_DIR})
        include_directories(${FILE_PATH}/ogr)
        include_directories(${FILE_PATH}/ogr/ogrsf_frmts)
        include_directories(${FILE_PATH}/port)
        include_directories(${FILE_PATH}/gcore)
        include_directories(${FILE_PATH}/alg)
        include_directories(${FILE_PATH}/frmts/vrt)
    ENDFOREACH()
endif(GDAL_FOUND)


###############################################################################
## target definitions #########################################################
###############################################################################

set(APP_NAME fb)

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
aboutdialog.cpp
newvoiddialog.cpp
fieldmanagerdialog.cpp
datetimedialog.cpp
forappicon.rc)

qt5_wrap_ui(fb_ui fb.ui)
set(UIS_HDRS ${fb_ui})

qt5_add_resources(fb_res res.qrc)
set(RESOURCES ${fb_res})

if(WIN32)
    add_executable(${APP_NAME} WIN32 ${PROJECT_SOURCES} ${RESOURCES} ${UIS_HDRS} ${QM_FILES})
else(WIN32)
    add_executable(${APP_NAME} ${PROJECT_SOURCES} ${RESOURCES} ${UIS_HDRS} ${QM_FILES})
endif(WIN32)

# TODO: make automatic updating *.ts files via qt5_create_translation()
# Use lupdate utility directly for now.
set(TS_FILES translations/fb_en_GB.ts) # manually written .ts files
qt5_add_translation(QM_FILES ${TS_FILES}) # no lupdate calling.


###############################################################################
## dependencies ###############################################################
###############################################################################

qt5_use_modules(${APP_NAME} Widgets Network LinguistTools)

message(STATUS "TARGET_LINK_LIB ${TARGET_LINK_LIB}")

#target_link_libraries(${APP_NAME} ${GDAL_LIBRARIES})
target_link_extlibraries(${APP_NAME} ${GDAL_LIBRARIES})


###############################################################################
## installation ###############################################################
###############################################################################

if(WIN32)
    install(TARGETS ${APP_NAME} RUNTIME DESTINATION ./)
else(WIN32)
    install(TARGETS ${APP_NAME}
        RUNTIME DESTINATION bin
        )
endif(WIN32)

# TODO: find Qt macros for install release stuff (Qt dlls, /platforms dir, etc)
if(WIN32)
    install(FILES ${QT_DIR_PREFIX_PATH}/lib/Qt5Core.dll DESTINATION ./)
    install(FILES ${QT_DIR_PREFIX_PATH}/lib/Qt5Gui.dll DESTINATION ./)
    install(FILES ${QT_DIR_PREFIX_PATH}/lib/Qt5Widgets.dll DESTINATION ./)
    install(FILES ${QT_DIR_PREFIX_PATH}/lib/Qt5Network.dll DESTINATION ./)
    install(FILES ${QT_DIR_PREFIX_PATH}/plugins/platforms/qwindows.dll DESTINATION platforms)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gdal201.dll DESTINATION ./)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libcurl.dll DESTINATION ./)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libeay32.dll DESTINATION ./)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libiconv.dll DESTINATION ./)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/msvcp110.dll DESTINATION ./)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/msvcr110.dll DESTINATION ./)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/proj.dll DESTINATION ./)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ssleay32.dll DESTINATION ./)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/zlib.dll DESTINATION ./)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/fb_en_GB.qm DESTINATION ./)
endif(WIN32)


###############################################################################
## packaging ##################################################################
###############################################################################

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPackOptions.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/CPackOptions.cmake @ONLY)

#INCLUDE(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "NextGIS Form Builder")
set(CPACK_PACKAGE_VENDOR "NextGIS")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "nextgis/form-builder")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "ngfb")
set(CPACK_PROJECT_CONFIG_FILE ${CMAKE_BINARY_DIR}/CPackOptions.cmake)
if(WIN32 AND NOT UNIX)
        set(CPACK_BINARY_NSIS ON)
        set(CPACK_BINARY_ZIP ON)
        set(CPACK_NSIS_MODIFY_PATH ON)
        set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
else(WIN32 AND NOT UNIX)
  #set(CPACK_STRIP_FILES "bin/wxgiscat")
  #set(CPACK_SOURCE_STRIP_FILES "")
endif(WIN32 AND NOT UNIX)
set(CPACK_PACKAGE_EXECUTABLES "ngfb" "NextGIS Form Builder")

include(CPack)



