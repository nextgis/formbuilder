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

#include "fieldslot_registrar.h"

using namespace Fb;
using namespace Core;


std::map<QString, FieldSlotData> FieldSlotRegistrar::field_slots;

void FieldSlotRegistrar::add (QString key_name, FieldSlotData field_slot_data)
{
    if (field_slots.find(key_name) != field_slots.end())
    {
        //qDebug
        return;
    }
    field_slots.insert({key_name, field_slot_data});
}

const FieldSlotData *FieldSlotRegistrar::get (QString key_name)
{
    if (field_slots.find(key_name) == field_slots.end())
    {
        //qDebug
        return nullptr;
    }
    return &field_slots.at(key_name);
}

const std::map<QString, FieldSlotData> &FieldSlotRegistrar::getAll ()
{
    return field_slots;
}
