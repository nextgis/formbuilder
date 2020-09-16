####################################################################################################
#                                                                                                  #
#    Project:   NextGIS Formbuilder                                                                #
#    Authors:   Mikhail Gusev, gusevmihs@gmail.com                                                 #
#               Copyright (C) 2014-2019 NextGIS                                                    #
#                                                                                                  #
####################################################################################################

set(FB_APP_NAME fb)

if(BUILD_NEXTGIS_PACKAGE)

	# FIXME: How to add client_id to launchpad build?
	# # Add fatal message so not to forget: add OAuth2 client id for this apllication.
	# if (DEFINED FB_OAUTH2_CLIENT_ID)
	#     add_definitions(-D_OAUTH2_CLIENT_ID=${FB_OAUTH2_CLIENT_ID})
	# else()
	#     #add_definitions(-D_OAUTH2_CLIENT_ID="")
	#     message(FATAL_ERROR "OAuth2 client id (in the FB_OAUTH2_CLIENT_ID variable) should be defined in order to build the application")
	# endif ()

	configure_file(${PROJECT_SOURCE_DIR}/cmake/formbuilder.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/formbuilder.desktop @ONLY)

endif()

set(FB_SOURCES

    app_icon.rc

    fb_project.cpp
    fb_updater.cpp
    fb_aboutdialog.cpp
    fb_authdialog.cpp
    fb_window.cpp

    main.cpp
)

set(FB_HEADERS

    fb_project.h
    fb_updater.h
    fb_aboutdialog.h
    fb_authdialog.h
    fb_window.h

    languages.h
)

set(FB_UIS

    fb_aboutdialog.ui
    fb_authdialog.ui
)

set(FB_RESOURCES

    data/data.qrc
)
qt5_add_resources(FB_QRCS ${FB_RESOURCES})


file(GLOB_RECURSE FB_H_TO_TRANSLATE ../*.h)
file(GLOB_RECURSE FB_CPP_TO_TRANSLATE ../*.cpp)
file(GLOB_RECURSE FB_UI_TO_TRANSLATE ../*.ui)
set(FB_TSS
    translations/fb_ru_RU.ts
)
qt5_create_translation(FB_QMS
                       ${FB_CPP_TO_TRANSLATE} ${FB_H_TO_TRANSLATE} ${FB_UI_TO_TRANSLATE}
                       ${FB_TSS})
qt5_add_translation(FB_QMS ${FB_TSS})
add_custom_target(translations ALL DEPENDS ${FB_QMS}) # in order to track updating of *.ts files


add_executable(${FB_APP_NAME} MACOSX_BUNDLE WIN32 ${FB_SOURCES} ${FB_HEADERS} ${FB_UIS} ${FB_QRCS} ${FB_QMS})	


if(BUILD_NEXTGIS_PACKAGE)

	set(TARGET_LINK_LIB ${TARGET_LINK_LIB}
		${FB_CORE_LIB_NAME}
		${FB_GUI_LIB_NAME}
		${FB_MOCKUP_LIB_NAME}
		${FB_SERIALIZER_LIB_NAME}
		${FB_UTIL_LIB_NAME})

#	set_property(TARGET ${FB_APP_NAME} PROPERTY CXX_STANDARD 11)

    if(WIN32)
        add_definitions(-DFB_GDALDATA_IN_SHARE)
    endif()

	if(WIN32)
		set_property(TARGET ${FB_APP_NAME} APPEND PROPERTY LINK_FLAGS /entry:mainCRTStartup)
	endif()

	target_link_extlibraries(${FB_APP_NAME})

	if(OSX_FRAMEWORK)
		set_target_properties(${FB_APP_NAME} PROPERTIES
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
	
	# Installing:

	if(NOT SKIP_INSTALL_LIBRARIES AND NOT SKIP_INSTALL_ALL)
		install(TARGETS ${FB_APP_NAME}
			BUNDLE DESTINATION ${INSTALL_BIN_DIR}
			RUNTIME DESTINATION ${INSTALL_BIN_DIR}
			LIBRARY DESTINATION ${INSTALL_LIB_DIR}
			ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
			FRAMEWORK DESTINATION ${INSTALL_LIB_DIR}
#			PUBLIC_HEADER DESTINATION ${INSTALL_INC_DIR} COMPONENT headers
		)
	endif()

	if(NOT SKIP_INSTALL_FILES AND NOT SKIP_INSTALL_ALL)
		install(FILES ${CMAKE_CURRENT_BINARY_DIR}/fb_ru_RU.qm DESTINATION ${INSTALL_TRANSLATIONS_DIR})
	endif()

	if(UNIX AND NOT OSX_FRAMEWORK)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/formbuilder.png DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps)
		install(FILES ${CMAKE_CURRENT_BINARY_DIR}/formbuilder.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
	endif()

	if(OSX_FRAMEWORK)
        file(GLOB ICONS "data/*.icns")
		install(FILES ${ICONS} DESTINATION ${INSTALL_SHARE_DIR})
	endif()

	# Archiving:

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

else()

    target_link_libraries(${FB_APP_NAME}
                          Qt5::Widgets
                          ${GDAL_LIBRARY}
                          ${FB_CORE_LIB_NAME} ${FB_GUI_LIB_NAME} ${FB_MOCKUP_LIB_NAME}
                          ${FB_SERIALIZER_LIB_NAME} ${FB_UTIL_LIB_NAME}
                          ${NGSTD_LIBRARIES})

    # TODO: for Windows also add copying of the following files:
    # <bin folder>/Qt5Svg.dll # though the app is not linked directly with this lib
    # <bin folder>/styles/qwindowsvistastyle.dll    # for Qt >= 5.12.0 (on lesser versions qwindowsvistastyle is applied automatically)
    # <bin folder>/: msvcr100.dll, msvcr120.dll, msvcp140.dll (for 32bit build dlls must be 32 bit from C:\Windows\SysWOW64, not 64 bit from C:\Windows\System32)
    # ? <bin folder>/libEGL.dll & <bin folder>/libGLESv2.dll
    # ? <bin folder>/platforms/qwindows.dll
    # ? <bin folder>/imageformats/<qgif,qico,qsvg>.dll
    # ? <bin folder>/iconengines/qsvgicon.dll
	
endif()	

include(CPack)



