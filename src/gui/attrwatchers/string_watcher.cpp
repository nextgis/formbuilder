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

#include "string_watcher.h"

#include "core/attributes/string.h"

#include <QLineEdit>

using namespace Fb;
using namespace Gui;


StringWatcher::StringWatcher (Core::Attr *attr):
    AttrWatcher(attr)
{
}

StringWatcher::~StringWatcher ()
{
}

QWidget *StringWatcher::createWidget (MainWindow *window) const
{
    Q_UNUSED(window)

    Core::String *concr_attr = qobject_cast<Core::String*>(attr);
    if (concr_attr == nullptr)
    {
        //qDebug
        return nullptr;
    }

    QLineEdit *edit = new QLineEdit();
    edit->setText(concr_attr->getValue());
    edit->setMaxLength(concr_attr->getMaxLength());
    connect(edit, &QLineEdit::textChanged, this, &StringWatcher::onLineEditTextChanged);
    return edit;
}

void StringWatcher::onLineEditTextChanged (QString text)
{
    Core::String *concr_attr = qobject_cast<Core::String*>(attr);
    if (concr_attr == nullptr)
    {
        //qDebug
        return;
    }

    concr_attr->setValue(text);

    emit attrChangedByWidget(attr);
}



