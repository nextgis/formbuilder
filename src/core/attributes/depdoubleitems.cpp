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

#include "depdoubleitems.h"

using namespace Fb;
using namespace Core;


DepDoubleItems::DepDoubleItems (QString key_name, const DepDoubleItemsValue &init_value,
                                int min_size, int max_size, bool must_have_default):
    Attr(key_name, AttrInputType::DepDoubleItems),
    must_have_default(must_have_default)
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

    if (must_have_default && min_size == 0)
    {
        min_size = 1;
        //qDebug
    }

    if (min_size > max_size)
    {
        max_size = min_size;
        //qDebug
    }

    this->min_size = min_size;
    this->max_size = max_size;

    this->setValue(init_value);
}

DepDoubleItems::~DepDoubleItems ()
{
}


QVariant DepDoubleItems::getValueAsVar () const
{
    return QVariant::fromValue(value);
}

const DepDoubleItemsValue &DepDoubleItems::getValue () const
{
    return value;
}

void DepDoubleItems::setValue (const DepDoubleItemsValue &new_value)
{
    // Adjust size of the main "inners" list to min/max size.
    // ...

    // Check must_have_default and adjust size.
    // ...

    // The main "outers" list must be of the same size as "inners".
    // ...

    // Do the same for all dependent lists:
    // ...

    value = new_value;
}



