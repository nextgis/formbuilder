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
 
QMap<QString,FBDataType> FBProject::DATA_TYPES;
QMap<QString,FBGeomType> FBProject::GEOM_TYPES;
QMap<int,FBSrsType> FBProject::SRS_TYPES;
QString FBProject::CUR_ERR_INFO = "";

void FBProject::init ()
{
    // Settings for GDAL.
    // We need GDAL in any project, even void, because we will at least use vsizip
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
    // TODO: do we need to deregister/reset smth of this at the end of the program?
    GDALAllRegister();

    GEOM_TYPES.insert("POINT",FBPoint);
    GEOM_TYPES.insert("LINESTRING",FBLinestring);
    GEOM_TYPES.insert("POLYGON",FBPolygon);
    GEOM_TYPES.insert("MULTIPOINT",FBMultiPoint);
    GEOM_TYPES.insert("MULTILINESTRING",FBMultiLinestring);
    GEOM_TYPES.insert("MULTIPOLYGON",FBMultiPolygon);

    DATA_TYPES.insert("STRING",FBString);
    DATA_TYPES.insert("INTEGER",FBInteger);
    DATA_TYPES.insert("REAL",FBReal);
    DATA_TYPES.insert("DATE",FBDate);

    SRS_TYPES.insert(4326,FBSrs4326);
}


QString FBProject::getProgVersionStr ()
{
    return QString::number(_FB_VERSION,'f',1);
}

bool FBProject::isGeomTypeSupported (QString strGeomType)
{
    return GEOM_TYPES.contains(strGeomType);
}

bool FBProject::isDataTypeSupported (QString strDataType)
{
    return DATA_TYPES.contains(strDataType);
}


FBProject::~FBProject ()
{
}

FBProject::FBProject ()
{
    isInited = false;
    strNgfpPath = "";
    ngw_connection = FBNgwConnection(-1,"","",""); // not ngw dataset
}

FBErr FBProject::open (QString ngfpFullPath)
{
    if (isInited)
        return FBErrAlreadyInited;

    Json::Value jsonMeta = this->readMeta(ngfpFullPath);
    Json::Value jsonForm = this->readForm(ngfpFullPath);
    bool hasData = this->checkData(ngfpFullPath);

    // The main checks if JSON files exist and are correct.
    if (jsonMeta.isNull() || jsonForm.isNull() || !hasData)
    {
        return FBErrIncorrectFileStructure;
    }

    // Compare version of this program and .ngfp file version.
    QString versFile = QString::fromUtf8(jsonMeta[FB_JSON_META_VERSION].asString()
                                         .data());
    QString versProg = FBProject::getProgVersionStr();
    if (versFile != versProg)
    {
        FBProject::CUR_ERR_INFO = QObject::tr("Project file is of version ")
                               + versFile + QObject::tr(", while the program is of "
                               "version ") + versProg;
        return FBErrWrongVersion;
    }

    // Set project's data.
    // All data correctness (syntax and values) has been already checked.
    for (int k=0; k<jsonMeta[FB_JSON_META_FIELDS].size(); ++k)
    {
        Json::Value v = jsonMeta[FB_JSON_META_FIELDS][k];
        QString s1 = QString::fromUtf8(v[FB_JSON_META_DATATYPE].asString().data());
        FBDataType dt1 = DATA_TYPES[s1];
        QString s2 = QString::fromUtf8(v[FB_JSON_META_DISPLAY_NAME].asString().data());
        QString s3 = QString::fromUtf8(v[FB_JSON_META_KEYNAME].asString().data());
        fields.insert(s3,FBFieldDescr(dt1,s2));
    }
    QString sg = QString::fromUtf8(jsonMeta[FB_JSON_META_GEOMETRY_TYPE]
                                      .asString().data());
    geometry_type = GEOM_TYPES[sg];
    Json::Value v = jsonMeta[FB_JSON_META_SRS][FB_JSON_META_NGW_CONNECTION];
    if (v.isNull())
    {
        ngw_connection.id = -1;
    }
    else
    {
        int k1 = v[FB_JSON_META_ID].asInt();
        QString s1 = QString::fromUtf8(v[FB_JSON_META_LOGIN].asString().data());
        QString s2 = QString::fromUtf8(v[FB_JSON_META_PASSWORD].asString().data());
        QString s3 = QString::fromUtf8(v[FB_JSON_META_URL].asString().data());
        ngw_connection = FBNgwConnection(k1,s1,s2,s3);
    }
    int n1 = jsonMeta[FB_JSON_META_SRS][FB_JSON_META_ID].asInt();
    srs = SRS_TYPES[n1];
    version = versProg;

    strNgfpPath = ngfpFullPath;

    isInited = true;
    return FBErrNone;
}


