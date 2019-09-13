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


enum class FieldType
{
    Undefined,

    String,
    Integer,
    BigInteger,
    Real,
    Date,
    Time,
    DateTime
};

struct FieldTypeData
{
    QString display_name;
    QString ngw_name;
    OGRFieldType gdal_type;
};


inline const QMap<FieldType, FieldTypeData> &g_getFieldTypes ()
{
    static const QMap<FieldType, FieldTypeData> FIELD_TYPES =
    {
        {{FieldType::String},    {QObject::tr("String"),     "STRING",   OFTString}},
        {{FieldType::Integer},   {QObject::tr("Integer"),    "INTEGER",  OFTInteger}},
        {{FieldType::BigInteger},{QObject::tr("Big integer"),"BIGINT",   OFTInteger64}},
        {{FieldType::Real},      {QObject::tr("Real"),       "REAL",     OFTReal}},
        {{FieldType::Date},      {QObject::tr("Date"),       "DATE",     OFTDate}},
        {{FieldType::Time},      {QObject::tr("Time"),       "TIME",     OFTTime}},
        {{FieldType::DateTime},  {QObject::tr("Date-time"),  "DATETIME", OFTDateTime}}
    };
    return FIELD_TYPES;
}

inline QString g_getFieldTypeDispName (FieldType key)
{
    auto data = g_getFieldTypes().value(key);
    return data.display_name;
}

inline QString g_getFieldTypeNgwName (FieldType key)
{
    auto data = g_getFieldTypes().value(key);
    return data.ngw_name;
}

inline FieldType g_findFieldType (QString ngw_name)
{
    for (auto &key: g_getFieldTypes().keys())
    {
        auto &data = g_getFieldTypes().value(key);
        if (QString::compare(data.ngw_name, ngw_name, Qt::CaseInsensitive) == 0)
            return key;
    }
    return FieldType::Undefined;
}

inline FieldType g_findFieldType (int index)
{
    const auto &keys = g_getFieldTypes().keys();
    if (index < 0 || index >= keys.size())
        return FieldType::Undefined;
    return keys[index];
}

inline OGRFieldType g_findFieldTypeGdal (QString ngw_name)
{
    for (auto &field_type: g_getFieldTypes())
    {
        if (QString::compare(field_type.ngw_name, ngw_name, Qt::CaseInsensitive) == 0)
            return field_type.gdal_type;
    }
    return OFTString; // TODO: ok?
}

inline QString g_findFieldTypeNgw (OGRFieldType gdal_type)
{
    for (auto &field_type: g_getFieldTypes())
    {
        if (gdal_type == field_type.gdal_type)
            return field_type.ngw_name;
    }
    return "";
}


}
}

Q_DECLARE_METATYPE(Fb::Core::FieldType)



