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

namespace Fb
{
namespace Core
{


const int ATTR_STRING_DEF_MAX_LENGTH = 65536;

class String: public Attr
{
    Q_OBJECT

    public:

     explicit String (QString key_name, QString init_value,
                      int max_length = ATTR_STRING_DEF_MAX_LENGTH,
                      AttrInputType input_type = AttrInputType::String);
     virtual ~String ();

     virtual QVariant getValueAsVar () const override;
     virtual void setValueAsVar (const QVariant &new_value) override;

     QString getValue () const;
     void setValue (const QString &new_value);

     inline int getMaxLength () const { return max_length; }

    protected:

     QString value;
     int max_length;
};


}
}
