/**************************************************************************************************
 *                                                                                                *
 *    Project:  NextGIS Formbuilder                                                               *
 *    Authors:  Mikhail Gusev, gusevmihs@gmail.com                                                *
 *              Copyright (C) 2014-2018 NextGIS                                                   *
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

#pragma once

#include <QObject>
#include <QMap>

namespace Fb
{
namespace Util
{


enum class NgwResourceType;

struct NgwResourceData
{
    int id;
    NgwResourceType cls;
    QString display_name;
    bool children;
};

struct NgwFileInfo
{
    QString id;
    QString mime_type;
    QString name;
    int size;
};

struct NgwLayerInfo
{
    QString name;
    QString geom_type;
    QList<std::tuple<QString, QString, QString>> fields; // tuple: keyname, display_name, type
};


enum class NgwResourceType
{
    Undefined,
    ResourceGroup,
    VectorLayer,
    PostgisLayer,
};

struct NgwResourceTypeData
{
    QString ngw_key;
    QString display_name;
    QString icon_file_path;
};

const QMap<NgwResourceType, NgwResourceTypeData> NGW_RESOURCE_TYPES =
{
    {{NgwResourceType::ResourceGroup},
     {
         "resource_group",
         QObject::tr("Resource group"),
         ":/images/ngw/ngw_group.png"
     }},
    {{NgwResourceType::VectorLayer},
     {
         "vector_layer",
         QObject::tr("Vector layer"),
         ":/images/ngw/ngw_vector.png"
     }},
    {{NgwResourceType::PostgisLayer},
     {
         "postgis_layer",
         QObject::tr("PostGIS layer"),
         ":/images/ngw/ngw_postgis.png"
     }},
};


inline QString g_getNgwResTypeDispName (NgwResourceType key)
{
    if (!NGW_RESOURCE_TYPES.contains(key))
        return QObject::tr("<unknown>");
    auto data = NGW_RESOURCE_TYPES.value(key);
    return data.display_name;
}

inline QString g_getNgwResTypeIconPath (NgwResourceType key)
{
    if (!NGW_RESOURCE_TYPES.contains(key))
        return "";
    auto data = NGW_RESOURCE_TYPES.value(key);
    return data.icon_file_path;
}

inline NgwResourceType g_findNgwResType (QString ngw_key)
{
    for (auto key: NGW_RESOURCE_TYPES.keys())
    {
        auto data = NGW_RESOURCE_TYPES.value(key);
        if (data.ngw_key == ngw_key)
            return key;
    }
    return NgwResourceType::Undefined;
}


}
}

Q_DECLARE_METATYPE(Fb::Util::NgwResourceType)



