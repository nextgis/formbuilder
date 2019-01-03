#***************************************************************************
# Project:  NextGIS framework library
# Purpose:  cmake script
# Author:   Dmitry Baryshnikov, bishop.dev@gmail.com
#***************************************************************************
#   Copyright (C) 2015-2016 NextGIS, info@nextgis.ru
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

set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)

set(TRANSLATION_MESSAGES_FULL)
set(TRANSLATIONS
    fb_ru_RU.ts
)

file(GLOB_RECURSE TRANSLATION_CPP ${PROJECT_SOURCE_DIR}/src/${NGLIB}/*.cpp)
file(GLOB_RECURSE TRANSLATION_H ${PROJECT_SOURCE_DIR}/src/${NGLIB}/*.h)
file(GLOB_RECURSE TRANSLATION_UI ${PROJECT_SOURCE_DIR}/src/${NGLIB}/*.ui)
set(FILES_TO_TRANSLATE ${TRANSLATION_H} ${TRANSLATION_CPP} ${TRANSLATION_UI})

qt5_create_translation(TRANSLATION_MESSAGES ${FILES_TO_TRANSLATE} ${TRANSLATIONS})
set(TRANSLATION_MESSAGES_FULL ${TRANSLATION_MESSAGES_FULL} ${TRANSLATION_MESSAGES})

add_custom_target(fb_translations_update DEPENDS ${TRANSLATION_MESSAGES_FULL})
