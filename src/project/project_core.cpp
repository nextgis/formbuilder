/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  core project implementations
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

#include "project_core.h"

#include "projects.h"
 
QList<FbGeomType*> FBProject::GEOM_TYPES;
QList<FbDataType*> FBProject::DATA_TYPES;
QList<FbSrsType*> FBProject::SRS_TYPES;

QString FBProject::CUR_ERR_INFO = "";


// General method to "set environment".
// Call at the begining of the program to register all general types, initialize
// GDAL, etc.
void FBProject::initEnv () // STATIC
{
    // We need to register enum type for passing it as a parameter of signal/slot
    // pair through the threads (e.g. in FB class).
    qRegisterMetaType<FBErr>("FBErr");

    // Settings for GDAL.
    // We need GDAL in any project, even void, because we will at least use vsizip
    // for packaging to .ngfp and GeoJSON driver for layer creation.
    #ifdef FB_GDAL_DEBUG
    CPLSetConfigOption("CPL_DEBUG","ON");
    CPLSetConfigOption("CPL_CURL_VERBOSE","YES");
    CPLSetConfigOption("CPL_LOG",_FB_GDAL_DEBUG);
    CPLSetConfigOption("CPL_LOG_ERRORS","ON");
    #endif
    CPLSetConfigOption("CPL_VSIL_ZIP_ALLOWED_EXTENSIONS",FB_PROJECT_EXTENSION);
    #ifdef FB_GDALDATA_IN_SHARE
    QByteArray ba = QString(FB_PATH_GDALDATA).toUtf8();
    CPLSetConfigOption("GDAL_DATA", ba.data());
    #endif
    GDALAllRegister();

    // Geometry types.
    // Note: no default value for this list.
    // WARNING. All values must be unique, because they are searched and returned by
    // their first appearance in the list.
    GEOM_TYPES.append(new FbGeomType("POINT", wkbPoint,
                                     tr("Point")));
    GEOM_TYPES.append(new FbGeomType("LINESTRING", wkbLineString,
                                     tr("Line")));
    GEOM_TYPES.append(new FbGeomType("POLYGON", wkbPolygon,
                                     tr("Polygon")));
    GEOM_TYPES.append(new FbGeomType("MULTIPOINT", wkbMultiPoint,
                                     tr("Multipoint")));
    GEOM_TYPES.append(new FbGeomType("MULTILINESTRING", wkbMultiLineString,
                                     tr("Multiline")));
    GEOM_TYPES.append(new FbGeomType("MULTIPOLYGON", wkbMultiPolygon,
                                     tr("Multipolygon")));

    // Data types.
    // WARNING. The 0 and 1 list value (string and integer types) have its named
    // representations (its indexes are used in program).
    // WARNING. All values must be unique, because they are searched and returned by
    // their first appearance in the list.
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
    // WARNING. All values must be unique, because they are searched and returned by
    // their first appearance in the list.
    SRS_TYPES.append(new FbSrsType(4326, SRS_WKT_WGS84));
}


void FBProject::deinitEnv () // STATIC
{
    for (int i=0; i<GEOM_TYPES.size(); i++)
        delete GEOM_TYPES[i];
    for (int i=0; i<DATA_TYPES.size(); i++)
        delete DATA_TYPES[i];
    for (int i=0; i<SRS_TYPES.size(); i++)
        delete SRS_TYPES[i];
    GEOM_TYPES.clear();
    DATA_TYPES.clear();
    SRS_TYPES.clear();

    // TODO: do we need to deregister/reset smth for GDAL at the end of the program?
}


QString FBProject::getProgVersionStr () // STATIC
{
    return QString::number(FB_VERSION,'f',1);
}


FbGeomType *FBProject::findGeomTypeByNgw (QString aliasNgw) // STATIC
{
    for (int i=0; i<GEOM_TYPES.size(); i++)
        if (GEOM_TYPES[i]->aliasNgw == aliasNgw)
            return GEOM_TYPES[i];
    return NULL;
}
FbGeomType *FBProject::findGeomTypeByGdal (OGRwkbGeometryType aliasGdal) // STATIC
{
    for (int i=0; i<GEOM_TYPES.size(); i++)
        if (GEOM_TYPES[i]->aliasGdal == aliasGdal)
            return GEOM_TYPES[i];
    return NULL;
}
FbDataType *FBProject::findDataTypeByNgw (QString aliasNgw) // STATIC
{
    for (int i=0; i<DATA_TYPES.size(); i++)
        if (DATA_TYPES[i]->aliasNgw == aliasNgw)
            return DATA_TYPES[i];
    return NULL;
}
FbDataType *FBProject::findDataTypeByGdal (OGRFieldType aliasGdal) // STATIC
{
    for (int i=0; i<DATA_TYPES.size(); i++)
        if (DATA_TYPES[i]->aliasMainGdal == aliasGdal
                || DATA_TYPES[i]->aliasesOtherGdal.contains(aliasGdal))
            return DATA_TYPES[i];
    return NULL;
}
FbSrsType *FBProject::findSrsTypeByNgw (int numberNgw) // STATIC
{
    for (int i=0; i<SRS_TYPES.size(); i++)
        if (SRS_TYPES[i]->numberNgw == numberNgw)
            return SRS_TYPES[i];
    return NULL;
}


