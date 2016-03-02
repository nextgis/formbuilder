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
#include <QTemporaryDir>

#include <QTextStream>

#include "project.h"
 
QList<FbGeomType*> FBProject::GEOM_TYPES;
QList<FbDataType*> FBProject::DATA_TYPES;
QList<FbSrsType*> FBProject::SRS_TYPES;

QString FBProject::CUR_ERR_INFO = "";

void FBProject::init ()
{
    // We need to register enum type for passing it as a parameter of signal/slot
    // pair through the threads in FB class.
    qRegisterMetaType<FBErr>("FBErr");

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
    GDALAllRegister();

    // Geometry types.
    // Note: no default value for this list.
    // WARNING. All values must be unique.
    GEOM_TYPES.append(new FbGeomType("POINT", wkbPoint));
    GEOM_TYPES.append(new FbGeomType("LINESTRING", wkbLineString));
    GEOM_TYPES.append(new FbGeomType("POLYGON", wkbPolygon));
    GEOM_TYPES.append(new FbGeomType("MULTIPOINT", wkbMultiPoint));
    GEOM_TYPES.append(new FbGeomType("MULTILINESTRING", wkbMultiLineString));
    GEOM_TYPES.append(new FbGeomType("MULTIPOLYGON", wkbMultiPolygon));

    // Data types.
    // WARNING. The 0 and 1 list value (string and integer types) have its named
    // representations (its indexes are used in program).
    // WARNING. All values must be unique.
    DATA_TYPES.append(new FbDataType("STRING", OFTString));
    QList<OGRFieldType> list;
    list.append(OFTInteger64);
    DATA_TYPES.append(new FbDataType("INTEGER", OFTInteger, list));
    DATA_TYPES.append(new FbDataType("REAL", OFTReal));
    DATA_TYPES.append(new FbDataType("DATE", OFTDate));
    DATA_TYPES.append(new FbDataType("TIME", OFTTime));
    DATA_TYPES.append(new FbDataType("DATETIME", OFTDateTime));

    // Geometry types.
    // WARNING. The 0 list value (WGS84 SRS) is default for the program.
    // WARNING. All values must be unique.
    SRS_TYPES.append(new FbSrsType(4326, SRS_WKT_WGS84));
}


void FBProject::deinit ()
{
    for (int i=0; i<GEOM_TYPES.size(); i++)
        delete GEOM_TYPES[i];
    for (int i=0; i<DATA_TYPES.size(); i++)
        delete DATA_TYPES[i];
    for (int i=0; i<SRS_TYPES.size(); i++)
        delete SRS_TYPES[i];
    // TODO: do we need to deregister/reset smth for GDAL at the end of the program?
}


QString FBProject::getProgVersionStr ()
{
    return QString::number(_FB_VERSION,'f',1);
}


FbGeomType *FBProject::findGeomTypeByNgw (QString aliasNgw)
{
    for (int i=0; i<GEOM_TYPES.size(); i++)
        if (GEOM_TYPES[i]->aliasNgw == aliasNgw)
            return GEOM_TYPES[i];
    return NULL;
}
FbGeomType *FBProject::findGeomTypeByGdal (OGRwkbGeometryType aliasGdal)
{
    for (int i=0; i<GEOM_TYPES.size(); i++)
        if (GEOM_TYPES[i]->aliasGdal == aliasGdal)
            return GEOM_TYPES[i];
    return NULL;
}
FbDataType *FBProject::findDataTypeByNgw (QString aliasNgw)
{
    for (int i=0; i<DATA_TYPES.size(); i++)
        if (DATA_TYPES[i]->aliasNgw == aliasNgw)
            return DATA_TYPES[i];
    return NULL;
}
FbDataType *FBProject::findDataTypeByGdal (OGRFieldType aliasGdal)
{
    for (int i=0; i<DATA_TYPES.size(); i++)
        if (DATA_TYPES[i]->aliasMainGdal == aliasGdal
                || DATA_TYPES[i]->aliasesOtherGdal.contains(aliasGdal))
            return DATA_TYPES[i];
    return NULL;
}
FbSrsType *FBProject::findSrsTypeByNgw (int numberNgw)
{
    for (int i=0; i<SRS_TYPES.size(); i++)
        if (SRS_TYPES[i]->numberNgw == numberNgw)
            return SRS_TYPES[i];
    return NULL;
}


