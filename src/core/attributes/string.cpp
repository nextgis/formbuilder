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

#include "string.h"

using namespace Fb;
using namespace Core;


String::String (QString key_name, QString init_value, int max_length, AttrInputType input_type):
    Attr(key_name, input_type),
    max_length(max_length)
{
    this->setValue(init_value);
}

String::~String ()
{
}


QVariant String::getValueAsVar () const
{
    return value;
}

void String::setValueAsVar (const QVariant &new_value)
{
    setValue(new_value.toString());
}


QString String::getValue () const
{
    return value;
}

void String::setValue (const QString &new_value)
{
    value = new_value;

    if (new_value.length() > max_length)
    {
        QString str = value;
        str.chop(new_value.length() - max_length);
        value = str;
        //qDebug
    }
}



