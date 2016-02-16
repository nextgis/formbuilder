/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  
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

#include "project.h"
 

FBProjectVoid::~FBProjectVoid ()
{
}

FBProjectVoid::FBProjectVoid (FBGeomType geometry_type): FBProject()
{
    this->geometry_type = geometry_type;
}

FBErr FBProjectVoid::create (QString anyPath)
{
    if (isInited)
        return FBErrAlreadyInited;

    // Create default field for future layer, because in project must be at least
    // one field. Also layers with no fields are incorrect for NextGIS Web.
    FBFieldDescr fd(FBInteger,"ID");
    fields.insert("ID",fd);
    srs = FBSrs4326;
    version = FBProject::getProgVersionStr();

    strNgfpPath = ""; // need to be saved first time
    isInited = true;

    return FBErrNone;
}

FBErr FBProjectVoid::saveFirst (QString ngfpFullPath, Json::Value jsonForm)
{
    if (!isInited)
        return FBErrNotInited;


    return FBErrNone;
}

