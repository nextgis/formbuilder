/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  basic project implementations
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

#include <QByteArray>
#include <QDir>

#include "project.h"
 
QStringList FBProject::DATA_TYPES;
QStringList FBProject::GEOM_TYPES;
QList<int> FBProject::SRS_TYPES;
QString FBProject::strCurErrInfo = ""; // no error for now


/****************************************************************************/
/*                              initAll                                     */
/****************************************************************************/
/**
 *
 */
void FBProject::initAll ()
{
    // Settings for GDAL.
    // We need GDAL in any project, even void, because we will at least use vcizip
    // for packaging to ngfp and GeoJSON driver for data layer creation.
    #ifdef _FB_GDAL_DEBUG
    CPLSetConfigOption("CPL_DEBUG","ON");
    CPLSetConfigOption("CPL_CURL_VERBOSE","YES");
    CPLSetConfigOption("CPL_LOG",_FB_GDAL_DEBUG);
    CPLSetConfigOption("CPL_LOG_ERRORS","ON");
    #endif
    CPLSetConfigOption("CPL_VSIL_ZIP_ALLOWED_EXTENSIONS",FB_PROJECT_EXTENSION);
    QByteArray ba = QString(QDir::currentPath() + _FB_INSTALLPATH_GDALDATA).toUtf8();
    CPLSetConfigOption("GDAL_DATA", ba.data());

    GDALAllRegister();

    //FBRegistrar::registerProjectTypes();

    GEOM_TYPES.append("POINT");
    GEOM_TYPES.append("LINESTRING");
    GEOM_TYPES.append("POLYGON");
    GEOM_TYPES.append("MULTIPOINT");
    GEOM_TYPES.append("MULTILINESTRING");
    GEOM_TYPES.append("MULTIPOLYGON");

    DATA_TYPES.append("STRING");
    DATA_TYPES.append("INTEGER");
    DATA_TYPES.append("REAL");
    DATA_TYPES.append("DATE");

    SRS_TYPES.append(4326);
}


/****************************************************************************/
/*                         FBProject Constructor                            */
/****************************************************************************/
/**
 *
 */
FBProject::FBProject ()
{
    isInited = false;
    ngfpPath = "";
}


/****************************************************************************/
/*                         FBProject Destructor                             */
/****************************************************************************/
/**
 *
 */
FBProject::~FBProject ()
{

}


/****************************************************************************/
/*                         FBProjectVoid Constructor                        */
/****************************************************************************/
/**
 *
 */
FBProjectVoid::FBProjectVoid (QString geometry_type): FBProject()
{
    this->geometry_type = geometry_type;
}


/****************************************************************************/
/*                         FBProjectVoid Destructor                         */
/****************************************************************************/
/**
 *
 */
FBProjectVoid::~FBProjectVoid ()
{
}


/****************************************************************************/
/*                       FBProjectVoid::create()                            */
/****************************************************************************/
/**
 *
 */
FBErr FBProjectVoid::create ()
{
    if (isInited)
        return FBErrAlreadyInited;
    isInited = true;



}






