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

#include "splitcombobox_view.h"

#include "core/attributes/tripleitems.h"

using namespace Fb;
using namespace Mockup;


SplitComboBoxView::SplitComboBoxView (const Core::Elem *elem):
    ElemView(elem)
{
}

SplitComboBoxView::~SplitComboBoxView ()
{
}


const QVariant SplitComboBoxView::getDataForDecor () const
{
    return QVariant::fromValue(decor_data);
}


void SplitComboBoxView::atElemChange (Core::Attr *attr)
{
    Q_UNUSED(attr)

    auto attrs = elem->getAttrs();
    Core::Attr *attr_items = attrs.value("items");
    auto items = attr_items->getValueAsVar().value<Core::TripleItemsValue>();

    Core::Attr *attr_cap_left = attrs.value("caption_left");
    Core::Attr *attr_cap_right = attrs.value("caption_right");

    std::get<0>(decor_data) = attr_cap_left->getValueAsVar().toString();
    std::get<1>(decor_data) = attr_cap_right->getValueAsVar().toString();

    if (items.def_index < 0 || items.def_index >= items.outers_left.size())
    {
        std::get<2>(decor_data) = "";
        std::get<3>(decor_data) = "";
        return;
    }

    std::get<2>(decor_data) = items.outers_left[items.def_index];
    std::get<3>(decor_data) = items.outers_right[items.def_index];
}



