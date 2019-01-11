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

#include "depdoubleitems_watcher.h"

#include "core/attributes/depdoubleitems.h"
#include "dialogs/depdoubleitemsdialog.h"
#include "gui_common.h"

#include <QPushButton>

using namespace Fb;
using namespace Gui;


DepDoubleItemsWatcher::DepDoubleItemsWatcher (Core::Attr *attr):
    AttrWatcher(attr)
{
}

DepDoubleItemsWatcher::~DepDoubleItemsWatcher ()
{
}

QWidget *DepDoubleItemsWatcher::createWidget (MainWindow *window) const
{
    this->window = window;

    QPushButton *button = new QPushButton();
    button->setText(tr("Modify"));
    connect(button, &QPushButton::clicked, this, &DepDoubleItemsWatcher::onButtonClicked);
    return button;
}

void DepDoubleItemsWatcher::onButtonClicked ()
{
    Core::DepDoubleItems *concr_attr = qobject_cast<Core::DepDoubleItems*>(attr);
    if (concr_attr == nullptr)
    {
        //qDebug
        return;
    }

    DepDoubleItemsDialog dialog(window);

    dialog.putItems(concr_attr->getValue());

    if (!dialog.exec())
        return; // nothing has changed

    Core::DepDoubleItemsValue new_value;
    dialog.getItems(new_value);
    concr_attr->setValue(new_value);

    emit attrChangedByWidget(attr);
}



