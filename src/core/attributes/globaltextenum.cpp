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

#include "globaltextenum.h"

using namespace Fb;
using namespace Core;


QStringList GlobalTextEnum::s_values_range;


GlobalTextEnum::GlobalTextEnum (QString key_name, AttrInputType input_type):
    Attr(key_name, input_type)
{
    m_value_text = ""; // not set
}

GlobalTextEnum::~GlobalTextEnum ()
{
}


QVariant GlobalTextEnum::getValueAsVar () const
{
    return getValue();
}

void GlobalTextEnum::setValueAsVar (const QVariant &value)
{
    setValue(value.toString());
}


QString GlobalTextEnum::getValue () const
{
    return m_value_text;
}

void GlobalTextEnum::setValue (QString value_text)
{
//    if (findValueTextInRange(value_text) == -1)
//        return;
    m_value_text = value_text;
}

void GlobalTextEnum::resetValue ()
{
    m_value_text = "";
}


int GlobalTextEnum::findValueTextInRange (QString value_text) const
{
    for (int i = 0; i < s_values_range.size(); i++)
        if (s_values_range[i] == value_text)
            return i;
    return -1;
}



