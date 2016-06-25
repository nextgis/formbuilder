################################################################################
# Project:  CMake4GDAL
# Purpose:  CMake build scripts
# Author:   Dmitry Baryshnikov, polimax@mail.ru
################################################################################
# Copyright (C) 2015, NextGIS <info@nextgis.com>
# Copyright (C) 2012,2013,2014 Dmitry Baryshnikov
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
################################################################################


function(check_version include_dir major minor rev)

    # parse the version number from gdal_version.h and include in
    # major, minor and rev parameters

    file(READ ${include_dir}/gdal_version.h GDAL_VERSION_H_CONTENTS)

    string(REGEX MATCH "GDAL_VERSION_MAJOR[ \t]+([0-9]+)"
      GDAL_MAJOR_VERSION ${GDAL_VERSION_H_CONTENTS})
    string (REGEX MATCH "([0-9]+)"
      GDAL_MAJOR_VERSION ${GDAL_MAJOR_VERSION})
    string(REGEX MATCH "GDAL_VERSION_MINOR[ \t]+([0-9]+)"
      GDAL_MINOR_VERSION ${GDAL_VERSION_H_CONTENTS})
    string (REGEX MATCH "([0-9]+)"
      GDAL_MINOR_VERSION ${GDAL_MINOR_VERSION})
    string(REGEX MATCH "GDAL_VERSION_REV[ \t]+([0-9]+)"
      GDAL_REV_VERSION ${GDAL_VERSION_H_CONTENTS})
    string (REGEX MATCH "([0-9]+)"
      GDAL_REV_VERSION ${GDAL_REV_VERSION})

    set(${major} ${GDAL_MAJOR_VERSION} PARENT_SCOPE)
    set(${minor} ${GDAL_MINOR_VERSION} PARENT_SCOPE)
    set(${rev} ${GDAL_REV_VERSION} PARENT_SCOPE)

endfunction(check_version)

function(report_version name ver)

    string(ASCII 27 Esc)
    set(BoldYellow  "${Esc}[1;33m")
    set(ColourReset "${Esc}[m")
        
    message(STATUS "${BoldYellow}${name} version ${ver}${ColourReset}")
    
endfunction() 

function(warning_msg text)
    if(NOT SUPRESS_VERBOSE_OUTPUT)
    string(ASCII 27 Esc)
    set(Red         "${Esc}[31m")
    set(ColourReset "${Esc}[m")
        
    message(STATUS "${Red}${text}${ColourReset}")
    endif()
endfunction()    