/****************************************************************************/
/*                                                                          */
/*                             FBProject                                    */
/*                                                                          */
/****************************************************************************/

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

    fieldsModified = false;
}


/****************************************************************************/
/*                                                                          */
/*                             Main I/O methods                             */
/*                                                                          */
/****************************************************************************/

// READ
FBErr FBProject::read (QString ngfpFullPath)
{
    if (isInited)
        return FBErrAlreadyInited;

    // Read 3 main components of project file.
    Json::Value jsonMeta = this->readMeta(ngfpFullPath);
    Json::Value jsonForm = this->readForm(ngfpFullPath);
    bool hasData = this->readData(ngfpFullPath);
    // TODO: read additional components: images.
    //...

    // The main checks if JSON files exist and are correct.
    if (jsonMeta.isNull() || jsonForm.isNull() || !hasData)
    {
        return FBErrIncorrectFileStructure;
    }

    // Compare version of this program and .ngfp file version.
    QString versFile = QString::fromUtf8(jsonMeta[FB_JSON_META_VERSION]
                                         .asString().data());
    QString versProg = FBProject::getProgVersionStr();
    if (versFile != versProg)
    {
        FBProject::CUR_ERR_INFO = QObject::tr("Project file is of version ")
                               + versFile + QObject::tr(", while the program is of "
                               "version ") + versProg;
        return FBErrWrongVersion;
    }

    // Set project's metadata.
    // Note: All metadata correctness (syntax and values) has been already checked.
    // Also the support for geometry/data/srs types of metadata is also already
    // checked, so we do not check for NULL values here.
    for (int k=0; k<jsonMeta[FB_JSON_META_FIELDS].size(); ++k)
    {
        Json::Value v = jsonMeta[FB_JSON_META_FIELDS][k];
        QString s1 = QString::fromUtf8(v[FB_JSON_META_DATATYPE].asString().data());
        FbDataType *dt1 = FBProject::findDataTypeByNgw(s1);
        QString s2 = QString::fromUtf8(v[FB_JSON_META_DISPLAY_NAME].asString().data());
        QString s3 = QString::fromUtf8(v[FB_JSON_META_KEYNAME].asString().data());
        fields.insert(s3,FBField(dt1,s2));
    }
    QString sgt = QString::fromUtf8(jsonMeta[FB_JSON_META_GEOMETRY_TYPE]
                                      .asString().data());
    geometry_type = FBProject::findGeomTypeByNgw(sgt);
    Json::Value v = jsonMeta[FB_JSON_META_NGW_CONNECTION];
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
    // = QString::fromUtf8(jsonMeta[FB_JSON_META_SRS][FB_JSON_META_ID]
                                //.asString().data()).toInt();
    version = versProg;

    strNgfpPath = ngfpFullPath;

    isInited = true;
    return FBErrNone;
}


// READ META
Json::Value FBProject::readMeta (QString ngfpFullPath) // STATIC
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
    // ...

    return jsonRet;
}


// READ FORM
Json::Value FBProject::readForm (QString ngfpFullPath) // STATIC
{
    Json::Value jsonRet;
    jsonRet = FBProject::readJsonInNgfp(ngfpFullPath,FB_PROJECT_FILENAME_FORM);
    if (jsonRet.isNull())
        return jsonRet;

    // TODO: check form for syntax errors:
    // - correct array structure
    // - correctness to have null values for: a) the whole "attributes",
    // b) attribute's value. For that try to read the necessery amount of attrs
    // and the necessary attr values as it is defined in the according FBAttr
    // class. See FBElem::fromJson() why we need this.
    // - more general: try to create an elem for the passed keyname: what if
    // the keyname of the elem had been changed manually and it is correct
    // while its structure belongs to the elem with another keyname?
    // ...

    return jsonRet;
}


