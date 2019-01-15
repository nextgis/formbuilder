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

#include "number.h"

using namespace Fb;
using namespace Core;


Number::Number (QString key_name, int init_value, int min, int max, AttrInputType input_type):
    Attr(key_name, input_type)
{
    if (min > max)
    {
        min = max;
        //qDebug
    }

    this->min = min;
    this->max = max;

    this->setValue(init_value);
}

Number::~Number ()
{
}


QVariant Number::getValueAsVar () const
{
    return value;
}

void Number::setValueAsVar (const QVariant &new_value)
{
    setValue(new_value.toInt());
}


int Number::getValue () const
{
    return value;
}

void Number::setValue (int new_value)
{
    if (new_value < min)
    {
        value = min;
        //qDebug
    }

    if (new_value > max)
    {
        value = max;
        //qDebug
    }

    value = new_value;
}


void Number::setMin (int new_min)
{
    if (new_min > max)
    {
        new_min = max;
        //qDebug
    }

    if (value < new_min)
    {
        value = new_min;
        //qDebug
    }

    min = new_min;
}

void Number::setMax (int new_max)
{
    if (new_max < min)
    {
        new_max = min;
        //qDebug
    }

    if (value > new_max)
    {
        value = new_max;
        //qDebug
    }

    max = new_max;
}



