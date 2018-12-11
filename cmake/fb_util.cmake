####################################################################################################
#                                                                                                  #
#    Project:   NextGIS Formbuilder                                                                #
#    Authors:   Mikhail Gusev, gusevmihs@gmail.com                                                 #
#               Copyright (C) 2014-2018 NextGIS                                                    #
#                                                                                                  #
####################################################################################################

function(get_gdal_version version)
    if(GDAL_INCLUDE_DIR)
        set(VERSION_FILE ${GDAL_INCLUDE_DIR}/gdal_version.h)
        file(READ ${VERSION_FILE} GDAL_VERSION_H_CONTENTS)
        string(REGEX MATCH "GDAL_VERSION_MAJOR[ \t]+([0-9]+)"
          GDAL_VERSION_MAJOR ${GDAL_VERSION_H_CONTENTS})
        string (REGEX MATCH "([0-9]+)"
          GDAL_VERSION_MAJOR ${GDAL_VERSION_MAJOR})
        string(REGEX MATCH "GDAL_VERSION_MINOR[ \t]+([0-9]+)"
          GDAL_VERSION_MINOR ${GDAL_VERSION_H_CONTENTS})
        string (REGEX MATCH "([0-9]+)"
          GDAL_VERSION_MINOR ${GDAL_VERSION_MINOR})
        string(REGEX MATCH "GDAL_VERSION_REV[ \t]+([0-9]+)"
          GDAL_VERSION_REV ${GDAL_VERSION_H_CONTENTS})
        string (REGEX MATCH "([0-9]+)"
          GDAL_VERSION_REV ${GDAL_VERSION_REV})
    endif()
    set(${version} ${GDAL_VERSION_MAJOR}.${GDAL_VERSION_MINOR}.${GDAL_VERSION_REV} PARENT_SCOPE)
endfunction()