// READ DATA
// Method does not return any data, just checks its correctness.
bool FBProject::readData (QString ngfpFullPath) // STATIC
{
    // TODO: check data file, firstly its existance.
    // - ...

    return true;
}


// WRITE
// Main saving method of the project.
// Method resaves each time the whole project file from the beginning because it
// is not possible to write only one file to the ZIP archive at a time - the whole
// archive must be resaved.
// Project file is a ZIP archive which contains 3 JSON/GeoJSON files: metadata,
// layer (features data) and form. Additionally it can contain images for form.
// All writing work is made inside temporary directory, and at the end the new
// project file is created replacing an old one if was some.
FBErr FBProject::write (QString ngfpFullPath, Json::Value jsonForm)
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
    QString strNameBase;
    QString strPath;
    this->getPathComponents(ngfpFullPath,strPath,strNameBase);
    QTemporaryDir dir(strPath + "formXXXXXX"); // only for Qt >= 5
    if (!dir.isValid())
    {
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to create temporary dir at ")
                + strPath + QObject::tr("\nTry to select another saving directory");
        return FBErrBadSavePath;
    }
    QString strTempPath;
    strTempPath = dir.path() + "/";

    // Form paths for obligatory project files in a temporary directory.
    QList<QPair<QString,QString> > strsTempFiles
            = this->getInnerFilePaths(strTempPath);

    // 1. Create metadata file.
    // Metadata is obtained from this project.
    QFile fileMeta(strsTempFiles[0].first);
    err = FBProject::writeJsonFile(fileMeta,this->getJsonMetadata());
    if (err != FBErrNone)
        return err;
    emit changeProgress(10);

    // 2. Create form file.
    // The form is usually obtained from the screen of the main app gui.
    // TODO: check the passed json correctness before writing! Use the common method
    // for it.
    QFile fileForm(strsTempFiles[1].first);
    err = FBProject::writeJsonFile(fileForm,jsonForm);
    if (err != FBErrNone)
        return err;
    emit changeProgress(30);

    // 3. Create layer's data file.
    // Layer's data is obtained from the old project file, but if there is no
    // old file the concrete project class must know how to get data first time.
    if (strNgfpPath != "") // open old GeoJSON dataset in ngfp file
    {
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
        err = this->writeDataFile(datasetOld,strsTempFiles[2].first);
        GDALClose(datasetOld);
        if (err != FBErrNone)
            return err;
    }
    else // concrete class knows how to get data
    {
        err = this->writeDataFileFirst(strsTempFiles[2].first);
        if (err != FBErrNone)
            return err;
    }
    emit changeProgress(75);

    // 4. (Additional) Get images. Add them to the files list.
    // ...
    emit changeProgress(85);

    // Create ZIP archive in the same temp directory.
    // Write created files to it.
    QString ngfpTempPath = strTempPath + strNameBase + "." + FB_PROJECT_EXTENSION;
    err = this->makeZip(ngfpTempPath,strsTempFiles);
    if (err != FBErrNone)
        return err;
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


// WRITE
FBErr FBProject::write (Json::Value jsonForm)
{
    if (strNgfpPath == "")
        return FBErrNoNgfp;
    return this->write(strNgfpPath,jsonForm);
}


