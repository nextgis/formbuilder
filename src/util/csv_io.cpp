/**************************************************************************************************
 *                                                                                                *
 *    Project:  NextGIS Formbuilder                                                               *
 *    Authors:  Mikhail Gusev, gusevmihs@gmail.com                                                *
 *              Copyright (C) 2014-2019 NextGIS                                                   *
 *                                                                                                *
 *    This program is free software: you can redistribute it and/or modify it under the terms     *
 *    of the GNU General Public License as published by the Free Software Foundation, either      *
 *    version 2 of the License, or (at your option) any later version.                            *
 *                                                                                                *
 *    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;   *
 *    without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   *
 *    See the GNU General Public License for more details.                                        *
 *                                                                                                *
 *    You should have received a copy of the GNU General Public License along with this program.  *
 *    If not, see http://www.gnu.org/licenses/.                                                   *
 *                                                                                                *
 **************************************************************************************************/

#include "csv_io.h"

#include <QObject>

using namespace Fb;
using namespace Util;


void CsvIo::read (QString file_path, QList<QStringList> &csv_contents)
{
    csv_contents.clear();

    QByteArray ba_path(file_path.toUtf8());

    char **allowed_drivers = NULL;
    allowed_drivers = CSLAddString(allowed_drivers, "CSV");

    GDALDatasetPtr dataset_p((GDALDataset*)GDALOpenEx(ba_path.data(),
                             GDAL_OF_VECTOR | GDAL_OF_READONLY,
                             allowed_drivers, NULL, NULL));

    CSLDestroy(allowed_drivers);

    if (dataset_p.data() == NULL)
        throw QObject::tr("Unable to open CSV dataset via GDAL");

    OGRLayer *layer = dataset_p.data()->GetLayer(0);
    if (layer == NULL)
        throw QObject::tr("Unable to read layer[0] in CSV dataset via GDAL");

    OGRFeatureDefn *feature_defn = layer->GetLayerDefn();
    int field_count = feature_defn->GetFieldCount();
    for (int i = 0; i < field_count; i++)
    {
        QString column_name(feature_defn->GetFieldDefn(i)->GetNameRef());
        QStringList column = {column_name};
        csv_contents.append(column);
    }

    OGRFeature *feature;
    layer->ResetReading();
    while((feature = layer->GetNextFeature()) != NULL)
    {
        for (int i = 0; i < field_count; i++)
        {
            QString str = QString::fromUtf8(feature->GetFieldAsString(i));
            csv_contents[i].append(str);
        }
        OGRFeature::DestroyFeature(feature);
    }
}



