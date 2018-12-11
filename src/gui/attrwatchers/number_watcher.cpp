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

#include "number_watcher.h"

#include "core/attributes/number.h"
#include "custom_spinbox.h"

using namespace Fb;
using namespace Gui;


NumberWatcher::NumberWatcher (Core::Attr *attr):
    AttrWatcher(attr)
{
}

NumberWatcher::~NumberWatcher ()
{
}

QWidget *NumberWatcher::createWidget (MainWindow *window) const
{
    Q_UNUSED(window)

    Core::Number *concr_attr = qobject_cast<Core::Number*>(attr);
    if (concr_attr == nullptr)
    {
        //qDebug
        return nullptr;
    }

    CustomSpinBox *spinbox = new CustomSpinBox();
    spinbox->setMinimum(concr_attr->getMin());
    spinbox->setMaximum(concr_attr->getMax());
    spinbox->setValue(concr_attr->getValue());
    //spinbox->setWrapping(true);
    connect(spinbox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &NumberWatcher::onSpinBoxValueChanged);
    return spinbox;
}

void NumberWatcher::onSpinBoxValueChanged (int number)
{
    Core::Number *concr_attr = qobject_cast<Core::Number*>(attr);
    if (concr_attr == nullptr)
    {
        //qDebug
        return;
    }

    concr_attr->setValue(number);

    emit attrChangedByWidget(attr);
}



