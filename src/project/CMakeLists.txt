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

set(LIB_NAME "fbproject")

set(LIB_HHEADERS
project_core.h
projects.h
)

set(LIB_CSOURCES
project_core.cpp
projectvoid.cpp
projectgdal.cpp
projectshp.cpp
projectngw.cpp
)

add_library(${LIB_NAME} OBJECT ${LIB_HHEADERS} ${LIB_CSOURCES})

set(FB_TARGET_OBJECTS ${FB_TARGET_OBJECTS} $<TARGET_OBJECTS:${LIB_NAME}> PARENT_SCOPE)