FBProject::~FBProject ()
{
}

FBProject::FBProject ()
{
    isInited = false;
    strNgfpPath = "";
    ngw_connection = FBNgwConnection(-1,"","",""); // this is not ngw dataset yet
    srs = SRS_TYPES[FB_TYPEINDEX_SRS_WGS84]; // always the only SRS. During the
                                             // first save layer's data will be
                                             // transformed to it
    geometry_type = NULL;
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
    // Note: All data correctness (syntax and values) has been already checked.
    // Also the support for geometry/data/srs types of metadata is also already
    // checked, so we do not check for NULL values here.
    for (int k=0; k<jsonMeta[FB_JSON_META_FIELDS].size(); ++k)
    {
        Json::Value v = jsonMeta[FB_JSON_META_FIELDS][k];
        QString s1 = QString::fromUtf8(v[FB_JSON_META_DATATYPE].asString().data());
        FbDataType *dt1 = FBProject::findDataTypeByNgw(s1);
        QString s2 = QString::fromUtf8(v[FB_JSON_META_DISPLAY_NAME].asString().data());
        QString s3 = QString::fromUtf8(v[FB_JSON_META_KEYNAME].asString().data());
        fields.insert(s3,FBFieldDescr(dt1,s2));
    }
    QString sgt = QString::fromUtf8(jsonMeta[FB_JSON_META_GEOMETRY_TYPE]
                                      .asString().data());
    geometry_type = FBProject::findGeomTypeByNgw(sgt);
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
    // Note: there is no need to read SRS id, while we always use the default one.
    //int n1 = jsonMeta[FB_JSON_META_SRS][FB_JSON_META_ID].asInt();
    version = versProg;

    strNgfpPath = ngfpFullPath;

    isInited = true;
    return FBErrNone;
}


