#*************************************************************************** 
# Project:  NextGIS Formbuilder
# Purpose:  main cmake script
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

#Note that setting the minimum required CMake version to 2.8.11 
#is required for automatic linking to the qtmain.lib library on Windows.
cmake_minimum_required(VERSION 2.8.11)
project(fb)

# set path to additional CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# path example: <qt core dir>/qtbase
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

###############################################################################
## includes & find ############################################################
###############################################################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

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
## subtargets #################################################################
###############################################################################

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

###############################################################################
## installation ###############################################################
###############################################################################

# Install built executable, required dynamic libs, qm files and /gdal_data to
# binary directory, e.g. all except Qt stuff.

#...

###############################################################################
## packaging ##################################################################
###############################################################################

# Add to the installation path all including Qt dlls and /platform dir.

#...
