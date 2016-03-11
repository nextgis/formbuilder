/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  project declarations
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

// GLOBAL NOTE:
// 1) Do not use GDAL < 2.1.0 in program at all because of vsizip bug, see
// https://trac.osgeo.org/gdal/ticket/6005
// 2) Do not use Qt < 5, at least because the QTemporaryDir class is used in
// program.

#include <QObject>

#include <QList>
#include <QMap>
#include <QSet>

#include <QString>

#include <QFile>

#include "ogrsf_frmts.h"
#include "json/json.h"

// TODO: set these constants via build system:
#define _FB_VERSION 2.0
#define _FB_GDAL_DEBUG "D:/nextgis/formbuilder/gdal-log.txt" // uncomment for debugging
#define _FB_INSTALLPATH_GDALDATA "/gdal_data"

// Names.
#define FB_PROJECT_EXTENSION "ngfp"
#define FB_PROJECT_FILENAME_FORM "form.json"
#define FB_PROJECT_FILENAME_DATA "data.geojson"
#define FB_PROJECT_FILENAME_META "meta.json"

// Keys for JSON.
// Change this carefully, or do not change this in future versions! These keys are
// written in older versions of .ngfp files.
#define FB_JSON_META_VERSION "version"
#define FB_JSON_META_FIELDS "fields"
#define FB_JSON_META_KEYNAME "keyname"
#define FB_JSON_META_DATATYPE "datatype"
#define FB_JSON_META_DISPLAY_NAME "display_name"
#define FB_JSON_META_GEOMETRY_TYPE "geometry_type"
#define FB_JSON_META_NGW_CONNECTION "ngw_connection"
#define FB_JSON_META_SRS "srs"
#define FB_JSON_META_ID "id"
#define FB_JSON_META_LOGIN "login"
#define FB_JSON_META_PASSWORD "password"
#define FB_JSON_META_URL "url"

#define FB_TYPEINDEX_DATA_STRING 0
#define FB_TYPEINDEX_DATA_INTEGER 1
#define FB_TYPEINDEX_SRS_WGS84 0

// Error codes.
enum FBErr
{
    FBErrNone,

    // FOR DEVELOPERS: add string representations of new error codes to the
    // FB class.
    FBErrWrongVersion, FBErrIncorrectJson, FBErrIncorrectFileStructure,
    FBErrIncorrectGdalDataset, FBErrWrongSavePath, FBErrTempFileFail,
    FBErrGDALFail, FBErrCPLFail, FBErrReadNgfpFail,

    // For the following codes it is not necessary to show detailed error info.
    // WARNING. It is always necessary to check for this type when displaying error
    // in GUI because the old info can still hang in the global error variable.
    FBErrNullVal, FBErrNotInited, FBErrAlreadyInited, FBErrUnsupported,
};

// Types correspondance.
struct FbGeomType
{
    QString aliasNgw;
    OGRwkbGeometryType aliasGdal;
// QList<OGRwkbGeometryType> aliasesOtherGdal; // Polygon = Polygon25, Polygon3D, ...
    FbGeomType (QString an, OGRwkbGeometryType ad)
        { aliasNgw = an; aliasGdal = ad; }
    ~FbGeomType () { }
};
struct FbDataType
{
    QString aliasNgw;
    OGRFieldType aliasMainGdal;
    QList<OGRFieldType> aliasesOtherGdal;
    FbDataType (QString an, OGRFieldType amg)
        { aliasNgw = an; aliasMainGdal = amg; }
    FbDataType (QString an, OGRFieldType amg, QList<OGRFieldType> aog)
        { aliasNgw = an; aliasMainGdal = amg; aliasesOtherGdal = aog; }
    ~FbDataType () { }
};
struct FbSrsType
{
    int numberNgw;
    QString strFullGdal;
    FbSrsType (int nn, QString sfg)
        { numberNgw = nn; strFullGdal = sfg; }
    ~FbSrsType () { }
};

// Main metadata structures.
struct FBField
{ // see NextGISWeb fields description syntax
     FbDataType *datataype;
     QString display_name;
     FBField () { datataype = NULL; }
     FBField (FbDataType *dt, QString dn)
        { datataype = dt; display_name = dn; }
     //~FBFieldDescr();
     bool isEqual (FBField other) // the same as operator ==
        { return other.datataype == datataype
                && other.display_name == display_name; }
};
struct FBNgwConnection
{
    int id; // = -1 if no NGW connection
    QString login;
    QString password;
    QString url;
    FBNgwConnection () {}
    FBNgwConnection (int id, QString lg, QString ps, QString ul)
        { this->id = id; login = lg, password = ps; url = ul; }
    //~FBNgwConnection();
};