FBErr FBProject::saveAs (QString ngfpFullPath, Json::Value jsonForm)
{
    if (!isInited)
        return FBErrNotInited;

    return FBErrNone;
}


FBErr FBProject::save (Json::Value jsonForm)
{
    if (!isInited)
        return FBErrNotInited;

    return FBErrNone;
}


Json::Value FBProject::readForm (QString ngfpFullPath)
{
    Json::Value jsonRet;
    jsonRet = FBProject::readJsonInNgfp(ngfpFullPath,FB_PROJECT_FORM_FILENAME);
    if (jsonRet.isNull())
        return jsonRet;

    // TODO: check form for syntax errors: correct array structure, ...

    return jsonRet;
}


Json::Value FBProject::readMeta (QString ngfpFullPath)
{
    Json::Value jsonRet;
    jsonRet = FBProject::readJsonInNgfp(ngfpFullPath,FB_PROJECT_META_FILENAME);
    if (jsonRet.isNull())
        return jsonRet;

    // TODO: check metadata for syntax errors and structure:
    // - utf8 encoding?
    // - Key values for geometry_type and datatype.
    // - Existance and correctness: fields list, SRS, NGW connection string, geometry
    // - must be only one fixed SRS
    // type - so they can be translated to correct data for project, i.e. using
    // Json::Value::asInt() or as array of json values, etc.

    return jsonRet;
}


bool FBProject::checkData (QString ngfpFullPath)
{
    // TODO: check data file, firstly its existance.
    // ...

    return true;
}


bool FBProject::wasFirstSaved ()
{
    return strNgfpPath != "";
}


bool FBProject::isSaveRequired ()
{
    return true;
}


FBErr FBProject::writeForm (Json::Value jsonForm)
{

    return FBErrNone;
}


FBErr FBProject::writeMeta (QString strPathNgfp)
{

    return FBErrNone;
}


Json::Value FBProject::readJsonInNgfp (QString ngfpFullPath, QString fileName)
{
    Json::Value jsonNull;
    Json::Value jsonRet;
    QByteArray ba;
    VSILFILE *fp;
    Json::Reader jsonReader;

    ngfpFullPath.prepend("/vsizip/");
    ngfpFullPath.append("/");
    ngfpFullPath.append(fileName);
    ba = ngfpFullPath.toUtf8();
    fp = VSIFOpenL(ba.data(), "rb");
    if (fp == NULL)
    {
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to open ") + fileName
                                   + QObject::tr(" in ZIP-archive via GDAL vsizip");
        return jsonNull;
    }

    std::string jsonConStr = "";
    do
    {
        const char *str = CPLReadLineL(fp);
        if (str == NULL)
            break;
        jsonConStr += str;
    }
    while (true);
    VSIFCloseL(fp);
    if (!jsonReader.parse(jsonConStr, jsonRet, false)
            || jsonRet.isNull())
    {
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to read or parse ") + fileName;
        return jsonNull;
    }

    return jsonRet;
}



