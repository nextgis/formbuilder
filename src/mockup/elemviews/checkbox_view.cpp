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

#include "checkbox_view.h"

using namespace Fb;
using namespace Mockup;


CheckBoxView::CheckBoxView (const Core::Elem *elem):
    ElemView(elem)
{
}

CheckBoxView::~CheckBoxView ()
{
}


const QVariant CheckBoxView::getDataForDecor () const
{
    return QVariant::fromValue(decor_data);
}


void CheckBoxView::atElemChange (Core::Attr *attr)
{
    Q_UNUSED(attr)

    auto attrs = elem->getAttrs();

    Core::Attr *attr_init_val = attrs.value("init_val");
    Core::Attr *attr_text = attrs.value("text");

    decor_data.first = attr_init_val->getValueAsVar().toBool();
    decor_data.second = attr_text->getValueAsVar().toString();
}
