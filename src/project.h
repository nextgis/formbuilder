/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  formbuilder's project and its basic implementations
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

#include <QList>
#include <QMap>
#include <QString>

// Note: do not use GDAL < 2.1.0 in program at all because of vcizip bug, see
// https://trac.osgeo.org/gdal/ticket/6005
#include "ogrsf_frmts.h"
#include "json/json.h"

#include "fb_general.h"
//#include "form.h"

// TODO: think about how to set these constants via build system:
// Uncomment for GDAL debug output.
#define _FB_GDAL_DEBUG "D:/nextgis/formbuilder/gdal-log.txt"
#define _FB_INSTALLPATH_GDALDATA "/gdal_data"

// Key names.
#define FB_PROJECT_EXTENSION "ngfp"
#define FB_PROJECT_FORM_FILENAME "form.json"
#define FB_PROJECT_DATA_FILENAME "data.geojson"
#define FB_PROJECT_META_FILENAME "meta.json"


struct FBFieldDescr // see NextGISWeb fields description syntax
{
    QString datataype;
    QString display_name;
};

struct FBNgwConnection
{
    int id;
    QString login;
    QString password;
    QString url;
};


/**
 * Abstract project class which represents its file on disk (.ngfp file).
 * Project contains 1) form, 2) layer (data) and 3) metadata.
 * Note: this class does not keep the layer's data in memory (it
 * will be accessed on disk each time) and the form itself (it is owned
 * by the screen widget), but it is able to read/write them into project
 * file.
 *
 * Project types differ from each other only in creation and first saving
 * capabilities, because other work with project is just working with
 * .ngfp files - which is always the same.
 */
class FBProject
{
    public: // static members

     static void initAll ();
     static QStringList DATA_TYPES; // see NextGISWeb types syntax
     static QStringList GEOM_TYPES;
     static QList<int> SRS_TYPES;
     static QString strCurErrInfo; // additional error text if was some

    public: // methods

     FBProject ();
     virtual ~FBProject ();

     // main methods
//     virtual FBErr create (QString strPathAny) = 0;
//     virtual FBErr saveFirst (QString strPathNgfp, FBForm *formPtr) = 0;
//     FBErr saveAs (QString strPathNgfp, FBForm *formPtr);
//     FBErr save (FBForm *formPtr);
//     FBErr open (QString strPathNgfp);
//     FBErr fillFormFromJson (FBForm *formPtr); // return form reading from disk

     // info
     bool wasFirstSaved () {return false;}

    protected: // methods

     // working with ngfp
//     FBErr writeFormToJson (FBForm *formPtr);
     FBErr readJsonMeta ();
     FBErr writeJsonMeta ();

    protected: // fields
     
     // current state and link to the disk representation - i.e. ngfp file
     bool isInited; // whether the project had been already created or opened
     QString ngfpPath; // if path is void - project needs to be saved first time

     // project's images
     // QList<QImage> images;

     // project's metadata
     QMap<QString,FBFieldDescr> fields; // keyname is a unique key in the map
     QString geometry_type;
     FBNgwConnection ngw_connection; // it is here because of ngfp file syntax
     int srs;
     float version;
};


class FBProjectVoid: public FBProject
{
    public:
     FBProjectVoid (QString geometry_type);
     ~FBProjectVoid ();
     virtual FBErr create ();
//     virtual FBErr saveFirst (QString strPathNgfp, FBForm *formPtr);
};


#endif //PROJECT_H