/*****************************************************************************/
/*                                                                           */
/*                            FBProjectGDAL                                  */
/*                                                                           */
/*****************************************************************************/

FBProjectGDAL::FBProjectGDAL ():
    FBProject()
{
    strDatasetPath = "";
}

FBProjectGDAL::~FBProjectGDAL ()
{
}


FBErr FBProjectGDAL::readGdalDataset (QString datasetPath)
{
    // Firstly try to open dataset and check its correctness.
    QByteArray ba;
    ba = datasetPath.toUtf8();
    GDALDataset *dataset;
    dataset = (GDALDataset*) GDALOpenEx(ba.data(), GDAL_OF_VECTOR,
                                        NULL, NULL, NULL);
    if (dataset == NULL)
    {
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to open dataset via GDAL");
        return FBErrIncorrectGdalDataset;
    }

    // Check layer's count.
    int layerCount = dataset->GetLayerCount();
    if (layerCount == 0 || layerCount > 1)
    {
        GDALClose(dataset);
        FBProject::CUR_ERR_INFO = QObject::tr("Selected GDAL dataset must contain 1"
              "layer, while it contains ") + QString::number(layerCount);
        return FBErrIncorrectGdalDataset;
    }

    OGRLayer *layer = dataset->GetLayer(0);
    if (layer == NULL)
    {
        GDALClose(dataset);
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to read layer in selected GDAL"
              "dataset");
        return FBErrIncorrectGdalDataset;
    }

    OGRFeatureDefn *layerDefn = layer->GetLayerDefn();
    if (layerDefn->GetFieldCount() == 0)
    {
        GDALClose(dataset);
        FBProject::CUR_ERR_INFO = QObject::tr("Selected GDAL dataset's layer must"
              " contain at least one field, while it contains no fields");
        return FBErrIncorrectGdalDataset;
    }

    OGRSpatialReference *layerSpaRef = layer->GetSpatialRef();
    if (layerSpaRef == NULL)
    {
        GDALClose(dataset);
        FBProject::CUR_ERR_INFO = QObject::tr("Selected GDAL dataset does not contain"
              " its spatial reference system description");
        return FBErrIncorrectGdalDataset;
    }

    // All checks were made and we can fill some (not all) metadata of the project,
    // which can be read via GDAL.
    for (int i=0; i<layerDefn->GetFieldCount(); i++)
    {
        FBFieldDescr descr;
        OGRFieldDefn *fieldDefn = layerDefn->GetFieldDefn(i);
        OGRFieldType fieldType = fieldDefn->GetType();
        switch (fieldType)
        {
            case OFTInteger:
                descr.datataype = FBInteger;
            break;
            case OFTInteger64:
                descr.datataype = FBInteger; // WARNING: data will be truncated in FB
            break;
            case OFTReal:
                descr.datataype = FBReal;
            break;
            case OFTDate:
                descr.datataype = FBDate;
            break;
            default:
                descr.datataype = FBString; //descr.datataype = FBUndefDatatype;
            break;
        }
        descr.display_name = QString::fromUtf8(fieldDefn->GetNameRef());
        fields.insert(QString::fromUtf8(fieldDefn->GetNameRef()), descr);
    }

    OGRwkbGeometryType geomType = layerDefn->GetGeomType();
    switch (geomType)
    {
        case wkbPoint:
            geometry_type = FBPoint;
        break;
        case wkbLineString:
            geometry_type = FBLinestring;
        break;
        case wkbPolygon:
            geometry_type = FBPolygon;
        break;
        case wkbMultiPoint:
            geometry_type = FBMultiPoint;
        break;
        case wkbMultiLineString:
            geometry_type = FBMultiLinestring;
        break;
        case wkbMultiPolygon:
            geometry_type = FBMultiPolygon;
        break;
        default:
            geometry_type = FBUndefGeomtype;
        break;
    }

    GDALClose(dataset);

    return FBErrNone;
}



