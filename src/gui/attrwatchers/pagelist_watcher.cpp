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

#include "pagelist_watcher.h"

#include "core/attributes/stringlist.h"
#include "dialogs/tabpagesdialog.h"
#include "gui_common.h"

#include <QPushButton>

using namespace Fb;
using namespace Gui;


PageListWatcher::PageListWatcher (Core::Attr *attr):
    AttrWatcher(attr)
{
}

PageListWatcher::~PageListWatcher ()
{
}

QWidget *PageListWatcher::createWidget (MainWindow *window) const
{
    this->window = window;

    QPushButton *button = new QPushButton();
    button->setText(tr("Modify"));
    connect(button, &QPushButton::clicked, this, &PageListWatcher::onButtonClicked);
    return button;
}

void PageListWatcher::onButtonClicked ()
{
    Core::StringList *concr_attr = qobject_cast<Core::StringList*>(attr);
    if (concr_attr == nullptr)
    {
        //qDebug
        return;
    }

    TabPagesDialog dialog(window, concr_attr->getMinSize(), concr_attr->getMaxSize());

    const QStringList &list = concr_attr->getValue();
    int old_count = list.size();
    dialog.putItems(list);

    if (!dialog.exec())
        return; // nothing has changed

    QStringList new_value;
    dialog.getItems(new_value);

    int ret = QMessageBox::Ok;

    int diff = new_value.count() - old_count;
    if (diff < 0)
    {
        ret = g_showQuestion(window, tr("The amount of headers becomes less than it was. An "
                             "according amount of pages must be removed starting from the end. Do "
                             "you really want to remove last %1 page(s) with all its elements?")
                             .arg(std::abs(diff)));
    }

    if (ret == QMessageBox::Ok)
        concr_attr->setValue(std::move(new_value));

    emit attrChangedByWidget(attr);
}



