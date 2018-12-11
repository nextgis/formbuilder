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

#include "field.h"

#include <QRegularExpression>

using namespace Fb;
using namespace Core;


Field::Field (Layer *owner, QString name, QString alias, FieldType type):
    QObject(),
    owner(owner),
    name(name),
    alias(alias),
    type(type),
    elem(nullptr),
    slot("")
{
}

Field::~Field ()
{
}


bool Field::isBound () const
{
    return elem == nullptr ? false : true;
}

bool Field::canBind (const Elem *elem, QString slot) const
{
    if (elem == nullptr || slot == "")
        return false;
    auto field_slots = elem->getFieldSlots();
    auto field_slot = field_slots.value(slot);
    if (field_slot.find(type) != field_slot.end()) // compare field types of the field and a slot
        return true;
    return false;
}

void Field::bind (const Elem *new_elem, QString new_slot)
{
    if (!this->canBind(new_elem, new_slot))
        return;
    elem = new_elem; // rewrite old binding!
    slot = new_slot;
}

void Field::unbind ()
{
    elem = nullptr;
    slot = "";
}


bool Field::isNameCorrect (QString name) // STATIC
{
    if (name.isEmpty())
        return false;
    QRegularExpression exp1("^[a-zA-Z0-9\\_]+$");
    QRegularExpression expr2("^[a-zA-Z]");
    QRegularExpressionMatch match1 = exp1.match(name);
    QRegularExpressionMatch match2 = expr2.match(name.left(1));
    if (match1.hasMatch() && match2.hasMatch())
        return true;
    return false;
}


void Field::onNeedToSetNewAlias (const QString &text)
{
    alias = text;
}

void Field::onNeedToSetNewFieldType (int index)
{
    type = g_findFieldType(index);
}