// Main saving method of the project.
// Method resaves each time the whole project file from the beginning because it
// is not possible to write only one file to the ZIP archive at a time - the whole
// archive must be resaved.
// Project file is a ZIP archive which contains 3 JSON/GeoJSON files: metadata,
// layer (features data) and form. Additionally it can contain images for form.
// All writing work is made inside temporary directory, and at the end the new
// project file is created replacing an old one if was some.
FBErr FBProject::saveAs (QString ngfpFullPath, Json::Value jsonForm)
{
    // First checks.
    if (!isInited)
        return FBErrNotInited;
    if (jsonForm.isNull())
        return FBErrNullVal;
    FBErr err;
    emit changeProgress(0);

    // Get path components and create temporary dir for work.
    // This directory with all its content will be automatically deleted when this
    // method will end its work.
    QList<QPair<QString,QString> > strsTempFiles; // first = path, second = name in zip
    QString strNameBase;
    QString strPath;
    this->getPathComponents(ngfpFullPath,strPath,strNameBase);
    QTemporaryDir dir(strPath + "formXXXXXX"); // only for Qt >= 5
    if (!dir.isValid())
    {
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to create temporary dir at ")
                + strPath + QObject::tr("\nTry to select another saving directory");
        return FBErrWrongSavePath;
    }
    QString strTempPath;
    strTempPath = dir.path() + "/";
    emit changeProgress(5);

    // 1. Create and fill metadata file.
    // Metadata is obtained from this project.
    QString strMetaPath = strTempPath + FB_PROJECT_FILENAME_META;
    QFile fileMeta(strMetaPath);
    err = FBProject::fillJsonFile(fileMeta,this->getJsonMetadata());
    if (err != FBErrNone)
        return err;
    strsTempFiles.append(QPair<QString,QString>(strMetaPath,FB_PROJECT_FILENAME_META));
    emit changeProgress(10);

    // 2. Create and fill form file.
    // The form is usually obtained from the screen of the main app gui.
    QString strFormPath = strTempPath + FB_PROJECT_FILENAME_FORM;
    QFile fileForm(strFormPath);
    err = FBProject::fillJsonFile(fileForm,jsonForm);
    if (err != FBErrNone)
        return err;
    strsTempFiles.append(QPair<QString,QString>(strFormPath,FB_PROJECT_FILENAME_FORM));
    emit changeProgress(30);

    // 3. Create and fill layer's data file.
    // Layer's data is obtained from the old project file, but if there is no
    // old file the concrete project class must know how to get data first time.
    QString strDataPath = strTempPath + FB_PROJECT_FILENAME_DATA;
    if (strNgfpPath != "")
    {
        // Open old GeoJSON dataset via GDAL.
        QString strZipPath = strNgfpPath;
        strZipPath.prepend("/vsizip/");
        strZipPath.append("/");
        strZipPath.append(FB_PROJECT_FILENAME_DATA);
        QByteArray baZipPath;
        baZipPath = strZipPath.toUtf8();
        char **allowedDrivers = NULL;
        allowedDrivers = CSLAddString(allowedDrivers,"GeoJSON");
        GDALDataset *datasetOld = (GDALDataset*) GDALOpenEx(baZipPath.data(),
                GDAL_OF_VECTOR | GDAL_OF_READONLY, allowedDrivers, NULL, NULL);
        CSLDestroy(allowedDrivers);
        if (datasetOld == NULL)
        {
            FBProject::CUR_ERR_INFO = QObject::tr("Unable to open GeoJSON data file"
                         " in old project using GDAL /vsizip/");
            return FBErrGDALFail;
        }
        // Create new GeoJSON file and copy data.
        err = this->copyDataFile(datasetOld,strDataPath);
        GDALClose(datasetOld);
        if (err != FBErrNone)
            return err;
    }
    else
    {
        err = this->createDataFileFirst(strDataPath);
        if (err != FBErrNone)
            return err;
    }
    strsTempFiles.append(QPair<QString,QString>(strDataPath,FB_PROJECT_FILENAME_DATA));
    emit changeProgress(75);

    // 4. (Additional) Create images.
    // ...
    emit changeProgress(85);

    // Create ZIP archive in the same temp directory.
    // Write created files to it.
    QString ngfpTempPath = strTempPath + strNameBase + "." + FB_PROJECT_EXTENSION;
    QByteArray baTempPath;
    baTempPath = ngfpTempPath.toUtf8();
    void *hZip = CPLCreateZip(baTempPath.data(),NULL);
    if (!hZip)
    {
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to create final ZIP archive"
                                              " in a temp directory");
        return FBErrCPLFail;
    }
    size_t nBufferSize = 1024 * 1024;
    GByte *pabyBuffer = (GByte*)CPLMalloc(nBufferSize);
    QByteArray baTemp1;
    QByteArray baTemp2;
    bool ok = true;
    for (int i=0; i<strsTempFiles.size(); i++)
    {
        baTemp1 = strsTempFiles[i].first.toUtf8();
        baTemp2 = strsTempFiles[i].second.toUtf8();
        ok = this->addFileToZip(baTemp1.data(), baTemp2.data(), hZip,
                                &pabyBuffer, nBufferSize);
        if (!ok)
        {
            break;
        }
    }
    CPLCloseZip(hZip);
    CPLFree(pabyBuffer);
    if (!ok)
    {
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to write temporary file into"
                                              " final ZIP-arcive");
        return FBErrCPLFail;
    }
    emit changeProgress(95);

    // Create final project file copying it from the temporary directory. If there was
    // an old project file we replace it.
    if (QFile::exists(ngfpFullPath) && !QFile::remove(ngfpFullPath))
    {
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to remove old project file"
                                              " with the same name as the saved one");
        return FBErrTempFileFail;
    }
    if (!QFile::copy(ngfpTempPath, ngfpFullPath))
    {
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to copy final project file from"
                                 " the temporary directory to the selected one");
        return FBErrTempFileFail;
    }

    // Set that this project is already saved.
    strNgfpPath = ngfpFullPath;

    emit changeProgress(99);
    return FBErrNone;
}


