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

#include "enum_watcher.h"

#include "core/attributes/enum.h"

#include <QComboBox>

using namespace Fb;
using namespace Gui;


EnumWatcher::EnumWatcher (Core::Attr *attr):
    AttrWatcher(attr)
{
}

EnumWatcher::~EnumWatcher ()
{
}

QWidget *EnumWatcher::createWidget (MainWindow *window) const
{
    Q_UNUSED(window)

    Core::Enum *concr_attr = qobject_cast<Core::Enum*>(attr);
    if (concr_attr == nullptr)
    {
        //qDebug
        return nullptr;
    }

    QComboBox *combobox = new QComboBox();
    combobox->addItems(concr_attr->getValuesRange());
    combobox->setCurrentIndex(concr_attr->getValue());
    connect(combobox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &EnumWatcher::onComboBoxCurrentIndexChanged);
    return combobox;
}

void EnumWatcher::onComboBoxCurrentIndexChanged (int index)
{
    Core::Enum *concr_attr = qobject_cast<Core::Enum*>(attr);
    if (concr_attr == nullptr)
    {
        //qDebug
        return;
    }

    concr_attr->setValue(index);

    emit attrChangedByWidget(attr);
}



