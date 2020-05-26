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

#include "checkbox.h"

#include "attr_registrar.h"

using namespace Fb;
using namespace Core;


CheckBox::CheckBox (QString key_name):
    Elem(key_name)
{
    attr_keep_last  = (Boolean*) this->addAttr(new Boolean("keep_last", false));
    attr_text       = (String*)  this->addAttr(new String("text", tr("Text")));
    attr_init_value = (Boolean*) this->addAttr(new Boolean("init_val", false));

    fslot_int01 = this->addFieldSlot("field_int01");

    this->addTypeToFieldSlot(fslot_int01, {FieldType::Integer});
    this->addTypeToFieldSlot(fslot_int01, {FieldType::BigInteger});
    this->addTypeToFieldSlot(fslot_int01, {FieldType::Real});
}

CheckBox::~CheckBox ()
{
}


QStringList CheckBox::getStringsToTranslate () const
{
    return {attr_text->getValue()};
}

