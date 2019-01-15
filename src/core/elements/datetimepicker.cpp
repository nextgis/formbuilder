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

#include "datetimepicker.h"

#include "attr_registrar.h"

using namespace Fb;
using namespace Core;


DateTimePicker::DateTimePicker (QString key_name):
    Elem(key_name)
{
    attr_keep_last   = (Boolean*) this->addAttr(new Boolean("keep_last", false));
    attr_init_date   = (DateTime*)this->addAttr(new DateTime("init_date"));
    attr_date_is_cur = (Boolean*) this->addAttr(new Boolean("date_is_cur", true));
    attr_date_type   = (DateTimeEnum*) this->addAttr(new DateTimeEnum("date_type"));

    fslot_timestamp = this->addFieldSlot("field_timestamp");

    this->behave(nullptr);
}

DateTimePicker::~DateTimePicker ()
{
}


void DateTimePicker::behave (Attr *attr)
{
    Q_UNUSED(attr)

    // Clear allowed field types before assigning new ones.
    this->clearTypesForFieldSlot(fslot_timestamp);

    // Attribute "is current" disables other attrs if true.
    if (attr_date_is_cur->getValue() == true)
    {
        attr_keep_last->setValue(false);

        attr_init_date->setEnabled(false);
        attr_keep_last->setEnabled(false);
    }
    else
    {
        attr_init_date->setEnabled(true);
        attr_keep_last->setEnabled(true);
    }

    int date_type = attr_date_type->getValue();

    attr_init_date->setFormat(g_getDateTimeCommonFormat(date_type));

    // Can write to date OR time OR date-time fields.
    //this->addTypeToFieldSlot(fslot_timestamp, g_getDateTimeFieldType(date_type));

    // Currently allow all date-time types. At least solves problem with loading datetimepicker from json.
    this->addTypeToFieldSlot(fslot_timestamp, FieldType::Date);
    this->addTypeToFieldSlot(fslot_timestamp, FieldType::Time);
    this->addTypeToFieldSlot(fslot_timestamp, FieldType::DateTime);
}



