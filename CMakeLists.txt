#*************************************************************************** 
# Project:  NextGIS Formbuilder
# Purpose:  main cmake script
# Author:  Mikhail Gusev, gusevmihs@gmail.com
#***************************************************************************
#   Copyright (C) 2014-2017 NextGIS
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

#Note that setting the minimum required CMake version to 2.8.11 is required for 
#automatic linking to the qtmain.lib library on Windows.
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
project(fb)

set(VENDOR_NAME "nextgis")

# some init settings
set(CMAKE_COLOR_MAKEFILE ON)
# set path to additional CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

include(util)

# From docs:
# The easiest way to use CMake is to set the CMAKE_PREFIX_PATH environment variable to the install prefix of Qt 5.
# path example: <qt core dir>/qtbase
if(WIN32)
    set(QT_DIR_PREFIX_PATH "C:/Qt" CACHE PATH "This path will be set to CMAKE_PREFIX_PATH variable")
    set(CMAKE_PREFIX_PATH ${QT_DIR_PREFIX_PATH})
endif(WIN32)

set(BUILD_SHARED_LIBS ON CACHE BOOL "Link libraries dinamically")

include(GNUInstallDirs)

set(INSTALL_BIN_DIR ${CMAKE_INSTALL_BINDIR} CACHE INTERNAL "Installation directory for executables" FORCE)
set(INSTALL_LIB_DIR ${CMAKE_INSTALL_LIBDIR} CACHE INTERNAL "Installation directory for libraries" FORCE)
set(INSTALL_INC_DIR ${CMAKE_INSTALL_INCLUDEDIR} CACHE INTERNAL "Installation directory for headers" FORCE)
set(INSTALL_SHARE_DIR "${CMAKE_INSTALL_DATADIR}/${VENDOR_NAME}/${PROJECT_NAME}" CACHE INTERNAL "Installation directory for share data" FORCE)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX) 
    set( CMAKE_CXX_FLAGS "-fpic ${CMAKE_CXX_FLAGS}" )
    set( CMAKE_C_FLAGS   "-fpic ${CMAKE_C_FLAGS}" )  
endif()

set(EXTRA_LIBS)
if(UNIX)
    find_package(Threads)
    set(EXTRA_LIBS ${EXTRA_LIBS} ${CMAKE_THREAD_LIBS_INIT})
    find_library(DL_LIB dl)
    set(EXTRA_LIBS ${EXTRA_LIBS} ${DL_LIB})
    find_library(M_LIB m)
    set(EXTRA_LIBS ${EXTRA_LIBS} ${M_LIB})
endif()

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

include(FindAnyProject)

# Need Qt >= 5 at least for QTemporaryDir class ...  
# path example: <qt core dir>/qtbase/lib/cmake/Qt5Core
find_package(Qt5Widgets REQUIRED) # finds its own dependencies (QtGui and QtCore)
find_package(Qt5Network REQUIRED)
find_package(Qt5NetworkAuth REQUIRED)
find_package(Qt5LinguistTools)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})
include_directories(${Qt5NetworkAuth_INCLUDE_DIRS})
include_directories(${Qt5LinguistTools_INCLUDE_DIRS})

include(extlib) # Find GDAL (required)

if(GDAL_FOUND AND NOT WITH_GDAL_EXTERNAL) 
    # Check version.
    # Need at least GDAL 2.1 because of VSI zip bug: https://trac.osgeo.org/gdal/ticket/6005
    set(GDAL_REQUIRED_VERSION 2.1) # external GDAL is always of correct version
    check_version(${GDAL_INCLUDE_DIR} GDAL_VERSION_MAJOR GDAL_VERSION_MINOR GDAL_VERSION_REV)
    set(GDAL_VERSION "${GDAL_VERSION_MAJOR}.${GDAL_VERSION_MINOR}")
    message("GDAL version: ${GDAL_VERSION}")
    if(${GDAL_VERSION} VERSION_LESS GDAL_REQUIRED_VERSION)
        message(FATAL_ERROR "GDAL must be of version " ${GDAL_REQUIRED_VERSION} " or higher!")
    endif()
endif()

if(WIN32)# AND WITH_GDAL_EXTERNAL) # TODO: move this to app cmake file
    add_definitions(-DFB_GDALDATA_IN_SHARE)
    # Note: if we have GDAL installed for Linux it will definitely have gdal data dir.
endif()

if(WIN32)
    add_definitions(-DFB_SYSTRANSLATIONS_IN_SHARE)
endif()

#option(USE_GDAL_LOG OFF) # TODO: move this to app cmake file
#if(USE_GDAL_LOG)
#    add_definitions(-DFB_GDAL_DEBUG="${CMAKE_INSTALL_PREFIX}/${INSTALL_BIN_DIR}/gdal-log.txt")
#endif()

# Build gui executable.
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Add to the installation path all including Qt dlls and /platform dir (needed for Windows).

#...
