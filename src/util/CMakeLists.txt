####################################################################################################
#                                                                                                  #
#    Project:   NextGIS Formbuilder                                                                #
#    Authors:   Mikhail Gusev, gusevmihs@gmail.com                                                 #
#               Copyright (C) 2014-2018 NextGIS                                                    #
#                                                                                                  #
####################################################################################################

set(FB_UTIL_LIB_SOURCES

    csv_io.cpp
    json_io.cpp
    zip_io.cpp
    ngw_io.cpp
    ngw_default_io.cpp
    ngw_gdal_io.cpp
    web_worker.cpp
    ngw_api.cpp
)

set(FB_UTIL_LIB_HEADERS

    csv_io.h
    json_io.h
    zip_io.h
    ngw_io.h
    ngw_default_io.h
    ngw_gdal_io.h
    web_worker.h
    ngw_api.h

    ngw_types.h
    settings.h
    common.h
    gdal_dataset_deleter.h
)

set(FB_UTIL_LIB_PUBLIC_HEADERS

)


add_library(${FB_UTIL_LIB_NAME} STATIC ${FB_UTIL_LIB_SOURCES} ${FB_UTIL_LIB_HEADERS})

if(BUILD_NEXTGIS_PACKAGE)
    target_link_extlibraries(${FB_UTIL_LIB_NAME})
else()
    target_link_libraries(${FB_UTIL_LIB_NAME} Qt5::Widgets Qt5::Network ${NGSTD_LIBRARIES})
endif()
