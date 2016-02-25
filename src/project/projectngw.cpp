/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  NextGIS Web project implementations
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
 

FBProjectNgw::FBProjectNgw (QString strUrl, QString strLogin,
                            QString strPass, int nId, Json::Value jsonMeta):
    FBProjectGDAL ()
{
    ngw_connection.url = strUrl;
    ngw_connection.login = strLogin;
    ngw_connection.password = strPass;
    ngw_connection.id = nId;
    jsonTempMeta = jsonMeta;
}

FBProjectNgw::~FBProjectNgw ()
{
}

FBErr FBProjectNgw::create (QString anyPath)
{
    if (isInited)
        return FBErrAlreadyInited;

    // Set special GDAL settings for accessing remote GeoJSON dataset.
    QString logPas = ngw_connection.login + ":" + ngw_connection.password;
    QByteArray baLogPas = logPas.toUtf8();
    CPLSetConfigOption("GDAL_HTTP_USERPWD", baLogPas.data());

    // Firstly initialize with default GDAL method.
    FBErr err = this->readGdalDataset(anyPath);
    if (err != FBErrNone)
        return err;

    // Replace metadata which we could not get in a common way, firstly checking its
    // correctness.
    // This metadata was read from the NGW server during the last http connection via
    // specific dialogue. See comments there.
    // TODO: check if we need to do this: the amount of features in the layer must
    // be 0 - only than we must obtain metadata in another way. For now clear array anyway
    // and refill it.
    fields.clear();

    // Check and assign fields.
    Json::Value jsonFields = jsonTempMeta[FB_JSON_META_FIELDS];
    if (jsonFields.isNull() || !jsonFields.isArray() )
    {
        FBProject::CUR_ERR_INFO = QObject::tr("Selected GeoJSON dataset has"
              " no or incorrect fields definition");
        return FBErrIncorrectGdalDataset;
    }
    for (int k=0; k<jsonFields.size(); ++k)
    {
        Json::Value jsonKn = jsonFields[k][FB_JSON_META_KEYNAME];
        Json::Value jsonDt = jsonFields[k][FB_JSON_META_DATATYPE];
        Json::Value jsonDn = jsonFields[k][FB_JSON_META_DISPLAY_NAME];
        if (jsonKn.isNull() || !jsonKn.isString()
                || jsonDt.isNull() || !jsonDt.isString()
                || jsonDn.isNull() || !jsonDn.isString()
                || !FBProject::isDataTypeSupported(
                    QString::fromUtf8(jsonDt.asString().data())))
        {
            FBProject::CUR_ERR_INFO = QObject::tr("Selected GeoJSON dataset's"
                  " array of fields has incorrect structure");
            return FBErrIncorrectGdalDataset;
        }
        FBFieldDescr fieldDescr;
        fieldDescr.datataype = DATA_TYPES.value(
                    QString::fromUtf8(jsonDt.asString().data()));
        fieldDescr.display_name = QString::fromUtf8(jsonDn.asString().data());
        fields.insert(QString::fromUtf8(jsonKn.asString().data()), fieldDescr);
    }

    // Check and assign geometry type.
    Json::Value jsonGeom = jsonTempMeta[FB_JSON_META_GEOMETRY_TYPE];
    if (jsonGeom.isNull() || !FBProject::isGeomTypeSupported(
                QString::fromUtf8(jsonGeom.asString().data())))
    {
        FBProject::CUR_ERR_INFO = QObject::tr("Selected GeoJSON dataset has"
              " unsupportd geometry type or does not have it at all");
        return FBErrIncorrectGdalDataset;
    }
    geometry_type = GEOM_TYPES.value(QString::fromUtf8(jsonGeom.asString().data()));

    // Ngw settings are already set via constructor.

    // Other metadadta.
    srs = FBSrs4326; // ignore SRS type which was read while we must use default one
    version = FBProject::getProgVersionStr();

    strDatasetPath = anyPath; // store the path to dataset for the first save
    strNgfpPath = ""; // need to be saved first time

    isInited = true;
    return FBErrNone;
}

FBErr FBProjectNgw::saveFirst (QString ngfpFullPath, Json::Value jsonForm)
{
    if (!isInited)
        return FBErrNotInited;

    return FBErrNone;
}
