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

#include "core/attr.h"

#include <QDateTime>

namespace Fb
{
namespace Core
{


const QDateTime ATTR_DATETIME_DEF_INIT = {{2000, 1, 31}, {0, 0, 0}};

class DateTime: public Attr
{
    Q_OBJECT

    public:

     explicit DateTime (QString key_name, QDateTime initial = ATTR_DATETIME_DEF_INIT,
                        AttrInputType input_type = AttrInputType::DateTime);
     virtual ~DateTime ();

     QVariant getValueAsVar () const;
     const QDateTime &getValue () const;
     void setValue (const QDateTime &new_value);

     inline QString getFormat () const { return format; }
     inline void setFormat (QString new_format) { format = new_format; }

    protected:

     QDateTime value;
     QString format;
};


}
}
