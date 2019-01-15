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

#pragma once

#include "ogrsf_frmts.h"

#include <QObject>
#include <QMap>

namespace Fb
{
namespace Core
{


enum class GeomType
{
    Undefined,

    Point,
    Line,
    Polygon,
    Multipoint,
    Multiline,
    Multipolygon
};

struct GeomTypeData
{
    QString display_name;
    QString ngw_name;
    OGRwkbGeometryType gdal_type;
};


inline const QMap<GeomType, GeomTypeData> &g_getGeomTypes ()
{
    static const QMap<GeomType, GeomTypeData> GEOM_TYPES =
    {
        {{GeomType::Point},       {QObject::tr("Point"),       "POINT",          wkbPoint}},
        {{GeomType::Line},        {QObject::tr("Line"),        "LINESTRING",     wkbLineString}},
        {{GeomType::Polygon},     {QObject::tr("Polygon"),     "POLYGON",        wkbPolygon}},
        {{GeomType::Multipoint},  {QObject::tr("Multipoint"),  "MULTIPOINT",     wkbMultiPoint}},
        {{GeomType::Multiline},   {QObject::tr("Multiline"),   "MULTILINESTRING",wkbMultiLineString}},
        {{GeomType::Multipolygon},{QObject::tr("Multipolygon"),"MULTIPOLYGON",   wkbMultiPolygon}}
    };
    return GEOM_TYPES;
}

inline QString g_getGeomTypeDispName (GeomType key)
{
    auto data = g_getGeomTypes().value(key);
    return data.display_name;
}

inline QString g_getGeomTypeNgwName (GeomType key)
{
    auto data = g_getGeomTypes().value(key);
    return data.ngw_name;
}

inline GeomType g_findGeomType (QString ngw_name)
{
    for (auto &key: g_getGeomTypes().keys())
    {
        auto &data = g_getGeomTypes().value(key);
        if (QString::compare(data.ngw_name, ngw_name, Qt::CaseInsensitive) == 0)
            return key;
    }
    return GeomType::Undefined;
}

inline OGRwkbGeometryType g_findGeomTypeGdal (QString ngw_name)
{
    for (auto &geom_type: g_getGeomTypes())
    {
        if (QString::compare(geom_type.ngw_name, ngw_name, Qt::CaseInsensitive) == 0)
            return geom_type.gdal_type;
    }
    return wkbUnknown;
}


}
}

Q_DECLARE_METATYPE(Fb::Core::GeomType)