// WRITE DATA
// This method is needed separately of write() method, because we should not
// initiate the resaving of the whole file when we just need to change its data
// (i.e. we do not need here operations like saving form or fields structure changes).
FBErr FBProject::writeData (QString shapefilePath)
{
    if (!isInited)
        return FBErrNotInited;
    if (strNgfpPath == "") // otherwise no form and meta files created yet
        return FBErrNoNgfp;

    FBErr err;
    emit changeProgress(0);

    // Try to initialize side project from passed Shapefile, checking all
    // data/metadata correctness.
    FBProjectShapefile *projOther = new FBProjectShapefile();
    err = projOther->readFirst(shapefilePath);
    if (err != FBErrNone)
    {
        delete projOther;
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to initialize project from"
                                 " selected Shapefile");
        return err;
    }
    emit changeProgress(10);

    // TEMPORARY: Compare field lists of two projects and if they differ - return
    // error.
    // FOR FUTURE: Form the listy of the fields of this project, which were abdated.
    QMap<QString,FBField> fieldsOther = projOther->getFields();
    if (fields != fieldsOther)
    {
        delete projOther;
        FBProject::CUR_ERR_INFO = QObject::tr("Selected Shapefile has the fields"
             " structure which differs from the current project's one");
        return FBErrStructureDiffers;
    }

    // We do not need project any more, only Shapefile dataset itself.
    delete projOther;
    emit changeProgress(15);

    // Get path components and create temporary dir for work.
    // This directory with all its content will be automatically deleted when this
    // method will end its work.
    QString strNameBase;
    QString strPath;
    this->getPathComponents(strNgfpPath,strPath,strNameBase);
    QTemporaryDir dir(strPath + "formXXXXXX"); // only for Qt >= 5
    if (!dir.isValid())
    {
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to create temporary dir at ")
                + strPath + QObject::tr("\nTry to select another saving directory");
        return FBErrBadSavePath;
    }
    QString strTempPath;
    strTempPath = dir.path() + "/";

    // Form paths for obligatory project files in a temporary directory.
    QList<QPair<QString,QString> > strsTempFiles
            = this->getInnerFilePaths(strTempPath);

    // 1 and 2. Copy meta and form from current project.
    Json::Value jsonMeta = FBProject::readMeta(strNgfpPath);
    QFile fileMeta(strsTempFiles[0].first);
    err = FBProject::writeJsonFile(fileMeta,jsonMeta);
    if (err != FBErrNone)
        return err;
    Json::Value jsonForm = FBProject::readForm(strNgfpPath);
    QFile fileForm(strsTempFiles[1].first);
    err = FBProject::writeJsonFile(fileForm,jsonForm);
    if (err != FBErrNone)
        return err;
    emit changeProgress(45);

    // 3. Get data from the side project and write it to the temp directory.
    // The data will be also translated to the inner SRS if needed.
    // No fields modyfications will be done - only data copying.
    QByteArray baShapePath;
    baShapePath = shapefilePath.toUtf8();
    char **allowedDrivers = NULL;
    allowedDrivers = CSLAddString(allowedDrivers,"ESRI Shapefile");
    GDALDataset *datasetShape = (GDALDataset*) GDALOpenEx(baShapePath.data(),
            GDAL_OF_VECTOR | GDAL_OF_READONLY, allowedDrivers, NULL, NULL);
    CSLDestroy(allowedDrivers);
    if (datasetShape == NULL)
    {
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to open selected Shapefile"
                     " using GDAL");
        return FBErrGDALFail;
    }
    err = this->writeDataFile(datasetShape,strsTempFiles[2].first,true); // ignore field modif-s
    GDALClose(datasetShape);
    if (err != FBErrNone)
        return err;
    emit changeProgress(65);

    // 4. (Additional) Copy Image files. Add them to the files list.
    // ...
    emit changeProgress(80);

    // Form new ngfp file in the temp directory.
    QString ngfpTempPath = strTempPath + strNameBase + "." + FB_PROJECT_EXTENSION;
    err = this->makeZip(ngfpTempPath,strsTempFiles);
    if (err != FBErrNone)
        return err;
    emit changeProgress(90);

    // Replace the curent ngfp file with new one.
    if (QFile::exists(strNgfpPath) && !QFile::remove(strNgfpPath))
    {
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to remove old project file"
                                              " with the same name as the saved one");
        return FBErrTempFileFail;
    }
    if (!QFile::copy(ngfpTempPath, strNgfpPath))
    {
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to copy final project file from"
                                 " the temporary directory to the selected one");
        return FBErrTempFileFail;
    }
    emit changeProgress(99);

    // NOTE: after all we have the same ngfp file as it was before this operation,
    // but with another data.geojson file. If there is any changes in the project
    // hanging - e.g. among fields structure or form controls - they must be saved
    // in a common way.
    return FBErrNone;
}


/****************************************************************************/
/*                                                                          */
/*                             Modify methods                               */
/*                                                                          */
/****************************************************************************/

// Modify fields in metadata.
// IMPORTANT: Real data fields will be GUARANTEED created and/or deleted during the
// work of write() method.
// 1) Fully reset list of fields for metadata;
// 2) Update list of fields, which must be physically deleted during the next saveAs()
// method;
// WARNING: in this method we do not check correspondance of two passed lists, use this
// carefully.
// This method is used in specific situation: for working with field's manager dialog.
void FBProject::modifyFields (QMap<QString,FBField> newSetOfFields,
                              QSet<QString> fieldsWereDeleted)
{
    this->fields = newSetOfFields;
    this->fieldsDeleted.unite(fieldsWereDeleted);
    fieldsModified = true;
}


/****************************************************************************/
/*                                                                          */
/*                             Info methods                                 */
/*                                                                          */
/****************************************************************************/