Json::Value FBProject::readForm (QString ngfpFullPath)
{
    Json::Value jsonRet;
    jsonRet = FBProject::readJsonInNgfp(ngfpFullPath,FB_PROJECT_FILENAME_FORM);
    if (jsonRet.isNull())
        return jsonRet;

    // TODO: check form for syntax errors: correct array structure, ...

    return jsonRet;
}


Json::Value FBProject::readMeta (QString ngfpFullPath)
{
    Json::Value jsonRet;
    jsonRet = FBProject::readJsonInNgfp(ngfpFullPath,FB_PROJECT_FILENAME_META);
    if (jsonRet.isNull())
        return jsonRet;

    // TODO: check metadata for syntax errors and structure:
    // - utf8 encoding?
    // - Key values for geometry_type and datatype - their existance via findDataType()
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


Json::Value FBProject::getJsonMetadata ()
{
    Json::Value jsonRet;

    Json::Value jsonFields;
    Json::Value jsonGeom;
    Json::Value jsonNgw;
    Json::Value jsonSrs;
    Json::Value jsonVers;

    QMap<QString,FBFieldDescr>::const_iterator it = fields.constBegin();
    while (it != fields.constEnd())
    {
        Json::Value jsonElem;

        QByteArray baDt;
        baDt = it.value().datataype->aliasNgw.toUtf8();
        jsonElem[FB_JSON_META_DATATYPE] = baDt.data();
        QByteArray baDn;
        baDn = it.value().display_name.toUtf8();
        jsonElem[FB_JSON_META_DISPLAY_NAME] = baDn.data();
        QByteArray baKn;
        baKn = it.key().toUtf8();
        jsonElem[FB_JSON_META_KEYNAME] = baKn.data();

        jsonFields.append(jsonElem);
        ++it;
    }

    QByteArray baGeom;
    baGeom = geometry_type->aliasNgw.toUtf8();
    jsonGeom = baGeom.data();

    if (ngw_connection.id != -1)
    {
        jsonNgw[FB_JSON_META_ID] = ngw_connection.id;
        QByteArray baLogin;
        baLogin = ngw_connection.login.toUtf8();
        jsonNgw[FB_JSON_META_LOGIN] = baLogin.data();
        QByteArray baPas;
        baPas = ngw_connection.password.toUtf8();
        jsonNgw[FB_JSON_META_PASSWORD] = baPas.data();
        QByteArray baUrl;
        baUrl = ngw_connection.url.toUtf8();
        jsonNgw[FB_JSON_META_URL] = baUrl.data();
    }
    // else jsonNgw will be null

    jsonSrs[FB_JSON_META_ID] = srs->numberNgw;

    QByteArray baVers;
    baVers = FBProject::getProgVersionStr().toUtf8();
    jsonVers = baVers.data();

    // Final structure of the file:
    jsonRet[FB_JSON_META_FIELDS] = jsonFields;
    jsonRet[FB_JSON_META_GEOMETRY_TYPE] = jsonGeom;
    jsonRet[FB_JSON_META_NGW_CONNECTION] = jsonNgw;
    jsonRet[FB_JSON_META_SRS] = jsonSrs;
    jsonRet[FB_JSON_META_VERSION] = jsonVers;

    return jsonRet;
}


bool FBProject::wasFirstSaved ()
{
    return strNgfpPath != "";
}


bool FBProject::isSaveRequired ()
{
    return true;
}


// Puts into the passed parameters the component of the full path to the prpject
// file. If the full path does not end with project file extension function
// will put nothing.
void FBProject::getPathComponents (QString strFullPath, QString &strPath,
                                   QString &strNameBase)
{
    if (!strFullPath.endsWith(QString(FB_PROJECT_EXTENSION)))
        return;
    strNameBase = "";
    QString str = strFullPath;
    str.chop(QString(FB_PROJECT_EXTENSION).size()+1); // remove extension with dot
    while (str[str.size()-1] != '/') // all paths inside Qt are with '/' slash
    {
        strNameBase.prepend(str[str.length()-1]);
        str.chop(1);
    }
    strPath = str;
}


// Default implementation with no initial dataset.
FBErr FBProject::createDataFileFirst (QString strPath)
{
    // FOR NOW USE OLD CODE!
    // TODO: Change this to the real creation of new GeoJSON dataset without stub
    // dataset!

    // Create stub memory dataset and copy it to the new GeoJSON datset.
    GDALDriver *driverStub = GetGDALDriverManager()->GetDriverByName("Memory");
    if(driverStub == NULL)
    {
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to initialize GDAL Memory"
                                  " driver for first saving layer's data");
        return FBErrGDALFail;
    }
    GDALDataset *datasetStub = driverStub->Create("stub", 0, 0, 0, GDT_Unknown, NULL);
    if (datasetStub == NULL)
    {
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to initialize GDAL Memory"
                                  " dataset for first saving layer's data");
        return FBErrGDALFail;
    }
    QByteArray baSrs;
    baSrs = srs->strFullGdal.toUtf8(); // this metadata quaranteed inited
    OGRSpatialReference spaRef(baSrs.data());
    OGRwkbGeometryType geomType = geometry_type->aliasGdal; // quaranteed inited
    // Create layer without fields for default. New fields (particulary default fields)
    // will be created in the copyDataFile() method.
    OGRLayer *layerStub = datasetStub->CreateLayer("stublayer",&spaRef,geomType);
    if (layerStub == NULL)
    {
        GDALClose(datasetStub);
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to initialize GDAL Memory"
                                  " layer for first saving layer's data");
        return FBErrGDALFail;
    }
    // Copy layer from stub dataset to the new one with SRS setting and fields creation.
    FBErr err = this->copyDataFile(datasetStub, strPath);
    GDALClose(datasetStub);
    if (err != FBErrNone)
        return err;

    return FBErrNone;
}


