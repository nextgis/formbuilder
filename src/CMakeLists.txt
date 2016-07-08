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

set(APP_NAME fb)

# Note: to correctly build for Win XP pass flags to the command line.

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# TODO: make 3 separate libraries in future: forms, projects and screens.

set(APP_HEADERS

form/form_core.h
form/attributes.h
form/elements.h
form/factories.h

project/project_core.h
project/projects.h

screen/screen_core.h
screen/screens.h
screen/devices.h
)

set(APP_SOURCES

form/dialogcsv.cpp
form/dialoglistvalues.cpp
form/dialogngwlookup.cpp
form/dialogdlistvalues.cpp
form/dialogdatetime.cpp
form/form_core.cpp
form/attributes.cpp
form/elements.cpp

project/project_core.cpp
project/projectvoid.cpp
project/projectgdal.cpp
project/projectshp.cpp
project/projectngw.cpp

screen/screen_core.cpp
screen/screencommon.cpp
screen/screenmobile.cpp
screen/screenandroid.cpp
screen/screenios.cpp
screen/screenweb.cpp
screen/screenqgis.cpp

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
    message(WARNING "No Qt5LinguistTools found. All languages except english will be inavailable!")
endif()

if(WIN32)
    add_executable(${APP_NAME} WIN32 ${APP_SOURCES} ${APP_HEADERS} ${FB_TARGET_OBJECTS} ${RESOURCES} ${UI_HDRS} ${QM_FILES})
else()
    add_executable(${APP_NAME} ${APP_SOURCES} ${APP_HEADERS} ${FB_TARGET_OBJECTS} ${RESOURCES} ${UI_HDRS} ${QM_FILES})
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
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        install(FILES ${QT_DIR_PREFIX_PATH}/bin/Qt5Cored.dll DESTINATION ${INSTALL_BIN_DIR})
        install(FILES ${QT_DIR_PREFIX_PATH}/bin/Qt5Guid.dll DESTINATION ${INSTALL_BIN_DIR})
        install(FILES ${QT_DIR_PREFIX_PATH}/bin/Qt5Widgetsd.dll DESTINATION ${INSTALL_BIN_DIR})
        install(FILES ${QT_DIR_PREFIX_PATH}/bin/Qt5Networkd.dll DESTINATION ${INSTALL_BIN_DIR})
        install(FILES ${QT_DIR_PREFIX_PATH}/plugins/platforms/qwindowsd.dll DESTINATION ${INSTALL_BIN_DIR}/platforms)
    else()
        install(FILES ${QT_DIR_PREFIX_PATH}/bin/Qt5Core.dll DESTINATION ${INSTALL_BIN_DIR})
        install(FILES ${QT_DIR_PREFIX_PATH}/bin/Qt5Gui.dll DESTINATION ${INSTALL_BIN_DIR})
        install(FILES ${QT_DIR_PREFIX_PATH}/bin/Qt5Widgets.dll DESTINATION ${INSTALL_BIN_DIR})
        install(FILES ${QT_DIR_PREFIX_PATH}/bin/Qt5Network.dll DESTINATION ${INSTALL_BIN_DIR})
        install(FILES ${QT_DIR_PREFIX_PATH}/plugins/platforms/qwindows.dll DESTINATION ${INSTALL_BIN_DIR}/platforms)
    endif()
    # Install QM files.
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/fb_ru_RU.qm DESTINATION ${INSTALL_SHARE_DIR}/translations)
    install(FILES ${QT_DIR_PREFIX_PATH}/translations/qt_ru.qm DESTINATION ${INSTALL_SHARE_DIR}/translations)
    # Note: we regard that gdal is correctly installed if there is an internal windows build - so here is no gdal data and dlls copying.
elseif(UNIX)
    # Install program's QM file.
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/fb_ru_RU.qm DESTINATION ${INSTALL_SHARE_DIR}/translations)
endif()

################################### Packaging: ##############################################

set(FB_VERSION "2.1") # TODO: get FB version from sources.

set(FB_CPACK_PACKAGE_SETUPNAME "formbuilder")
set(FB_CPACK_PACKAGE_NAME "NextGIS Formbuilder")
set(FB_CPACK_PACKAGE_SHORTNAME "Formbuilder")
set(FB_CPACK_VENDOR "NextGIS")

