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

#include "datetime_watcher.h"

#include "core/attributes/datetime.h"
#include "core/attributes/datetimeenum.h"

#include <QDateTimeEdit>

using namespace Fb;
using namespace Gui;


DateTimeWatcher::DateTimeWatcher (Core::Attr *attr):
    AttrWatcher(attr)
{
}

DateTimeWatcher::~DateTimeWatcher ()
{
}

QWidget *DateTimeWatcher::createWidget (MainWindow *window) const
{
    Q_UNUSED(window)

    Core::DateTime *concr_attr = qobject_cast<Core::DateTime*>(attr);
    if (concr_attr == nullptr)
    {
        //qDebug
        return nullptr;
    }

    QDateTimeEdit *edit = new QDateTimeEdit();
    edit->setDateTime(concr_attr->getValue());
    edit->setDisplayFormat(concr_attr->getFormat());
    connect(edit, &QDateTimeEdit::dateTimeChanged, this, &DateTimeWatcher::onDateTimeEditChanged);
    return edit;
}

void DateTimeWatcher::onDateTimeEditChanged (const QDateTime &new_value)
{
    Core::DateTime *concr_attr = qobject_cast<Core::DateTime*>(attr);
    if (concr_attr == nullptr)
    {
        //qDebug
        return;
    }

    concr_attr->setValue(new_value);

    emit attrChangedByWidget(attr);
}