// Convert metadata of the project to the JSON format.
Json::Value FBProject::getJsonMetadata ()
{
    Json::Value jsonRet;

    Json::Value jsonFields;
    Json::Value jsonGeom;
    Json::Value jsonNgw;
    Json::Value jsonSrs;
    Json::Value jsonVers;

    QMap<QString,FBField>::const_iterator it = fields.constBegin();
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

    QByteArray baSrs;
    baSrs = QString::number(srs->numberNgw).toUtf8();
    jsonSrs[FB_JSON_META_ID] = baSrs.data();

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


bool FBProject::isSaveRequired ()
{
    // ...

    return true;
}


// Get path components.
// Puts into the passed parameters the component of the full path to the project
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


/****************************************************************************/
/*                                                                          */
/*                           Inner I/O methods                              */
/*                                                                          */
/****************************************************************************/

// Copy data file (layer) first time.
// Default implementation with no initial dataset.
FBErr FBProject::writeDataFileFirst (QString strPath)
{
    // FOR NOW USE OLD CODE HERE.
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
    FBErr err = this->writeDataFile(datasetStub, strPath);
    GDALClose(datasetStub);
    if (err != FBErrNone)
        return err;

    return FBErrNone;
}


// Copy one layer from passed GDAL dataset (any) to the newly created GeoJSON
// file (layer) in the passed directory (any).
FBErr FBProject::writeDataFile (GDALDataset *datasetSrcPtr, QString strPath,
                                bool ignoreFieldsModifs)
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
    // the passed directory.
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
    // because: a) the source layer must not be modified! b) GeoJSON GDAL driver
    // does not support some layer operations, e.g. field's deletion.
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

    // Start layer modification.

    // 1. Change the structure of fields.
    // All checks will be done inside the method.
    if (!ignoreFieldsModifs)
    {
        err = this->modifyFieldsOfLayer(layerTemp);
        if (err != FBErrNone)
        {
            GDALClose(datasetTemp);
            GDALClose(datasetNew);
            return err;
        }
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


Json::Value FBProject::readJsonInNgfp (QString ngfpFullPath, QString fileName) // STATIC
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


FBErr FBProject::writeJsonFile (QFile &file, Json::Value json)
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


/****************************************************************************/
/*                                                                          */
/*                         Some common actions                              */
/*                                                                          */
/****************************************************************************/

// Return obligatory project file paths in a strict order.
// All, that will be written to ngfp file must starts from these files - i.e.
// 1st, 2nd and 3d files are: meta, form and data.
// Returned list: first = path to temp file in a temp dir, second = future name
// in zip (ngfp).
QList<QPair<QString,QString> > FBProject::getInnerFilePaths (QString tempPath)
{
    QList<QPair<QString,QString> > strsTempFiles;
    QString strMetaPath = tempPath + FB_PROJECT_FILENAME_META;
    QString strFormPath = tempPath + FB_PROJECT_FILENAME_FORM;
    QString strDataPath = tempPath + FB_PROJECT_FILENAME_DATA;
    strsTempFiles.append(QPair<QString,QString>(strMetaPath,FB_PROJECT_FILENAME_META));
    strsTempFiles.append(QPair<QString,QString>(strFormPath,FB_PROJECT_FILENAME_FORM));
    strsTempFiles.append(QPair<QString,QString>(strDataPath,FB_PROJECT_FILENAME_DATA));
    return strsTempFiles;
}


// Make final ngfp file (assumed in a temp directory).
// Assemble ZIP archive from passed list of files, first 3 of which are obligatory
// (see according method).
FBErr FBProject::makeZip (QString ngfpTempPath,
                          QList<QPair<QString,QString> > strsTempFiles)
{
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


// Guaranteed modify real structure of GDAL-layer's fields.
FBErr FBProject::modifyFieldsOfLayer (OGRLayer *layer)
{
    if (layer == NULL)
    {
        return FBErrNullVal;
    }

    // TODO: check if OGRLayer is able to delete fields.
    // Currently only Memory layer is passed to this method, so there is no actual
    // need to do this.

    // Delete fields with their data which were marked as deleted.
    if (fieldsDeleted.size() > 0)
    {
        QSet<QString>::const_iterator it = fieldsDeleted.constBegin();
        while (it != fieldsDeleted.constEnd())
        {
            QByteArray ba = (*it).toUtf8();
            int index = layer->FindFieldIndex(ba.data(),TRUE);
            if (index == -1)
            {
                ++it;
                continue; // means that the deleted field was created in program
            }
            if (layer->DeleteField(index) != OGRERR_NONE)
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
    QMap<QString,FBField>::const_iterator it = fields.constBegin();
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

    fieldsModified = false; // guaranteed no fields modifyings for now

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
                               " geometry of some feature in the new layer, while "
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



