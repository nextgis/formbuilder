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

#include "counter.h"

#include "attr_registrar.h"

using namespace Fb;
using namespace Core;


Counter::Counter (QString key_name):
    Elem(key_name)
{
    attr_count_init  = (Number*) this->addAttr(new Number("count_init", 0, 0,
                                                          std::numeric_limits<int>::max() - 1));
    attr_count_incr  = (Number*) this->addAttr(new Number("count_incr", 1, 1,
                                                          std::numeric_limits<int>::max() - 1));
    attr_count_pref  = (String*) this->addAttr(new String("count_pref", ""));
    attr_count_suff  = (String*) this->addAttr(new String("count_suff", ""));

    fslot_common = this->addFieldSlot("field_common");

    this->behave(nullptr);
}

Counter::~Counter ()
{
}


void Counter::behave (Attr *attr)
{
    // No (BIG)INTEGER or REAL fields support at the beginning.
    this->clearTypesForFieldSlot(fslot_common);

    QString pref = attr_count_pref->getValue();
    QString suff = attr_count_suff->getValue();

    // Can write to (BIG)INTEGER and REAL fields only if "prefix" and "suffix" attrs are void
    // strings.
    if (pref == "" && suff == "")
    {
        this->addTypeToFieldSlot(fslot_common, {FieldType::Integer});
        this->addTypeToFieldSlot(fslot_common, {FieldType::BigInteger});
        this->addTypeToFieldSlot(fslot_common, {FieldType::Real});
    }
}



