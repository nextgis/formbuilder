#***************************************************************************
# Project:  NextGIS Form Builder
# Purpose:  cmake script
#***************************************************************************
# Copyright (c) 2014, 2015 NextGIS
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


#if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
if(WIN32)
    set(QT_DIR_PREFIX_PATH "C:/Qt" CACHE PATH "This path will be set to CMAKE_PREFIX_PATH variable")
	set(CMAKE_PREFIX_PATH ${QT_DIR_PREFIX_PATH})
endif(WIN32)

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)
  

# set path to additional CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})


# Qt5Widgets finds its own dependencies (QtGui and QtCore).
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5LinguistTools)

# The Qt5Widgets_INCLUDES also includes the include directories for
# dependencies QtCore and QtGui
include_directories(${Qt5Widgets_INCLUDES})
include_directories(${Qt5Network_INCLUDES})
include_directories(${Qt5LinguistTools_INCLUDES})


find_package(GDAL REQUIRED)
if(GDAL_FOUND)
    include_directories(${GDAL_INCLUDE_DIR})
    FOREACH(file_path ${GDAL_INCLUDE_DIR})
        include_directories(${file_path}/ogr)
        include_directories(${file_path}/ogr/ogrsf_frmts)
        include_directories(${file_path}/port)
        include_directories(${file_path}/gcore)
        include_directories(${file_path}/alg)
        include_directories(${file_path}/frmts/vrt)
    ENDFOREACH()
endif(GDAL_FOUND)


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

target_link_libraries(${APP_NAME} ${GDAL_LIBRARIES})

qt5_use_modules(${APP_NAME} Widgets Network LinguistTools)





#install

if(WIN32)
    install(TARGETS ${APP_NAME} RUNTIME DESTINATION ./)
else(WIN32)
    install(TARGETS ${APP_NAME}
        RUNTIME DESTINATION bin
        )
endif(WIN32)

# TODO: google for Qt macros for install release stuff (Qt dlls, /platforms dir, etc)

install(FILES ${QT_DIR_PREFIX_PATH}/lib/Qt5Core.dll DESTINATION ./)
install(FILES ${QT_DIR_PREFIX_PATH}/lib/Qt5Gui.dll DESTINATION ./)
install(FILES ${QT_DIR_PREFIX_PATH}/lib/Qt5Widgets.dll DESTINATION ./)
install(FILES ${QT_DIR_PREFIX_PATH}/lib/Qt5Network.dll DESTINATION ./)

if(WIN32)
    install(FILES ${QT_DIR_PREFIX_PATH}/plugins/platforms/qwindows.dll DESTINATION platforms)
endif(WIN32)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/fb_tr_en.qm DESTINATION ./)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gdal201.dll DESTINATION ./)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libcurl.dll DESTINATION ./)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libeay32.dll DESTINATION ./)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libiconv.dll DESTINATION ./)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/msvcp110.dll DESTINATION ./)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/msvcr110.dll DESTINATION ./)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/proj4.dll DESTINATION ./)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ssleay32.dll DESTINATION ./)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/zlib.dll DESTINATION ./)



#cpack

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPackOptions.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/CPackOptions.cmake @ONLY)

#INCLUDE(InstallRequiredSystemLibraries)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "NextGIS Form Builder")
SET(CPACK_PACKAGE_VENDOR "NextGIS")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "nextgis/form-builder")
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "ngfb")
SET(CPACK_PROJECT_CONFIG_FILE ${CMAKE_BINARY_DIR}/CPackOptions.cmake)
IF(WIN32 AND NOT UNIX)
        SET(CPACK_BINARY_NSIS ON)
        SET(CPACK_BINARY_ZIP ON)
        SET(CPACK_NSIS_MODIFY_PATH ON)
        SET(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
ELSE(WIN32 AND NOT UNIX)
  #SET(CPACK_STRIP_FILES "bin/wxgiscat")
  #SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)
SET(CPACK_PACKAGE_EXECUTABLES "ngfb" "NextGIS Form Builder")

INCLUDE(CPack)





