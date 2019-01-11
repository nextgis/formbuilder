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

#include "attr_registrar.h"

using namespace Fb;
using namespace Core;


std::map<QString, AttrData> AttrRegistrar::attributes;

void AttrRegistrar::add (QString key_name, AttrData attr_data)
{
    if (attributes.find(key_name) != attributes.end())
    {
        //qDebug
        return;
    }
    attributes.insert({key_name, attr_data});
}

const AttrData *AttrRegistrar::get (QString key_name)
{
    if (attributes.find(key_name) == attributes.end())
    {
        //qDebug
        return nullptr;
    }
    return &attributes.at(key_name);
}

const std::map<QString, AttrData> &AttrRegistrar::getAll ()
{
    return attributes;
}
