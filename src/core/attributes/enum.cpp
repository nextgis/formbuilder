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

#include "enum.h"

using namespace Fb;
using namespace Core;


Enum::Enum (QString key_name, const QStringList &values_range, int init_value_index,
            AttrInputType input_type):
    Attr(key_name, input_type),
    values_range(values_range)
{
    if (this->values_range.size() == 0)
        throw; // TODO: named exception

    this->setValue(init_value_index);
}

Enum::~Enum ()
{
}


QVariant Enum::getValueAsVar () const
{
    return value_index;
}

int Enum::getValue () const
{
    return value_index;
}

void Enum::setValue (int new_value_indedx)
{
    value_index = new_value_indedx;
    if (!this->isValueIndexInRange())
        value_index = 0;
}


bool Enum::isValueIndexInRange ()
{
    return (value_index < 0 || value_index >= values_range.size()) ? false : true;
}



