####################################################################################################
#                                                                                                  #
#    Project:   NextGIS Formbuilder                                                                #
#    Authors:   Mikhail Gusev, gusevmihs@gmail.com                                                 #
#               Copyright (C) 2014-2019 NextGIS                                                    #
#                                                                                                  #
####################################################################################################

set(FB_GUI_LIB_SOURCES

    attrwatchers/boolean_watcher.cpp
    attrwatchers/number_watcher.cpp
    attrwatchers/string_watcher.cpp
    attrwatchers/enum_watcher.cpp
    attrwatchers/globaltextenum_watcher.cpp
    attrwatchers/datetime_watcher.cpp
    attrwatchers/doubleitems_watcher.cpp
    attrwatchers/tripleitems_watcher.cpp
    attrwatchers/depdoubleitems_watcher.cpp
    attrwatchers/pagelist_watcher.cpp

    dialogs/newvoidprojectdialog.cpp
    dialogs/layerdialog.cpp
    dialogs/layerdialog2.cpp
    dialogs/csvcolumnsdialog.cpp
    dialogs/doubleitemsdialog.cpp
    dialogs/depdoubleitemsdialog.cpp
    dialogs/tripleitemsdialog.cpp
    dialogs/tabpagesdialog.cpp
    dialogs/ngwdialog.cpp
    dialogs/prefixdialog.cpp

    attrwatcher.cpp
    attrwatcher_registrar.cpp
    commontablewidget.cpp
    itemswidget.cpp
    elemtreewidget.cpp
    propstablewidget.cpp
    fieldstablewidget.cpp
    fieldstablewidget2.cpp

    custom_dialog.cpp
    custom_spinbox.cpp
    custom_scrollarea.cpp
    mainwindow.cpp
)

set(FB_GUI_LIB_HEADERS

    attrwatchers/boolean_watcher.h
    attrwatchers/number_watcher.h
    attrwatchers/string_watcher.h
    attrwatchers/enum_watcher.h
    attrwatchers/globaltextenum_watcher.h
    attrwatchers/datetime_watcher.h
    attrwatchers/doubleitems_watcher.h
    attrwatchers/tripleitems_watcher.h
    attrwatchers/depdoubleitems_watcher.h
    attrwatchers/pagelist_watcher.h

    dialogs/newvoidprojectdialog.h
    dialogs/layerdialog.h
    dialogs/layerdialog2.h
    dialogs/csvcolumnsdialog.h
    dialogs/doubleitemsdialog.h
    dialogs/depdoubleitemsdialog.h
    dialogs/tripleitemsdialog.h
    dialogs/tabpagesdialog.h
    dialogs/ngwdialog.h
    dialogs/prefixdialog.h

    attrwatcher.h
    attrwatcher_registrar.h
    commontablewidget.h
    itemswidget.h
    elemtreewidget.h
    propstablewidget.h
    fieldstablewidget.h
    fieldstablewidget2.h

    custom_dialog.h
    custom_spinbox.h
    custom_scrollarea.h
    mainwindow.h

    gui_common.h
    color_themes.h
    sizes.h

    all_registry.h
)

set(FB_GUI_LIB_PUBLIC_HEADERS

)

set(FB_GUI_LIB_UIS

    dialogs/newvoidprojectdialog.ui
    dialogs/ngwdialog.ui
)

set(FB_GUI_LIB_RESOURCES

    styles/styles.qrc
    images/images.qrc
)
qt5_add_resources(FB_GUI_LIB_QRCS ${FB_GUI_LIB_RESOURCES})


add_library(${FB_GUI_LIB_NAME} STATIC ${FB_GUI_LIB_SOURCES} ${FB_GUI_LIB_HEADERS}
            ${FB_GUI_LIB_UIS} ${FB_GUI_LIB_QRCS})

if(BUILD_NEXTGIS_PACKAGE)
	set(TARGET_LINK_LIB ${TARGET_LINK_LIB} ${FB_CORE_LIB_NAME} ${FB_UTIL_LIB_NAME}) # not a parent scope so does not need to clean
	target_link_extlibraries(${FB_GUI_LIB_NAME})
else()
	target_link_libraries(${FB_GUI_LIB_NAME}
                                Qt5::Widgets
                                ${FB_CORE_LIB_NAME} ${FB_UTIL_LIB_NAME})
endif()			
			
			

