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

#include "boolean_watcher.h"

#include "core/attributes/boolean.h"

#include <QHBoxLayout>
#include <QCheckBox>

using namespace Fb;
using namespace Gui;


BooleanWatcher::BooleanWatcher (Core::Attr *attr):
    AttrWatcher(attr)
{
}

BooleanWatcher::~BooleanWatcher ()
{
}

QWidget *BooleanWatcher::createWidget (MainWindow *window) const
{
    Q_UNUSED(window)

    Core::Boolean *concr_attr = qobject_cast<Core::Boolean*>(attr);
    if (concr_attr == nullptr)
    {
        //qDebug
        return nullptr;
    }

    QWidget *widget = new QWidget();
    QHBoxLayout *lay = new QHBoxLayout(widget);
    lay->setContentsMargins(5, 0, 0, 0);
    lay->setSpacing(0);
    QCheckBox *checkbox = new QCheckBox(widget);
    lay->addWidget(checkbox);
    checkbox->setChecked(concr_attr->getValue());
    connect(checkbox, &QCheckBox::stateChanged, this, &BooleanWatcher::onCheckBoxStateChanged);
    return widget;
}

void BooleanWatcher::onCheckBoxStateChanged (int state)
{
    Core::Boolean *concr_attr = qobject_cast<Core::Boolean*>(attr);
    if (concr_attr == nullptr)
    {
        //qDebug
        return;
    }

    concr_attr->setValue(state);

    emit attrChangedByWidget(attr);
}



