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

#include "ogrsf_frmts.h"

#include <QObject>
#include <QMap>

namespace Fb
{
namespace Core
{


enum class SrsType
{
    Undefined,
    Epsg4326
};

struct SrsTypeData
{
    QString display_name;
    QString epsg_code;
    QString ngw_number;
    QString gdal_string;
};


inline const QMap<SrsType, SrsTypeData> &g_getSrsTypes ()
{
    static const QMap<SrsType, SrsTypeData> SRS_TYPES =
    {
        {{SrsType::Epsg4326}, {QObject::tr("WGS 84"), "EPSG:4326", "4326", {SRS_WKT_WGS84}}},

    };
    return SRS_TYPES;
}

inline QString g_getSrsTypeNgwNumber (SrsType key)
{
    auto data = g_getSrsTypes().value(key);
    return data.ngw_number;
}

inline SrsType g_findSrsType (QString ngw_number)
{
    for (auto &key: g_getSrsTypes().keys())
    {
        auto &data = g_getSrsTypes().value(key);
        if (QString::compare(data.ngw_number, ngw_number, Qt::CaseInsensitive) == 0)
            return key;
    }
    return SrsType::Undefined;
}


}
}

Q_DECLARE_METATYPE(Fb::Core::SrsType)