// Create GeoJSON file (layer) from the passed dataset.
FBErr FBProject::copyDataFile (GDALDataset *datasetSrcPtr, QString strPath)
{
    if (!isInited)
        return FBErrNotInited;
    if (datasetSrcPtr == NULL)
        return FBErrNullVal;
    FBErr err;

    // Get source layer.
    OGRLayer *layerSrc = datasetSrcPtr->GetLayer(0);
    if (layerSrc == NULL)
    {
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to get layer for data saving"
                                      " from the old dataset");
        return FBErrGDALFail;
    }

    // Create new (target) dataset. This will finally create the GeoJSON file in
    // the passed directiry.
    GDALDriver *driverNew = GetGDALDriverManager()->GetDriverByName("GeoJSON");
    if (driverNew == NULL)
    {
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to initialize GDAL GeoJSON"
                                  " driver for saving layer's data");
        return FBErrGDALFail;
    }
    QByteArray baDataPathNew = strPath.toUtf8();
    GDALDataset *datasetNew = driverNew
                    ->Create(baDataPathNew.data(), 0, 0, 0, GDT_Unknown, NULL);
    if (datasetNew == NULL)
    {
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to create GDAL GeoJSON"
                                  " dataset for saving layer's data");
        return FBErrGDALFail;
    }

    // Create temp Memory dataset which will be used for further layer modifyings
    // because: a) the source layer must not be modified; b) the target layer GeoJSON
    // format does not support some layer operations (e.g. fields deletion).
    GDALDriver *driverTemp = GetGDALDriverManager()->GetDriverByName("Memory");
    if (driverTemp == NULL)
    {
        GDALClose(datasetNew);
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to initialize GDAL Memory"
                                  " driver for saving layer's data");
        return FBErrGDALFail;
    }
    GDALDataset *datasetTemp = driverTemp->Create("temp", 0, 0, 0, GDT_Unknown, NULL);
    if (datasetTemp == NULL)
    {
        GDALClose(datasetNew);
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to create GDAL Memory"
                                  " dataset for saving layer's data");
        return FBErrGDALFail;
    }
    OGRLayer *layerTemp = datasetTemp->CopyLayer(layerSrc, layerSrc->GetName());
    if (layerSrc == NULL)
    {
        GDALClose(datasetTemp);
        GDALClose(datasetNew);
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to create GDAL Memory"
                    " layer for saving layer's data, by copying source layer");
        return FBErrGDALFail;
    }

    // Start layer modifying.

    // 1. Change the structure of fields.
    // All checks will be done inside the method.
    err = this->modifyFieldsOfLayer(layerTemp);
    if (err != FBErrNone)
    {
        GDALClose(datasetTemp);
        GDALClose(datasetNew);
        return err;
    }

    // 2. Transform layer geometries to the single SRS of the project.
    // All checks will be done inside the method.
    err = this->reprojectLayer(layerTemp);
    if (err != FBErrNone)
    {
        GDALClose(datasetTemp);
        GDALClose(datasetNew);
        return err;
    }

    // Finally create the new (target) layer by copying temp layer to the target
    // dataset.
    OGRLayer *layerNew = datasetNew->CopyLayer(layerTemp, layerTemp->GetName());
    if (layerNew == NULL)
    {
        GDALClose(datasetTemp);
        GDALClose(datasetNew);
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to create layer in GDAL"
                       " GeoJSON dataset for saving layer's data");
        return FBErrGDALFail;
    }
    GDALClose(datasetTemp);
    GDALClose(datasetNew);

    return FBErrNone;
}


