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

#include "textedit_view.h"

using namespace Fb;
using namespace Mockup;


TextEditView::TextEditView (const Core::Elem *elem):
    ElemView(elem)
{
}

TextEditView::~TextEditView ()
{
}


const QVariant TextEditView::getDataForDecor () const
{
    return decor_data;
}


void TextEditView::atElemChange (Core::Attr *attr)
{
    Q_UNUSED(attr)

    auto attrs = elem->getAttrs();

    Core::Attr *attr_init_text = attrs.value("init_text");
    Core::Attr *attr_ngw_login = attrs.value("ngw_login");
    Core::Attr *attr_ngid_login = attrs.value("ngid_login");

    if (attr_ngid_login->getValueAsVar().toBool() == true)
        decor_data = tr("<NextGIS ID login>");
    else if (attr_ngw_login->getValueAsVar().toBool() == true)
        decor_data = tr("<NextGIS Web login>");
    else
        decor_data = attr_init_text->getValueAsVar().toString();
}



