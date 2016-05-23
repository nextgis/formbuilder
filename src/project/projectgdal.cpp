/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  GDAL project implementations
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
 

FBProjectGdal::FBProjectGdal ():
    FBProject()
{
    strDatasetPath = "";
}

FBProjectGdal::~FBProjectGdal ()
{
}


FBErr FBProjectGdal::readFromDataset (QString datasetPath)
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
        return FBErrIncorrectGdalDataset_NotForNgw;
    }

    OGRSpatialReference *layerSpaRef = layer->GetSpatialRef();
    if (layerSpaRef == NULL)
    {
        GDALClose(dataset);
        FBProject::CUR_ERR_INFO = QObject::tr("Selected GDAL dataset does not contain"
              " its spatial reference system description");
        return FBErrIncorrectGdalDataset;
    }

    // All checks were made and we can fill the rest metadata of the project, which
    // can be read via GDAL.

    OGRwkbGeometryType geomType = layerDefn->GetGeomType();
    FbGeomType *gt = FBProject::findGeomTypeByGdal(geomType);
    if (gt == NULL)
    {
        // No default value for geometry type. NextGIS Mobile will not be able
        // to read "undefined" geometry. So rise error here.

        // TODO: do not rise error for GDAL types which can be translated, such as
        // wkbPolygon25D to simple Polygon.

        GDALClose(dataset);
        FBProject::CUR_ERR_INFO = QObject::tr("Selected GDAL dataset has unsupported"
                          " geometry type: ") + OGRGeometryTypeToName(geomType);
        return FBErrIncorrectGdalDataset_NotForNgw;
    }
    geometry_type = gt;

    for (int i=0; i<layerDefn->GetFieldCount(); i++)
    {
        FBField descr;
        OGRFieldDefn *fieldDefn = layerDefn->GetFieldDefn(i);
        OGRFieldType fieldType = fieldDefn->GetType();
        FbDataType *dt = FBProject::findDataTypeByGdal(fieldType);
        if (dt == NULL)
        {
            // Default datatype.
            // Data will be converted to string during the writing into JSON file.
            descr.datataype = DATA_TYPES[FB_TYPEINDEX_DATA_STRING];
        }
        else
        {
            descr.datataype = dt;
        }
        descr.display_name = QString::fromUtf8(fieldDefn->GetNameRef());
        fields.insert(QString::fromUtf8(fieldDefn->GetNameRef()), descr);
    }

    GDALClose(dataset);

    return FBErrNone;
}


FBErr FBProjectGdal::writeDataFileFirst (QString strPath)
{
    // Open existing GDAL dataset.
    // NOTE: we do not need do set specific GDAL opening settings, while it was
    // done during the creation of this project IN THIS SESSION of the program
    // (otherwise this method just will have not be called).
    QByteArray ba = strDatasetPath.toUtf8();
    GDALDataset *datasetSrc = (GDALDataset*) GDALOpenEx(ba.data(),
                                        GDAL_OF_VECTOR, NULL, NULL, NULL);
    if (datasetSrc == NULL)
    {
        FBProject::CUR_ERR_INFO = QObject::tr("Unable to open source dataset to"
                                              " copy its layer");
        return FBErrGDALFail;
    }

    // Copy layer from the source dataset to the GeoJSON new dataset of the .ngfp
    // file. All fields and geometry changes will be also done inside this method.
    FBErr err = this->writeDataFile(datasetSrc, strPath);
    GDALClose(datasetSrc);
    if (err != FBErrNone)
        return err;

    return FBErrNone;
}

