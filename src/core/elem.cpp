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

#include "elem.h"

#include "attr_registrar.h"

using namespace Fb;
using namespace Core;


Elem::Elem (QString key_name):
    QObject(),
    key_name(key_name)
{
}

Elem::~Elem ()
{
    for (auto attr_key_name: attrs.keys())
        delete attrs.value(attr_key_name);
    attrs.clear();
}


void Elem::onAttrChanged (Attr *attr)
{
    this->behave(attr); // will be overriden

    emit hasChanged(attr);
}


/*
Attr* Elem::addAttr (QString attr_key_name)
{
    auto attr_data = AttrRegistrar::get(attr_key_name);
    if (attr_data == nullptr)
    {
        //qDebug
        return nullptr;
    }
    if (attr_data->fct == nullptr)
    {
        //qDebug
        return nullptr;
    }

    Attr *attr = attr_data->fct->create();
    attrs.insert(attr_key_name, attr);

    // TODO: check that ok.

    return attr;
}
*/


Attr* Elem::addAttr (Attr *attr)
{
    if (attr == nullptr)
        throw; // TODO: named exception

    QString attr_key_name = attr->getKeyName();
    if (attr_key_name == "")
        throw; // TODO: named exception

    auto attr_data = AttrRegistrar::get(attr_key_name);
    if (attr_data == nullptr)
    {
        //qDebug
    }

    // We create attr anyway, even if it has not got any data.
    attrs.insert(attr_key_name, attr);

    // TODO: check that ok.

    return attr;
}


QString Elem::addFieldSlot (QString slot_key_name)
{
    field_slots.insert(slot_key_name, {FieldType::String}); // always string type

    // TODO: check that ok.

    return slot_key_name;
}

void Elem::addTypeToFieldSlot (QString slot_key_name, FieldType field_type)
{
    auto it = field_slots.find(slot_key_name);
    if (it == field_slots.end())
    {
        //qDebug
        return;
    }

    it.value().insert(field_type);

    // TODO: check that ok.
}

//void Elem::removeTypeFromFieldSlot (QString slot_key_name, FieldType field_type)
//{
//}

void Elem::clearTypesForFieldSlot (QString slot_key_name)
{
    auto it = field_slots.find(slot_key_name);
    if (it == field_slots.end())
    {
        //qDebug
        return;
    }

    it.value().clear();
    it.value().insert(FieldType::String);  // always string type
}



