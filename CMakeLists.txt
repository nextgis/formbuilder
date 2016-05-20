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

#Note that setting the minimum required CMake version to 2.8.11 is required for 
#automatic linking to the qtmain.lib library on Windows.
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
project(fb)

# some init settings
set(CMAKE_COLOR_MAKEFILE ON)
# set path to additional CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# From docs:
# The easiest way to use CMake is to set the CMAKE_PREFIX_PATH environment variable to the install prefix of Qt 5.
# path example: <qt core dir>/qtbase
if(WIN32)
    set(QT_DIR_PREFIX_PATH "C:/Qt" CACHE PATH "This path will be set to CMAKE_PREFIX_PATH variable")
    set(CMAKE_PREFIX_PATH ${QT_DIR_PREFIX_PATH})
endif(WIN32)

include(GNUInstallDirs)

set(INSTALL_BIN_DIR ${CMAKE_INSTALL_BINDIR} CACHE INTERNAL "Installation directory for executables" FORCE)
set(INSTALL_LIB_DIR ${CMAKE_INSTALL_LIBDIR} CACHE INTERNAL "Installation directory for libraries" FORCE)
set(INSTALL_INC_DIR ${CMAKE_INSTALL_INCLUDEDIR} CACHE INTERNAL "Installation directory for headers" FORCE)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set( CMAKE_CXX_FLAGS "-fpic ${CMAKE_CXX_FLAGS}" )
    set( CMAKE_C_FLAGS   "-fpic ${CMAKE_C_FLAGS}" )  
endif()

set(EXTRA_LIBS)
if(NOT WIN32)
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

# path example: <qt core dir>/qtbase/lib/cmake/Qt5Core
find_package(Qt5Widgets REQUIRED) # finds its own dependencies (QtGui and QtCore)
# do not need to use if(Qt5Widgets_FOUND) because Qt will check it automatically
find_package(Qt5Network REQUIRED)
find_package(Qt5LinguistTools) # TODO: do we need it here?

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})
include_directories(${Qt5LinguistTools_INCLUDE_DIRS})

