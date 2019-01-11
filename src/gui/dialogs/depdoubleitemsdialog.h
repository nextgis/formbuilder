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

#pragma once

#include "gui/custom_dialog.h"
#include "gui/itemswidget.h"
#include "core/attributes/depdoubleitems.h"

namespace Fb
{
namespace Gui
{


class DepDoubleItemsDialog: public CustomDialog
{
    Q_OBJECT

    public:

     explicit DepDoubleItemsDialog (QWidget *parent);
     virtual ~DepDoubleItemsDialog ();

     void putItems (const Core::DepDoubleItemsValue &value);
     void getItems (Core::DepDoubleItemsValue &value) const;

    protected slots:

     void onOkClicked ();
     void onLeftWidgetRowSelected ();
     void onLeftWidgetRowAdded ();
     void onLeftWidgetRowRemoved ();
     void onLeftWidgetCleared ();
     void onRightWidgetResize (QSize new_size);

    protected:

     ItemsWidget *appendRightWidget ();
     void removeCurRightWidget ();
     void removeAllRightWidgets ();
     void setRightWidget (int index);

     ItemsWidget *left_widget;
     QList<ItemsWidget*> right_widgets;

     ItemsWidget *cur_right_widget;

     QGridLayout *lay_this;
     QVBoxLayout *lay_right;
};


}
}



