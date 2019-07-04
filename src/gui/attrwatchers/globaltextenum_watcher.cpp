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

#include "globaltextenum_watcher.h"

#include "core/attributes/enum.h"
#include "core/attributes/globaltextenum.h"

#include <QComboBox>

using namespace Fb;
using namespace Gui;


GlobalTextEnumWatcher::GlobalTextEnumWatcher (Core::Attr *attr):
    AttrWatcher(attr)
{
}

GlobalTextEnumWatcher::~GlobalTextEnumWatcher ()
{
}

QWidget *GlobalTextEnumWatcher::createWidget (MainWindow *window) const
{
    Q_UNUSED(window)

    Core::GlobalTextEnum *concr_attr = qobject_cast<Core::GlobalTextEnum*>(attr);
    if (concr_attr == nullptr)
    {
        //qDebug
        return nullptr;
    }

    auto list_names = concr_attr->getValuesRange();
    auto sel_list_name = concr_attr->getValue();

    QComboBox *combobox = new QComboBox();
    combobox->addItem("-"); // in order to allow not to select any list name
    combobox->addItems(list_names);

    int sel_list_index = 0; // select "-" by default
    for (int i = 0; i < list_names.size(); i++)
    {
        if (list_names[i] == sel_list_name) // we know that list names are unique
        {
            sel_list_index = i + 1;
            break;
        }
    }
    combobox->setCurrentIndex(sel_list_index);

    connect(combobox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GlobalTextEnumWatcher::onComboBoxCurrentIndexChanged);
    return combobox;
}

void GlobalTextEnumWatcher::onComboBoxCurrentIndexChanged (int index)
{
    Core::GlobalTextEnum *concr_attr = qobject_cast<Core::GlobalTextEnum*>(attr);
    if (concr_attr == nullptr)
    {
        //qDebug
        return;
    }

    auto list_names = concr_attr->getValuesRange();
    int sel_list_index = index - 1;

    if (sel_list_index == -1)
        concr_attr->resetValue();
    else
        concr_attr->setValue(list_names[sel_list_index]);

    emit attrChangedByWidget(attr);
}



