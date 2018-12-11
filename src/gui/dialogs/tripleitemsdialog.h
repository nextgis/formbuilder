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
#include "core/attributes/tripleitems.h"

namespace Fb
{
namespace Gui
{


class TripleItemsDialog: public CustomDialog
{
    Q_OBJECT

    public:

     explicit TripleItemsDialog (QWidget *parent, int min_rows, int max_rows,
                                 bool must_have_default_row);
     virtual ~TripleItemsDialog ();

     void putItems (const Core::TripleItemsValue &value);
     void getItems (Core::TripleItemsValue &value) const;

    protected:

     ItemsWidget *widget;
};


}
}



