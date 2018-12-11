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

#include "radiogroup_view.h"

#include "core/attributes/doubleitems.h"

using namespace Fb;
using namespace Mockup;


RadioGroupView::RadioGroupView (const Core::Elem *elem):
    ElemView(elem)
{
}

RadioGroupView::~RadioGroupView ()
{
}


const QVariant RadioGroupView::getDataForDecor () const
{
    return QVariant::fromValue(decor_data);
}


void RadioGroupView::atElemChange (Core::Attr *attr)
{
    Q_UNUSED(attr)

    auto attrs = elem->getAttrs();
    Core::Attr *attr_items = attrs.value("radio_items");
    auto items = attr_items->getValueAsVar().value<Core::DoubleItemsValue>();

    decor_data.first.clear();
    for (int i = 0; i < items.outers.size(); i++)
        decor_data.first.append(items.outers[i]);
    decor_data.second = items.def_index;
}