/**
 * Project class which represents its file on disk (.ngfp file).
 * Project file contains 1) form, 2) layer (data) and 3) metadata.
 * Note: this class does not keep the layer's data in memory (it
 * will be accessed on disk each time) and the form itself (it is owned
 * by the screen widget), but it is able to read/write them into project
 * file.
 *
 * Concrete project types differ from each other only in creation and first
 * saving capabilities, because other work with project is just working with
 * .ngfp files - which is always the same and implemented in base class.
 */
class FBProject: public QObject
{
    Q_OBJECT // only for showing progress for long operations

    //Q_ENUMS(FBErr)

    public: // static global members

     // General lists of types correspondance.
     // WARNING. Only for reading. Define new types in init() method.
     static QList<FbGeomType*> GEOM_TYPES;
     static QList<FbDataType*> DATA_TYPES;
     static QList<FbSrsType*> SRS_TYPES;

     static QString CUR_ERR_INFO; // additional error text if was some

     static void init ();
     static void deinit ();

     static QString getProgVersionStr ();

     static FbGeomType *findGeomTypeByNgw (QString aliasNgw);
     static FbGeomType *findGeomTypeByGdal (OGRwkbGeometryType aliasGdal);
     static FbDataType *findDataTypeByNgw (QString aliasNgw);
     static FbDataType *findDataTypeByGdal (OGRFieldType aliasGdal);
     static FbSrsType *findSrsTypeByNgw (int numberNgw);

     static void getPathComponents (QString strFullPath, QString &strPath,
                             QString &strNameBase);

    public: // methods

     FBProject ();
     virtual ~FBProject ();

     // main methods
     virtual FBErr create (QString anyPath) { return FBErrUnsupported; }
     FBErr saveAs (QString ngfpFullPath, Json::Value jsonForm);
     FBErr open (QString ngfpFullPath);
     static Json::Value readForm (QString ngfpFullPath);
     static Json::Value readMeta (QString ngfpFullPath);
     static bool checkData (QString ngfpFullPath);
     void resetFields (QMap<QString,FBField> fields) { this->fields = fields; }
     void expandFieldsDeleted (QSet<QString> fieldsDeleted);

     // info
     Json::Value getJsonMetadata ();
     bool wasInited () { return isInited; }
     bool wasFirstSaved ();
     QString getCurrentNgfpPath () { return strNgfpPath; }
     bool isSaveRequired ();
     QString getProjectfilePath () { return strNgfpPath; }
     virtual QString getDatasetPath () { return ""; }
     QMap<QString,FBField> getFields () { return fields; }

    signals:

     void changeProgress (int percentage); // amount of work completed for long actions

    protected: // methods

     virtual FBErr createDataFileFirst (QString strPath);
     FBErr copyDataFile (GDALDataset *datasetSrcPtr, QString strPath);
     FBErr modifyFieldsOfLayer (OGRLayer *layer);
     FBErr reprojectLayer (OGRLayer *layer);

    protected: // fields available for all project types
     
     bool isInited; // whether the project had been already created or opened. If the
                    // project is inited - it MUST HAVE ALL ITS METADATA SET
     // Current state and link to the disk representation - i.e. ngfp file.
     QString strNgfpPath; // if path is void - project needs to be saved first time

     // project's images
//     QList<QImage> images;

     // project's metadata
     QMap<QString,FBField> fields; // keyname is a unique key in the map
     FbGeomType *geometry_type;
     FBNgwConnection ngw_connection; // it is here because of ngfp file syntax
     FbSrsType *srs;
     QString version;

     QSet<QString> fieldsDeleted; // keynames of fields

    private: // methods with common actions

     static Json::Value readJsonInNgfp (QString ngfpFullPath, QString fileName);
     static FBErr fillJsonFile (QFile &file, Json::Value json);
     static bool addFileToZip (const CPLString &szFilePathName, const CPLString
                   &szFileName, void* hZIP, GByte **pabyBuffer, size_t nBufferSize);
};

class FBProjectGDAL: public FBProject
{
    public:
     FBProjectGDAL ();
     virtual ~FBProjectGDAL ();
     QString getDatasetPath () { return strDatasetPath; }
    protected:
     FBErr setFromGdalDataset (QString datasetPath);
     FBErr createDataFileFirst (QString strPath);
    protected:
     QString strDatasetPath;
};

class FBProjectVoid: public FBProject
{
    public:
     FBProjectVoid (FbGeomType *geomType);
     virtual ~FBProjectVoid ();
     virtual FBErr create (QString anyPath);
     virtual QString getDatasetPath () { return QObject::tr("no dataset"); }
};

class FBProjectShapefile: public FBProjectGDAL
{
    public:
     FBProjectShapefile ();
     ~FBProjectShapefile ();
     FBErr create (QString anyPath);
};

class FBProjectNgw: public FBProjectGDAL
{
    public:
     FBProjectNgw (QString strUrl, QString strLogin,
                   QString strPass, int nId, Json::Value jsonMeta);
     ~FBProjectNgw ();
     FBErr create (QString anyPath);
    private:
     Json::Value jsonTempMeta; // only for creation step
};


#endif //PROJECT_H
