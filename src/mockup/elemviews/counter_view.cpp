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

#include "counter_view.h"

using namespace Fb;
using namespace Mockup;


CounterView::CounterView (const Core::Elem *elem):
    ElemView(elem)
{
}

CounterView::~CounterView ()
{
}


const QVariant CounterView::getDataForDecor () const
{
    return decor_data;
}


void CounterView::atElemChange (Core::Attr *attr)
{
    Q_UNUSED(attr)

    auto attrs = elem->getAttrs();
    Core::Attr *attr_count_init = attrs.value("count_init");
    Core::Attr *attr_count_pref = attrs.value("count_pref");
    Core::Attr *attr_count_suff = attrs.value("count_suff");

    decor_data = attr_count_pref->getValueAsVar().toString() +
                 attr_count_init->getValueAsVar().toString() +
                 attr_count_suff->getValueAsVar().toString();
}



