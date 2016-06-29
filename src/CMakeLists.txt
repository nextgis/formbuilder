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

# To correctly build for Win XP?
###set(CMAKE_EXE_LINKER_FLAGS "/SUBSYSTEM:WINDOWS,5.01")
#if(WIN32)
#  add_definitions(-D_WIN32_WINNT=0x0501)
#endif(WIN32)
if(CMAKE_GENERATOR_TOOLSET MATCHES "v([0-9]+)_xp")
     add_definitions(-D_WIN32_WINNT=0x0501)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/form)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/project)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/screen)

set(APP_NAME fb)

set(APP_SOURCES
jsoncpp.cpp
ngw.cpp
dialognewvoid.cpp
dialogngwproject.cpp
dialogfieldmanager.cpp
appicon.rc
registrar.cpp
fb.cpp
main.cpp
)

qt5_wrap_ui(fb_ui fb.ui)
set(UI_HDRS ${fb_ui})

qt5_add_resources(fb_res res.qrc)
set(RESOURCES ${fb_res})

# TODO: make automatic updating of *.ts files via qt5_create_translation()
# Use lupdate utility directly for now.
if(Qt5LinguistTools_FOUND)
    set(TS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/translations/fb_ru_RU.ts)
    qt5_add_translation(QM_FILES ${TS_FILES}) # no lupdate calling.
else()
    message(WARNING "No Qt5LinguistTools found. All languages except english are inavailable!")
endif()

if(WIN32)
    add_executable(${APP_NAME} WIN32 ${APP_SOURCES} ${FB_TARGET_OBJECTS} ${RESOURCES} ${UI_HDRS} ${QM_FILES})
else()
    add_executable(${APP_NAME} ${APP_SOURCES} ${FB_TARGET_OBJECTS} ${RESOURCES} ${UI_HDRS} ${QM_FILES})
endif()

target_link_libraries(${APP_NAME} Qt5::Widgets Qt5::Network)# Qt5::LinguistTools)
if(UNIX)
    target_link_libraries(${APP_NAME} ${EXTRA_LIBS})
endif()
target_link_extlibraries(${APP_NAME}) 

install(TARGETS ${APP_NAME} 
        RUNTIME DESTINATION ${INSTALL_BIN_DIR} 
        ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${INSTALL_LIB_DIR})
if(WIN32)
    # Install Qt dlls (do it anyway because Qt is yet not supported in external build system).
    install(FILES ${QT_DIR_PREFIX_PATH}/bin/Qt5Core.dll DESTINATION ${INSTALL_BIN_DIR})
    install(FILES ${QT_DIR_PREFIX_PATH}/bin/Qt5Gui.dll DESTINATION ${INSTALL_BIN_DIR})
    install(FILES ${QT_DIR_PREFIX_PATH}/bin/Qt5Widgets.dll DESTINATION ${INSTALL_BIN_DIR})
    install(FILES ${QT_DIR_PREFIX_PATH}/bin/Qt5Network.dll DESTINATION ${INSTALL_BIN_DIR})
    install(FILES ${QT_DIR_PREFIX_PATH}/plugins/platforms/qwindows.dll DESTINATION ${INSTALL_BIN_DIR}/platforms)
    # Install QM files.
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/fb_ru_RU.qm DESTINATION ${INSTALL_SHARE_DIR}/translations)
    install(FILES ${QT_DIR_PREFIX_PATH}/translations/qt_ru.qm DESTINATION ${INSTALL_SHARE_DIR}/translations)
    # Note: we regard that gdal is correctly installed if there is an internal windows build - so here is no gdal data and dlls copying.
elseif(UNIX)
    # Install program's QM file.
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/fb_ru_RU.qm DESTINATION ${INSTALL_SHARE_DIR}/translations)
endif()
include(InstallRequiredSystemLibraries)
