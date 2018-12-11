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

#include "boolean.h"

using namespace Fb;
using namespace Core;


Boolean::Boolean (QString key_name, bool init_value, AttrInputType input_type):
    Attr(key_name, input_type)
{
    this->setValue(init_value);
}

Boolean::~Boolean ()
{
}


QVariant Boolean::getValueAsVar () const
{
    return value;
}

bool Boolean::getValue () const
{
    return value;
}

void Boolean::setValue (bool new_value)
{
    value = new_value;
}



