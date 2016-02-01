/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  formbuilder's project definitions
 * Author:   Mikhail Gusev, gusevmihs@gmail.com
 ******************************************************************************
*   Copyright (C) 2014-2016 NextGIS
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 2 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#ifndef PROJECT_H
#define PROJECT_H


// Note: do not use GDAL < 2.1.0 because of vcizip bug, see
// https://trac.osgeo.org/gdal/ticket/6005
#include "ogrsf_frmts.h"
#include "json/json.h"

// TODO: think about how to set these constants via build system:
// Uncomment for GDAL debug output.
#define _FB_GDAL_DEBUG "D:/nextgis/formbuilder/gdal-log.txt"
#define _FB_INSTALLPATH_GDALDATA "/gdal_data"

// Key names.
#define FB_PROJECT_EXTENSION "ngfp"
#define FB_PROJECT_FORM_FILENAME "form.json"
#define FB_PROJECT_DATA_FILENAME "data.geojson"
#define FB_PROJECT_META_FILENAME "meta.json"


/**
 * Abstract project class which represents .ngfp project: methods for saving,
 * loading and operating form&layer package file.
 */
class FBProject
{
    public:
    
     // main methods
     static void init ();
     virtual bool save ();
     virtual bool load ();
    
    protected:
    
    private:
    
     // main private methods
     bool readFormJson ();
     void writeFormJson ();
     
     // project's metadata
    
};

#endif //PROJECT_H
