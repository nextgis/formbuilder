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

#include "datetimepicker_view.h"

#include "core/datetime_types.h"

#include <QDateTime>

using namespace Fb;
using namespace Mockup;


DateTimePickerView::DateTimePickerView (const Core::Elem *elem):
    ElemView(elem)
{
}

DateTimePickerView::~DateTimePickerView ()
{
}


const QVariant DateTimePickerView::getDataForDecor () const
{
    return decor_data;
}


void DateTimePickerView::atElemChange (Core::Attr *attr)
{
    Q_UNUSED(attr)

    auto attrs = elem->getAttrs();

    Core::Attr *attr_date_init = attrs.value("init_date");
    Core::Attr *attr_date_is_cur = attrs.value("date_is_cur");
    Core::Attr *attr_date_type = attrs.value("date_type");

    int date_type = attr_date_type->getValueAsVar().toInt();
    QDateTime date = attr_date_init->getValueAsVar().toDateTime();

    if (attr_date_is_cur->getValueAsVar().toBool() == true)
    {
        switch (date_type)
        {
            case 0: decor_data = tr("<Current date>"); break;
            case 1: decor_data = tr("<Current time>"); break;
            default: decor_data = tr("<Current date and time>"); break;
        }
    }
    else
    {
        decor_data = date.toString(Core::g_getDateTimeCommonFormat(date_type));
    }
}



