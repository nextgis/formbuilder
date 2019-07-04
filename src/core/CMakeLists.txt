####################################################################################################
#                                                                                                  #
#    Project:   NextGIS Formbuilder                                                                #
#    Authors:   Mikhail Gusev, gusevmihs@gmail.com                                                 #
#               Copyright (C) 2014-2019 NextGIS                                                    #
#                                                                                                  #
####################################################################################################

set(FB_CORE_LIB_SOURCES

    attributes/boolean.cpp
    attributes/number.cpp
    attributes/string.cpp
    attributes/stringlist.cpp
    attributes/enum.cpp
    attributes/datetime.cpp
    attributes/datetimeenum.cpp
    attributes/doubleitems.cpp
    attributes/tripleitems.cpp
    attributes/depdoubleitems.cpp
    attributes/globaltextenum.cpp

    elements/label.cpp
    elements/voidspace.cpp
    elements/textedit.cpp
    elements/checkbox.cpp
    elements/combobox.cpp
    elements/depcomboboxes.cpp
    elements/splitcombobox.cpp
    elements/radiogroup.cpp
    elements/datetimepicker.cpp
    elements/coordinates.cpp
    elements/counter.cpp
    elements/dmeter.cpp
    elements/photo.cpp
    elements/signfield.cpp
    elements/ngmform.cpp
    elements/tabs.cpp

    attr.cpp
    attr_registrar.cpp
    fieldslot_registrar.cpp
    elem.cpp
    elem_registrar.cpp
    field.cpp
    layer.cpp
    project.cpp
)

set(FB_CORE_LIB_HEADERS

    attributes/boolean.h
    attributes/number.h
    attributes/string.h
    attributes/stringlist.h
    attributes/enum.h
    attributes/datetime.h
    attributes/datetimeenum.h
    attributes/doubleitems.h
    attributes/tripleitems.h
    attributes/depdoubleitems.h
    attributes/globaltextenum.h

    elements/label.h
    elements/voidspace.h
    elements/textedit.h
    elements/checkbox.h
    elements/combobox.h
    elements/depcomboboxes.h
    elements/splitcombobox.h
    elements/radiogroup.h
    elements/datetimepicker.h
    elements/coordinates.h
    elements/counter.h
    elements/dmeter.h
    elements/photo.h
    elements/signfield.h
    elements/ngmform.h
    elements/tabs.h

    attr.h
    attr_registrar.h
    fieldslot_registrar.h
    elem.h
    elem_registrar.h
    field.h
    layer.h
    project.h

    field_types.h
    geom_types.h
    srs_types.h
    datetime_types.h

    all_registry.h
)

set(FB_CORE_LIB_PUBLIC_HEADERS
)


add_library(${FB_CORE_LIB_NAME} STATIC ${FB_CORE_LIB_SOURCES} ${FB_CORE_LIB_HEADERS})

if(BUILD_NEXTGIS_PACKAGE)
	target_link_extlibraries(${FB_CORE_LIB_NAME})
else()
	target_link_libraries(${FB_CORE_LIB_NAME} Qt5::Core)
endif()

