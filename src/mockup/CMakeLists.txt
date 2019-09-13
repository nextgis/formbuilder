####################################################################################################
#                                                                                                  #
#    Project:   NextGIS Formbuilder                                                                #
#    Authors:   Mikhail Gusev, gusevmihs@gmail.com                                                 #
#               Copyright (C) 2014-2019 NextGIS                                                    #
#                                                                                                  #
####################################################################################################

set(FB_MOCKUP_LIB_SOURCES

    elemviews/default_view.cpp
    elemviews/box_view.cpp
    elemviews/label_view.cpp
    elemviews/textedit_view.cpp
    elemviews/checkbox_view.cpp
    elemviews/combobox_view.cpp
    elemviews/depcomboboxes_view.cpp
    elemviews/splitcombobox_view.cpp
    elemviews/radiogroup_view.cpp
    elemviews/datetimepicker_view.cpp
    elemviews/counter_view.cpp
    elemviews/tabs_view.cpp
    elemviews/ngmform_view.cpp

    decorators/androidanydecorator.cpp

    custom_widget.cpp
    space.cpp
    elemview.cpp
    elemview_registrar.cpp
    container.cpp
    screen.cpp
    device.cpp
    decorator.cpp
)

set(FB_MOCKUP_LIB_HEADERS

    elemviews/default_view.h
    elemviews/box_view.h
    elemviews/label_view.h
    elemviews/textedit_view.h
    elemviews/checkbox_view.h
    elemviews/combobox_view.h
    elemviews/depcomboboxes_view.h
    elemviews/splitcombobox_view.h
    elemviews/radiogroup_view.h
    elemviews/datetimepicker_view.h
    elemviews/counter_view.h
    elemviews/tabs_view.h
    elemviews/ngmform_view.h

    decorators/androidanydecorator.h

    custom_widget.h
    space.h
    elemview.h
    elemview_registrar.h
    container.h
    screen.h
    device.h
    decorator.h

    device_data.h

    all_registry.h
)

set(FB_MOCKUP_LIB_PUBLIC_HEADERS

)

set(FB_MOCKUP_LIB_UIS

    device.ui
)

set(FB_MOCKUP_LIB_RESOURCES

    resources/resources.qrc
)
qt5_add_resources(FB_MOCKUP_LIB_QRCS ${FB_MOCKUP_LIB_RESOURCES})


add_library(${FB_MOCKUP_LIB_NAME} STATIC ${FB_MOCKUP_LIB_SOURCES} ${FB_MOCKUP_LIB_HEADERS}
                                         ${FB_MOCKUP_LIB_UIS} ${FB_MOCKUP_LIB_QRCS})

if(BUILD_NEXTGIS_PACKAGE)
	set(TARGET_LINK_LIB ${TARGET_LINK_LIB} ${FB_CORE_LIB_NAME} ${FB_UTIL_LIB_NAME}) # not a parent scope so does not need to clean
	target_link_extlibraries(${FB_MOCKUP_LIB_NAME})
else()
	target_link_libraries(${FB_MOCKUP_LIB_NAME} Qt5::Widgets ${FB_CORE_LIB_NAME} ${FB_UTIL_LIB_NAME})
endif()	
