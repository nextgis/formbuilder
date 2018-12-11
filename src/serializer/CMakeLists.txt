####################################################################################################
#                                                                                                  #
#    Project:   NextGIS Formbuilder                                                                #
#    Authors:   Mikhail Gusev, gusevmihs@gmail.com                                                 #
#               Copyright (C) 2014-2018 NextGIS                                                    #
#                                                                                                  #
####################################################################################################

set(FB_SERIALIZER_LIB_SOURCES

#    ngfp_reader.cpp
    ngfp_writer.cpp
)

set(FB_SERIALIZER_LIB_HEADERS

#    ngfp_reader.h
    ngfp_writer.h

    ngfp_common.h
)

set(FB_SERIALIZER_LIB_PUBLIC_HEADERS

)


# if(BUILD_NEXTGIS_PACKAGE)
	# qt5_wrap_cpp(MOC_HEADERS ${FB_SERIALIZER_LIB_HEADERS})
	# set(FB_SERIALIZER_LIB_HEADERS ${FB_SERIALIZER_LIB_HEADERS} ${MOC_HEADERS})
# endif()

add_library(${FB_SERIALIZER_LIB_NAME} STATIC ${FB_SERIALIZER_LIB_SOURCES} ${FB_SERIALIZER_LIB_HEADERS})

if(BUILD_NEXTGIS_PACKAGE)
	target_link_extlibraries(${FB_SERIALIZER_LIB_NAME})
else()
	target_link_libraries(${FB_SERIALIZER_LIB_NAME} Qt5::Core)# Qt5::Widgets)
endif()	
