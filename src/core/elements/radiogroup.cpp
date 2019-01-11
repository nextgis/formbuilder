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

#include "radiogroup.h"

#include "attr_registrar.h"

using namespace Fb;
using namespace Core;


RadioGroup::RadioGroup (QString key_name):
    Elem(key_name)
{
    attr_keep_last   = (Boolean*)     this->addAttr(new Boolean("keep_last", false));
    attr_items       = (DoubleItems*) this->addAttr(new DoubleItems("radio_items",
                                                    {{tr("Item 1"), tr("Item 2")},
                                                     {tr("Item 1"), tr("Item 2")},
                                                     0}, 2, 16, true));
    fslot_common = this->addFieldSlot("field_common");
}

RadioGroup::~RadioGroup ()
{
}




