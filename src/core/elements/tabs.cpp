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

#include "tabs.h"

#include "attr_registrar.h"

using namespace Fb;
using namespace Core;


Tabs::Tabs (QString key_name):
    Elem(key_name)
{
    attr_cur_page  = (Number*)     this->addAttr(new Number("cur_page", 1, 1, 2));
    attr_page_hdrs = (StringList*) this->addAttr(new StringList("page_hdrs",
                                                 {tr("TAB 1"), tr("TAB 2")}, 1, 8,
                                                 AttrInputType::PageList));
    this->behave(nullptr);
}

Tabs::~Tabs ()
{
}


void Tabs::behave (Attr *attr)
{
    if (attr == attr_page_hdrs)
    {
        attr_cur_page->setMax(attr_page_hdrs->getValue().size());
    }
}


QStringList Tabs::getStringsToTranslate () const
{
    return attr_page_hdrs->getValue();
}

