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

#include "field_types.h"

#include <QObject>
#include <QMap>

namespace Fb
{
namespace Core
{


struct DateTimeTypeData
{
    QString display_name;
    QString common_format;
    QString display_format;
    FieldType field_type;
};


inline const QList<DateTimeTypeData> &g_getDateTimeTypes ()
{
    static const QList<DateTimeTypeData> DATETIME_TYPES = // the order is important!
    {
        {QObject::tr("Date"),
        "yyyy.MM.dd",
        QObject::tr("yyyy.MM.dd"),
        FieldType::Date},

        {QObject::tr("Time"),
        "HH:mm:ss",
        QObject::tr("HH:mm:ss"),
        FieldType::Time},

        {QObject::tr("Date & time"),
        "yyyy.MM.dd HH:mm:ss",
        QObject::tr("yyyy.MM.dd HH:mm:ss"),
        FieldType::DateTime}
    };
    return DATETIME_TYPES;
}

inline QStringList g_getDateTimeTypesStringList ()
{
    QStringList list;
    for (auto &dt: g_getDateTimeTypes())
    {
        list.append(dt.display_name);
    }
    return list;
}

inline QString g_getDateTimeCommonFormat (int index)
{
    if (index < 0 || index >= g_getDateTimeTypes().size())
        return QString();
    return g_getDateTimeTypes()[index].common_format;
}

inline FieldType g_getDateTimeFieldType (int index)
{
    if (index < 0 || index >= g_getDateTimeTypes().size())
        return FieldType::Undefined;
    return g_getDateTimeTypes()[index].field_type;
}


}
}



