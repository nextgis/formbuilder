#***************************************************************************
# Project:  NextGIS Formbuilder
# Purpose:  main cmake script
# Author:  Mikhail Gusev, gusevmihs@gmail.com
# Author:   Dmitry Baryshnikov, dmitry.baryshnikov@nexgis.com
#***************************************************************************
#   Copyright (C) 2014-2018 NextGIS
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

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(fb)

set(PACKAGE_VENDOR NextGIS)
set(PACKAGE_BUGREPORT info@nextgis.com)

# Some init settings
set(CMAKE_COLOR_MAKEFILE ON)
# Set path to additional CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

if(OSX_FRAMEWORK)
    set(INSTALL_BIN_DIR "Applications" CACHE INTERNAL "Installation directory for executables" FORCE)
    set(INSTALL_LIB_DIR "${INSTALL_BIN_DIR}/${PROJECT_NAME}.app/Contents/Frameworks" CACHE INTERNAL "Installation directory for libraries" FORCE)
    set(INSTALL_SHARE_DIR "${INSTALL_BIN_DIR}/${PROJECT_NAME}.app/Contents/Resources" CACHE INTERNAL "Installation directory for shared files" FORCE)
    set(INSTALL_TRANSLATIONS_DIR "${INSTALL_BIN_DIR}/${PROJECT_NAME}.app/Contents/Resources/translations" CACHE INTERNAL "Installation directory for translations" FORCE)
    set(QT5_TRANSLATION_DIR "Library/Translations/Qt5")

    set(SKIP_INSTALL_HEADERS ON)
    set(SKIP_INSTALL_EXECUTABLES OFF)
    set(SKIP_INSTALL_FILES OFF)
    set(SKIP_INSTALL_EXPORT ON)
    set(CMAKE_MACOSX_RPATH ON)

    set(QT4_PLUGINS_DIR Library/Plugins/Qt4)
    set(QT4_TRANSLATION_DIR Library/Translations/Qt4)
    set(QT5_PLUGINS_DIR Library/Plugins/Qt5)
    set(QT5_TRANSLATION_DIR Library/Translations/Qt5)
else()
    include(GNUInstallDirs)

    set(INSTALL_BIN_DIR ${CMAKE_INSTALL_BINDIR} CACHE INTERNAL "Installation directory for executables" FORCE)
    set(INSTALL_LIB_DIR ${CMAKE_INSTALL_LIBDIR} CACHE INTERNAL "Installation directory for libraries" FORCE)
    set(INSTALL_SHARE_DIR "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}" CACHE INTERNAL "Installation directory for share data" FORCE)
    set(INSTALL_TRANSLATIONS_DIR "${CMAKE_INSTALL_DATADIR}/translations" CACHE INTERNAL "Installation directory for translations" FORCE)

    # On Windows qt.conf will be common for several applications so we need to have paths for qt4/5
    set(QT4_PLUGINS_DIR FIXME)
    set(QT4_TRANSLATION_DIR FIXME)
    set(QT5_PLUGINS_DIR FIXME)
    set(QT5_TRANSLATION_DIR FIXME)
endif()

set(QT_PREFIX ${CMAKE_INSTALL_PREFIX})
if(POLICY CMP0071)
    cmake_policy(SET CMP0071 NEW)
endif()

# Tell CMake to run moc when necessary:
# FIXME: Not working on Qt <= 5.5 and Ubuntu [trusty, xenial]
# set(CMAKE_AUTOMOC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(util)
check_version(FB_MAJOR_VERSION FB_MINOR_VERSION)
set(VERSION ${FB_MAJOR_VERSION}.${FB_MINOR_VERSION})

if(OSX_FRAMEWORK)
    set(FRAMEWORK_VERSION ${FB_MAJOR_VERSION})
endif()

report_version(${PROJECT_NAME} ${VERSION})

include(FindAnyProject)
option(BUILD_SHARED_LIBS "Build external libraries shared [Default: ON]" ON)

if(UNIX)
    find_package(Threads)
    set(TARGET_LINK_LIB ${TARGET_LINK_LIB} ${CMAKE_THREAD_LIBS_INIT})
    find_library(DL_LIB dl)
    set(TARGET_LINK_LIB ${TARGET_LINK_LIB} ${DL_LIB})
    find_library(M_LIB m)
    set(TARGET_LINK_LIB ${TARGET_LINK_LIB} ${M_LIB})
endif()

if(OSX_FRAMEWORK)
    set(TARGET_LINK_LIB ${TARGET_LINK_LIB} "-framework CoreFoundation -framework CoreServices -framework ApplicationServices")
endif()

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set(QT5_REQUIRED_COMPONENTS Widgets Network Svg LinguistTools)
if(APPLE)
    set(QT5_REQUIRED_COMPONENTS ${QT5_REQUIRED_COMPONENTS} MacExtras)
endif()

find_anyproject(Qt5 REQUIRED COMPONENTS ${QT5_REQUIRED_COMPONENTS})
find_anyproject(GDAL REQUIRED VERSION 2.1
    CMAKE_ARGS
    -DSKIP_DEFAULTS=ON
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
    -DENABLE_PRF=OFF
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
    -DENABLE_GIF=OFF
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
    -DENABLE_CARTO=OFF
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
    -DENABLE_CAD=OFF
    -DGDAL_BUILD_APPS=OFF
    -DGDAL_BUILD_DOCS=OFF
)

get_target_property(RCC_PATH Qt5::rcc IMPORTED_LOCATION)
execute_process(COMMAND ${RCC_PATH} -v RESULT_VARIABLE ${IS_RCC_WORKING})
if(NOT IS_RCC_WORKING EQUAL 0)
    if(WITH_ZLIB_EXTERNAL)
        # ZLIB needed for uic, rcc, lrelease and lupdate
        find_anyproject(ZLIB REQUIRED)
        get_target_property(ZLIB_IMPORTED_LOCATION ${ZLIB_LIBRARIES} IMPORTED_LOCATION_RELEASE)
        get_target_property(RCC_PATH Qt5::rcc IMPORTED_LOCATION)
        get_filename_component(QT_BIN_DIR ${RCC_PATH} PATH)
        if(WIN32)
            get_filename_component(PATH_DIR ${ZLIB_IMPORTED_LOCATION} PATH)
            set(ENV{PATH} "$ENV{PATH};${PATH_DIR}")
            file(COPY ${ZLIB_IMPORTED_LOCATION} DESTINATION ${QT_BIN_DIR})
        elseif(OSX_FRAMEWORK)
            get_filename_component(PATH_DIR ${ZLIB_IMPORTED_LOCATION} PATH)
            get_filename_component(PATH_DIR ${PATH_DIR} PATH)
            get_filename_component(PATH_DIR ${PATH_DIR} PATH)
            file(COPY ${PATH_DIR} DESTINATION ${QT_BIN_DIR}/Frameworks)
        endif()
    endif()
endif()

find_anyproject(NGSTD REQUIRED COMPONENTS framework5)

# Build gui executable.
add_subdirectory(src)

configure_file(${PROJECT_SOURCE_DIR}/cmake/qt.conf.cmake.in ${CMAKE_BINARY_DIR}/qt.conf IMMEDIATE @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/qt.conf DESTINATION ${INSTALL_SHARE_DIR})
