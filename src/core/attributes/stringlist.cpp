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

#include "stringlist.h"

using namespace Fb;
using namespace Core;


StringList::StringList (QString key_name, QStringList init_value, int min_size, int max_size,
                        AttrInputType input_type):
    Attr(key_name, input_type)
{
    if (min_size < 0)
    {
        min_size = 0;
        //qDebug
    }
    if (max_size < 0)
    {
        max_size = 0;
        //qDebug
    }

    if (min_size > max_size)
    {
        min_size = max_size;
        //qDebug
    }

    this->min_size = min_size;
    this->max_size = max_size;

    this->setValue(init_value);
}

StringList::~StringList ()
{
}


QVariant StringList::getValueAsVar () const
{
    return value;
}

void StringList::setValueAsVar (const QVariant &new_value)
{
    setValue(new_value.toStringList());
}


const QStringList &StringList::getValue () const
{
    return value;
}

void StringList::setValue (QStringList new_value)
{
    bool was_adjusted = false;

    while (new_value.size() < min_size)
    {
        new_value.append("");
        was_adjusted = true;
    }

    if (!new_value.isEmpty())
    {
        while (new_value.size() > max_size)
        {
            new_value.removeLast();
            was_adjusted = true;
        }
    }

    //if (was_adjusted)
        //qDebug

    value = std::move(new_value);
}