# TODO: need to check 2.1 version for external GDAL.
# For internal GDAL version is checked further.
find_anyproject(GDAL REQUIRED CMAKE_ARGS 
                    -DENABLE_PLSCENES=OFF
    -DENABLE_AAIGRID_GRASSASCIIGRID=OFF
    -DENABLE_ADRG_SRP=OFF
    -DENABLE_AIG=OFF
    -DENABLE_AIRSAR=OFF
    -DENABLE_ARG=OFF
    -DENABLE_BLX=OFF
    -DENABLE_BMP=OFF
    -DENABLE_BSB=OFF
    -DENABLE_CALS=OFF
    -DENABLE_CEOS=OFF
    -DENABLE_CEOS2=OFF
    -DENABLE_COASP=OFF
    -DENABLE_COSAR=OFF
    -DENABLE_CTG=OFF
    -DENABLE_DIMAP=OFF
    -DENABLE_DTED=OFF
    -DENABLE_E00GRID=OFF
    -DENABLE_ELAS=OFF
    -DENABLE_ENVISAT=OFF
    -DENABLE_ERS=OFF
    -DENABLE_FIT=OFF
    -DENABLE_GFF=OFF
    -DENABLE_GRIB=OFF
    -DENABLE_GSAG_GSBG_GS7BG=OFF
    -DENABLE_GXF=OFF
    -DENABLE_HF2=OFF
    -DENABLE_IDRISI_RASTER=OFF
    -DENABLE_ILWIS=OFF
    -DENABLE_INGR=OFF
    -DENABLE_IRIS=OFF
    -DENABLE_JAXAPALSAR=OFF
    -DENABLE_JDEM=OFF
    -DENABLE_KMLSUPEROVERLAY=OFF
    -DENABLE_L1B=OFF
    -DENABLE_LEVELLER=OFF
    -DENABLE_MAP=OFF
    -DENABLE_MBTILES=OFF
    -DENABLE_MSGN=OFF
    -DENABLE_NGSGEOID=OFF
    -DENABLE_NITF_RPFTOC_ECRGTOC=OFF
    -DENABLE_NWT=OFF
    -DENABLE_OZI=OFF
    -DENABLE_PDS_ISIS2_ISIS3_VICAR=OFF
    -DENABLE_PLMOSAIC=OFF
    -DENABLE_PNG=OFF
    -DENABLE_POSTGISRASTER=OFF
    -DENABLE_R=OFF
    -DENABLE_RASTERLITE=OFF
    -DENABLE_RIK=OFF
    -DENABLE_RMF=OFF
    -DENABLE_RS2=OFF
    -DENABLE_SAGA=OFF
    -DENABLE_SDTS_RASTER=OFF
    -DENABLE_SGI=OFF
    -DENABLE_SRTMHGT=OFF
    -DENABLE_TERRAGEN=OFF
    -DENABLE_TIL=OFF
    -DENABLE_TSX=OFF
    -DENABLE_USGSDEM=OFF
    -DENABLE_WCS=OFF
    -DENABLE_WMS=OFF
    -DENABLE_WMTS=OFF
    -DENABLE_XPM=OFF
    -DENABLE_XYZ=OFF
    -DENABLE_ZMAP=OFF
    -DENABLE_AERONAVFAA=OFF
    -DENABLE_ARCGEN=OFF
    -DENABLE_AVC=OFF
    -DENABLE_BNA=OFF
    -DENABLE_CARTODB=OFF
    -DENABLE_CLOUDANT=OFF
    -DENABLE_COUCHDB=OFF
    -DENABLE_CSV=OFF
    -DENABLE_CSW=OFF
    -DENABLE_DGN=OFF
    -DENABLE_DXF=OFF
    -DENABLE_EDIGEO=OFF
    -DENABLE_ELASTIC=OFF
    -DENABLE_GEOCONCEPT=OFF
    -DENABLE_GEORSS=OFF
    -DENABLE_GFT=OFF
    -DENABLE_GML=OFF
    -DENABLE_GMT=OFF
    -DENABLE_GPSBABEL=OFF
    -DENABLE_GPX=OFF
    -DENABLE_GTM=OFF
    -DENABLE_HTF=OFF
    -DENABLE_IDRISI_VECTOR=OFF
    -DENABLE_JML=OFF
    -DENABLE_NTF=OFF
    -DENABLE_ODS=OFF
    -DENABLE_OPENAIR=OFF
    -DENABLE_OPENFILEGDB=OFF
    -DENABLE_OSM=OFF
    -DENABLE_PDS_VECTOR=OFF
    -DENABLE_PG=OFF
    -DENABLE_PGDUMP=OFF
    -DENABLE_REC=OFF
    -DENABLE_S57=OFF
    -DENABLE_SDTS_VECTOR=OFF
    -DENABLE_SEGUKOOA=OFF
    -DENABLE_SEGY=OFF
    -DENABLE_SELAFIN=OFF
#    -DENABLE_SHAPE=OFF
    -DENABLE_SQLITE_GPKG=OFF
    -DENABLE_SUA=OFF
    -DENABLE_SVG=OFF
    -DENABLE_SXF=OFF
    -DENABLE_TIGER=OFF
    -DENABLE_VDV=OFF
    -DENABLE_VFK=OFF
    -DENABLE_WASP=OFF
    -DENABLE_WFS=OFF
    -DENABLE_XLSX=OFF
    -DGDAL_BUILD_APPS=OFF
    -DGDAL_BUILD_DOCS=OFF)
    
if(GDAL_FOUND AND NOT WITH_GDAL_EXTERNAL)
    message("GDAL version: " ${GDAL_VERSION})
    set(GDAL_REQUIRED_VERSION 2.1)
    if(${GDAL_VERSION} VERSION_LESS GDAL_REQUIRED_VERSION)
        message(FATAL_ERROR "GDAL must be of version 2.1 or higher!")
    endif()
endif()

# Build gui executable.
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Add to the installation path all including Qt dlls and /platform dir.

#...
