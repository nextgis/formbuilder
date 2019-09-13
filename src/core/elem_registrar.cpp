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

#include "elem_registrar.h"

using namespace Fb;
using namespace Core;


std::map<QString, ElemData> ElemRegistrar::elements;

void ElemRegistrar::add (QString key_name, ElemData elem_data)
{
    if (elements.find(key_name) != elements.end())
    {
        //qDebug
        return;
    }
    elements.insert({key_name, elem_data});
}

const ElemData *ElemRegistrar::get (QString key_name)
{
    if (elements.find(key_name) == elements.end())
    {
        //qDebug
        return nullptr;
    }
    return &elements.at(key_name);
}

const std::map<QString, ElemData> &ElemRegistrar::getAll ()
{
    return elements;
}
