/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  different projects
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

#ifndef PROJECTS_H
#define PROJECTS_H

#include "project_core.h"

// Project based on newly created layer.
class FBProjectVoid: public FBProject
{
    public:
     FBProjectVoid (FbGeomType *geomType);
     virtual ~FBProjectVoid ();
     virtual FBErr readFirst (QString anyPath);
     virtual QString getDatasetPath () { return QObject::tr("no dataset"); }
};

// Abstract project based on GDAL dataset.
class FBProjectGdal: public FBProject
{
    public:
     FBProjectGdal ();
     virtual ~FBProjectGdal ();
     QString getDatasetPath () { return strDatasetPath; }
    protected:
     FBErr readFromDataset (QString datasetPath);
     FBErr writeDataFileFirst (QString strPath);
    protected:
     QString strDatasetPath;
};

// Project based on Shapefile dataset.
class FBProjectShapefile: public FBProjectGdal
{
    public:
     FBProjectShapefile ();
     ~FBProjectShapefile ();
     FBErr readFirst (QString anyPath);
};

// Project based on NGW connection (finally on received via http GeoJSON dataset).
class FBProjectNgw: public FBProjectGdal
{
    public:
     FBProjectNgw (QString strUrl, QString strLogin,
                   QString strPass, int nId, Json::Value jsonMeta);
     virtual ~FBProjectNgw ();
     virtual FBErr readFirst (QString anyPath);
    protected:
     Json::Value jsonTempMeta; // only for initialize step
};


#endif //PROJECTS_H