#set(CPACK_PACKAGE_FILE_NAME "${FB_CPACK_PACKAGE_SETUPNAME}-${FB_VERSION}-${CPACK_SYSTEM_NAME}-setup")
set(CPACK_PACKAGE_NAME "${FB_CPACK_PACKAGE_SHORTNAME}")
set(CPACK_PACKAGE_VENDOR "${FB_CPACK_VENDOR}")
set(CPACK_PACKAGE_VERSION "${FB_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${FB_CPACK_PACKAGE_NAME} Installation")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${FB_CPACK_VENDOR}/${FB_CPACK_PACKAGE_SHORTNAME}")
#set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${}-${}-${}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_RELOCATABLE TRUE)
#set(CPACK_PACKAGE_ICON ) # http://stackoverflow.com/a/28768495

if(WIN32)
    set(CPACK_GENERATOR "NSIS") #set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_MONOLITHIC_INSTALL ON)
    set(CPACK_NSIS_DISPLAY_NAME "${FB_CPACK_PACKAGE_NAME}")
    #set(CPACK_NSIS_COMPONENT_INSTALL ON)
    #set(CPACK_NSIS_CONTACT "${}")
    set(CPACK_NSIS_PACKAGE_NAME "${FB_CPACK_PACKAGE_NAME}")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    string(REPLACE "/" "\\\\" NSIS_INSTALL_SHARE_DIR "${INSTALL_SHARE_DIR}")
    string(REPLACE "/" "\\\\" NSIS_INSTALL_BIN_DIR "${INSTALL_BIN_DIR}")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "${FB_CPACK_VENDOR}\\\\${FB_CPACK_PACKAGE_SHORTNAME}") # to replace '/' with '\'

    # Create shortcuts.
    # Need SetOutPath - otherwise no translations loaded when run the shortcut
    # because working directory is set incorrectly.
    set(CPACK_PACKAGE_EXECUTABLES "${APP_NAME};${FB_CPACK_PACKAGE_NAME}")   
    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
            SetOutPath \\\"$INSTDIR\\\\bin\\\"
            CreateShortCut \\\"$DESKTOP\\\\${FB_CPACK_PACKAGE_NAME}.lnk\\\" \\\"$INSTDIR\\\\bin\\\\${APP_NAME}.exe\\\"
        ")
    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
            Delete \\\"$DESKTOP\\\\${FB_CPACK_PACKAGE_NAME}.lnk\\\"
        ")

#else ()
#    set (CPACK_PROJECT_CONFIG_FILE ${CMAKE_SOURCE_DIR}/cmake/CPackConfig.cmake)
#    set (CPACK_GENERATOR "DEB;RPM;TGZ;ZIP")
#    set (CPACK_COMPONENTS_ALL applications libraries headers documents)
#    set (CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)
#    set (CPACK_DEBIAN_COMPONENT_INSTALL ON)
#    set (CPACK_DEBIAN_PACKAGE_SECTION "Libraries")
#    set (CPACK_DEBIAN_PACKAGE_MAINTAINER "${PACKAGE_BUGREPORT}")
#    set (CPACK_DEBIAN_PRE_INSTALL_SCRIPT_FILE "/sbin/ldconfig")
#    set (CPACK_DEBIAN_PRE_UNINSTALL_SCRIPT_FILE "/sbin/ldconfig")
#    set (CPACK_DEBIAN_POST_INSTALL_SCRIPT_FILE "/sbin/ldconfig")
#    set (CPACK_DEBIAN_POST_UNINSTALL_SCRIPT_FILE "/sbin/ldconfig")
#    set (CPACK_DEBIAN_PACKAGE_DEPENDS "zlib1g, libjpeg, libgeos, libcurl4-gnutls | libcurl-ssl, libexpat1, libproj, libxml2, liblzma, libarmadillo, libtiff5, libgeotiff, libjson-c, libsqlite3, python2.7, python-numpy, libpcre3, libspatialite, libpq")
#    set (CPACK_RPM_COMPONENT_INSTALL ON)
#    set (CPACK_RPM_PACKAGE_GROUP "Development/Tools")
#    set (CPACK_RPM_PACKAGE_LICENSE "X/MIT")
#    set (CPACK_RPM_PACKAGE_URL "${PACKAGE_URL}")
#    set (CPACK_RPM_PRE_INSTALL_SCRIPT_FILE "/sbin/ldconfig")
#    set (CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE "/sbin/ldconfig")
#    set (CPACK_RPM_POST_INSTALL_SCRIPT_FILE "/sbin/ldconfig")
#    set (CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE "/sbin/ldconfig")
endif ()

include(InstallRequiredSystemLibraries)

include(CPack)
