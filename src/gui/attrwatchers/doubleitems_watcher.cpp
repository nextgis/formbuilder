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

#include "doubleitems_watcher.h"

#include "core/attributes/doubleitems.h"
#include "dialogs/doubleitemsdialog.h"
#include "gui_common.h"

#include <QPushButton>

using namespace Fb;
using namespace Gui;


DoubleItemsWatcher::DoubleItemsWatcher (Core::Attr *attr):
    AttrWatcher(attr)
{
}

DoubleItemsWatcher::~DoubleItemsWatcher ()
{
}

QWidget *DoubleItemsWatcher::createWidget (MainWindow *window) const
{
    this->window = window;

    QPushButton *button = new QPushButton();
    button->setText(tr("Modify"));
    connect(button, &QPushButton::clicked, this, &DoubleItemsWatcher::onButtonClicked);
    return button;
}

void DoubleItemsWatcher::onButtonClicked ()
{
    Core::DoubleItems *concr_attr = qobject_cast<Core::DoubleItems*>(attr);
    if (concr_attr == nullptr)
    {
        //qDebug
        return;
    }

    DoubleItemsDialog dialog(window, concr_attr->getMinSize(), concr_attr->getMaxSize(),
                             concr_attr->getMustHaveDefault());

    dialog.putItems(concr_attr->getValue());

    if (!dialog.exec())
        return; // nothing has changed

    Core::DoubleItemsValue new_value;
    dialog.getItems(new_value);
    concr_attr->setValue(new_value);

    emit attrChangedByWidget(attr);
}



