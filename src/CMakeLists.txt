#*************************************************************************** 
# Project:  NextGIS Formbuilder
# Purpose:  cmake script
# Author:  Mikhail Gusev, gusevmihs@gmail.com
#***************************************************************************
#   Copyright (C) 2014-2016 NextGIS
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#***************************************************************************

cmake_minimum_required(VERSION 2.8.11)

# To correctly build for Win XP?
#set(CMAKE_EXE_LINKER_FLAGS "/SUBSYSTEM:WINDOWS,5.01")
if(WIN32)
  add_definitions(-D_WIN32_WINNT=0x0501)
endif(WIN32)

###############################################################################
## subdirs ####################################################################
###############################################################################

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/form)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/project)

###############################################################################
## target definitions #########################################################
###############################################################################

set(APP_NAME fb)

set(PROJECT_SOURCES
jsoncpp.cpp
main.cpp
fb.cpp
screen.cpp
dialognewvoid.cpp
dialogngw.cpp
dialogfieldmanager.cpp
appicon.rc
)

qt5_wrap_ui(fb_ui fb.ui)
set(UI_HDRS ${fb_ui})

qt5_add_resources(fb_res res.qrc)
set(RESOURCES ${fb_res})

if(WIN32)
    add_executable(${APP_NAME} WIN32 ${PROJECT_SOURCES} ${FB_TARGET_OBJECTS} ${RESOURCES} ${UI_HDRS})# ${QM_FILES})
else(WIN32)
    add_executable(${APP_NAME} ${PROJECT_SOURCES} ${FB_TARGET_OBJECTS} ${RESOURCES} ${UI_HDRS})# ${QM_FILES})
endif(WIN32)

#
## TODO: make automatic updating *.ts files via qt5_create_translation()
## Use lupdate utility directly for now.
#set(TS_FILES translations/fb_en_GB.ts) # manually written .ts files
#qt5_add_translation(QM_FILES ${TS_FILES}) # no lupdate calling.
#

###############################################################################
## dependencies ###############################################################
###############################################################################

target_link_libraries(${APP_NAME} Qt5::Widgets Qt5::Network)# Qt5::LinguistTools)
target_link_extlibraries(${APP_NAME}) # will link GDAL automatically
