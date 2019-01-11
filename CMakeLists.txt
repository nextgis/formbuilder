####################################################################################################
#                                                                                                  #
#    Project:   NextGIS Formbuilder                                                                #
#    Authors:   Mikhail Gusev, gusevmihs@gmail.com                                                 #
#               Copyright (C) 2014-2018 NextGIS                                                    #
#                                                                                                  #
####################################################################################################

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR) #cmake_minimum_required(VERSION 3.9.0)

project(fb) #project(formbuilder)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
include(fb_util)

# An option to build package for NextGIS online installer.
set(BUILD_NEXTGIS_PACKAGE OFF CACHE BOOL "Build package for NextGIS Installation system (based on Borsch) instead of simple build")

# TODO: add an option to build offline installer.

# TODO: add an option to build online/offline documentation.

# TODO: add an option to build tests.

# Include root dir.
include_directories(src)

# Build libraries and desktop application.
add_subdirectory(src)
