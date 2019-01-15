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

#include "depcomboboxes_view.h"

#include "core/attributes/depdoubleitems.h"

using namespace Fb;
using namespace Mockup;


DepComboBoxesView::DepComboBoxesView (const Core::Elem *elem):
    ElemView(elem)
{
}

DepComboBoxesView::~DepComboBoxesView ()
{
}


const QVariant DepComboBoxesView::getDataForDecor () const
{
    return QVariant::fromValue(decor_data);
}


void DepComboBoxesView::atElemChange (Core::Attr *attr)
{
    Q_UNUSED(attr)

    auto attrs = elem->getAttrs();
    Core::Attr *attr_items = attrs.value("items");
    auto items = attr_items->getValueAsVar().value<Core::DepDoubleItemsValue>();

    int main_list_index = items.main.def_index;
    if (main_list_index < 0 || main_list_index >= items.main.outers.size())
    {
        decor_data.first = "";
        decor_data.second = "";
        return;
    }

    decor_data.first = items.main.outers[main_list_index];

    int dep_list_index = items.deps[main_list_index].def_index;
    if (dep_list_index < 0 || dep_list_index >= items.deps[main_list_index].outers.size())
        decor_data.second = "";
    else
        decor_data.second = items.deps[main_list_index].outers[dep_list_index];
}