FBErr FBProject::modifyFieldsOfLayer (OGRLayer *layer)
{
    if (layer == NULL)
    {
        return FBErrNullVal;
    }

    // IMPORTANT TODO: check if layer is able to delete fields!
    // Currently only Memory layer is passed to this method.

    // Delete fields with their data which were marked as deleted.
    if (fieldsDeleted.size() > 0)
    {
        QSet<QString>::const_iterator it = fieldsDeleted.constBegin();
        while (it != fieldsDeleted.constEnd())
        {
            QByteArray ba = (*it).toUtf8();
            if (layer->DeleteField(layer->FindFieldIndex(ba.data(),TRUE))
                    != OGRERR_NONE)
            {
                FBProject::CUR_ERR_INFO = QObject::tr("Unable to delete field in"
                                                      " a layer via GDAL");
                return FBErrGDALFail;
            }
            ++it;
        }
        fieldsDeleted.clear();
    }

    // Create new void fields, if the layer with this name in the general list of
    // fields is not in the layer already. We can do it because the names of fields
    // are unique.
    QMap<QString,FBFieldDescr>::const_iterator it = fields.constBegin();
    while (it != fields.constEnd())
    {
        QByteArray baKn;
        baKn = it.key().toUtf8();
        OGRFieldType ft = it.value().datataype->aliasMainGdal;
        // Note: here will be actual value truncation, e.g. if in the source dataset
        // the fild was of type integer64, but in the list of fields it is written
        // as the integer.
        if (layer->FindFieldIndex(baKn.data(),TRUE) == -1)
        {
            OGRFieldDefn fieldDefn(baKn.data(), ft);
            layer->CreateField(&fieldDefn);
        }
        ++it;
    }

    return FBErrNone;
}


