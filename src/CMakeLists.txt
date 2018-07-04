#***************************************************************************
# Project:  NextGIS Formbuilder
# Purpose:  cmake script
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

add_subdirectory(translations)

set(APP_NAME fb)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# FIXME: How to add client_id to launchpad build?
# # Add fatal message so not to forget: add OAuth2 client id for this apllication.
# if (DEFINED FB_OAUTH2_CLIENT_ID)
#     add_definitions(-D_OAUTH2_CLIENT_ID=${FB_OAUTH2_CLIENT_ID})
# else()
#     #add_definitions(-D_OAUTH2_CLIENT_ID="")
#     message(FATAL_ERROR "OAuth2 client id (in the FB_OAUTH2_CLIENT_ID variable) should be defined in order to build the application")
# endif ()

configure_file(${PROJECT_SOURCE_DIR}/cmake/formbuilder.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/formbuilder.desktop @ONLY)

set(APP_HEADERS
    gui/gui_globals.h
    gui/inputtablewidget.h
    gui/csvcolumnsdialog.h
    gui/itemsdialog.h

    form/form_core.h
    form/attributes.h
    form/elements.h
    form/factories.h

    project/project_core.h
    project/projects.h

    screen/screen_core.h
    screen/screens.h
    screen/devices.h

    fbclickablelabel.h
    fbupdater.h
    fb.h
    ngw.h
)

set(APP_SOURCES
    gui/gui_globals.cpp
    gui/inputtablewidget.cpp
    gui/csvcolumnsdialog.cpp
    gui/itemsdialog.cpp

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

    fbclickablelabel.cpp

    jsoncpp.cpp
    ngw.cpp
    dialognewvoid.cpp
    dialogngwproject.cpp
    dialogngwlayer.cpp
    dialogfieldmanager.cpp
    dialoglists.cpp
    appicon.rc
    registrar.cpp
    fb.cpp
    main.cpp
    fbupdater.cpp
)

qt5_wrap_cpp(MOC_SRCS ${APP_HEADERS})
qt5_wrap_ui(UI_HDRS fb.ui)
qt5_add_resources(RESOURCES res.qrc)

set(TRANSLATIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/fb_ru_RU.ts
)

unset(QM_FILES)
foreach(TRANSLATION ${TRANSLATIONS})
    if(EXISTS ${TRANSLATION})
        qt5_add_translation(QM_FILES ${TRANSLATION})
    endif()
endforeach()

add_executable(${APP_NAME} MACOSX_BUNDLE WIN32 ${APP_SOURCES} ${APP_HEADERS} ${MOC_SRCS} ${FB_TARGET_OBJECTS} ${RESOURCES} ${UI_HDRS} ${QM_FILES})
set_property(TARGET ${APP_NAME} PROPERTY CXX_STANDARD 11)

if(WIN32)
    add_definitions(-DFB_GDALDATA_IN_SHARE) # This is define path to gdal data as ../share/gdal
endif()

if(WIN32)
    set_property(TARGET ${APP_NAME} APPEND PROPERTY LINK_FLAGS /entry:mainCRTStartup)
endif()

target_link_extlibraries(${APP_NAME})

if(OSX_FRAMEWORK)
    set_target_properties(${APP_NAME} PROPERTIES
        MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} Version ${VERSION}, Copyright 2011-2018 NextGIS."
        # MACOSX_BUNDLE_ICON_FILE ${ICON_FILE_NAME}
        MACOSX_BUNDLE_GUI_IDENTIFIER "${PROJECT_NAME}"
        MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} Version ${VERSION}"
        MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION}
        MACOSX_BUNDLE_BUNDLE_VERSION ${FRAMEWORK_VERSION}
        MACOSX_BUNDLE_COPYRIGHT "Copyright 2011-2018, NextGIS. All Rights Reserved."
        MACOSX_BUNDLE_INFO_PLIST "${PROJECT_SOURCE_DIR}/cmake/MacOSXBundleInfo.plist.in"
        INSTALL_RPATH "@executable_path/../../../../Library/Frameworks;@executable_path/../Frameworks"
    )
endif()

# install ======================================================================

if(NOT SKIP_INSTALL_LIBRARIES AND NOT SKIP_INSTALL_ALL)
    install(TARGETS ${APP_NAME}
        BUNDLE DESTINATION ${INSTALL_BIN_DIR}
        RUNTIME DESTINATION ${INSTALL_BIN_DIR}
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
        FRAMEWORK DESTINATION ${INSTALL_LIB_DIR}
        PUBLIC_HEADER DESTINATION ${INSTALL_INC_DIR} COMPONENT headers
    )
endif()

if(NOT SKIP_INSTALL_FILES AND NOT SKIP_INSTALL_ALL)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/fb_ru_RU.qm DESTINATION ${INSTALL_TRANSLATIONS_DIR})
endif()

if(UNIX AND NOT OSX_FRAMEWORK)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/img/formbuilder.png DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/formbuilder.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
endif()

if(OSX_FRAMEWORK)
    file(GLOB ICONS "img/*.icns")
    install(FILES ${ICONS} DESTINATION ${INSTALL_SHARE_DIR})
endif()

# Archiving ====================================================================

set(CPACK_PACKAGE_NAME "${PACKAGE_NAME}")
set(CPACK_PACKAGE_VENDOR "${PACKAGE_VENDOR}")
set(CPACK_PACKAGE_VERSION "${VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PACKAGE_NAME} Installation")
set(CPACK_PACKAGE_RELOCATABLE TRUE)
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_GENERATOR "ZIP")
set(CPACK_MONOLITHIC_INSTALL ON)
set(CPACK_STRIP_FILES TRUE)

# Get cpack zip archive name
get_cpack_filename(${VERSION} PROJECT_CPACK_FILENAME)
set(CPACK_PACKAGE_FILE_NAME ${PROJECT_CPACK_FILENAME})

include(CPack)