// Transform coordinates (only first geometry column) of all features in a layer
// to the inner single SRS of the project.
FBErr FBProject::reprojectLayer (OGRLayer *layer)
{
    if (layer == NULL)
    {
        return FBErrNullVal;
    }

    QByteArray baSrs;
    baSrs = srs->strFullGdal.toUtf8(); // metadata is quaranteed inited
    OGRSpatialReference *srsNew = new OGRSpatialReference(baSrs.data());

    OGRSpatialReference *srsOld = layer->GetSpatialRef();

    if (srsOld != NULL && srsOld->IsSame(srsNew) == FALSE)
    {
        // We transform geometry of each feature, because there is no capability to
        // transform the whole layer at once (reason - for several datasets SRS
        // is stored in the separate file, e.g. for Shapefile).
        OGRCoordinateTransformation *transform
                = OGRCreateCoordinateTransformation(srsOld, srsNew);
        if (transform == NULL)
        {
            FBProject::CUR_ERR_INFO = QObject::tr("Unable to create GDAL transformation"
                   " object, while trying to reproject new layer's data");
            return FBErrGDALFail;
        }
        else
        {
            OGRFeature *feat;
            layer->ResetReading();
            while((feat = layer->GetNextFeature()) != NULL)
            {
                OGRGeometry *geom = feat->StealGeometry();
                if (geom != NULL
                    && geom->transform(transform) == OGRERR_NONE
                    && feat->SetGeometryDirectly(geom) == OGRERR_NONE
                    && layer->SetFeature(feat) == OGRERR_NONE)
                {
                    OGRFeature::DestroyFeature(feat);
                }
                else
                {
                    OGRFeature::DestroyFeature(feat);
                    FBProject::CUR_ERR_INFO = QObject::tr("Unable to reproject"
                               " geoometry of some feature in the new layer, while "
                               " trying to reproject new layer's data");
                    OCTDestroyCoordinateTransformation(
                                (OGRCoordinateTransformationH)transform);
                    return FBErrGDALFail;
                }
            }
            OCTDestroyCoordinateTransformation(
                        (OGRCoordinateTransformationH)transform);
        }
    }

    // We anyway need to change the general SRS of the layer.
    // It is necessary at least for thouse cases when the layer does not contain any
    // features (for example it is the first save of the void project).
    layer->GetLayerDefn()->GetGeomFieldDefn(0)->SetSpatialRef(srsNew);

    // TODO: if there are another geometry columns in a dataset - delete them.

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


FBErr FBProject::fillJsonFile (QFile &file, Json::Value json)
{
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to create file in"
                                     " a temporary directory");
        return FBErrTempFileFail;
    }
    Json::StyledWriter writer;
    std::string finalStdString = writer.write(json);
    QString finalString= QString::fromUtf8(finalStdString.data());
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out<<finalString;
    return FBErrNone;
}


bool FBProject::addFileToZip (const CPLString &szFilePathName,
    const CPLString &szFileName, void* hZIP, GByte **pabyBuffer, size_t nBufferSize)
{
    int nRet = 0;
    size_t nBytesRead;
    VSILFILE *fp;

    fp = VSIFOpenL(szFilePathName, "rb");
    if (fp == NULL)
    {
        return false;
    }

    // Write to ZIP metadata when the file starts and how it is called.
    if (CPLCreateFileInZip(hZIP, szFileName, NULL) != CE_None)
    {
        VSIFCloseL(fp);
        return false;
    }

    // Write data by blocks. Data is compressed in IO time.
    do
    {
        nBytesRead = VSIFReadL(*pabyBuffer, 1, nBufferSize, fp);
        if (long(nBytesRead) < 0)
            nRet = -1;
        if (nRet == 0 && CPLWriteFileInZip(hZIP, *pabyBuffer, nBytesRead) != CE_None)
            nRet = -1;
    }
    while (nRet == 0 && nBytesRead == nBufferSize);

    CPLCloseFileInZip(hZIP);
    VSIFCloseL(fp);

    return true;
}



