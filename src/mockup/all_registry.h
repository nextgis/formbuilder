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

#pragma once

#include "elemview_registrar.h"

// Include here all elem view factories:
#include "elemviews/box_view.h"
#include "elemviews/label_view.h"
#include "elemviews/textedit_view.h"
#include "elemviews/checkbox_view.h"
#include "elemviews/combobox_view.h"
#include "elemviews/depcomboboxes_view.h"
#include "elemviews/splitcombobox_view.h"
#include "elemviews/radiogroup_view.h"
#include "elemviews/datetimepicker_view.h"
#include "elemviews/counter_view.h"
#include "elemviews/tabs_view.h"
#include "elemviews/ngmform_view.h"

namespace Fb
{
namespace Mockup
{


inline void g_registerElemViews ()
{
    ElemViewRegistrar::add("label", new LabelViewFct());
    ElemViewRegistrar::add("voidspace", new BoxViewFct());
    ElemViewRegistrar::add("textedit", new TextEditViewFct());
    ElemViewRegistrar::add("checkbox", new CheckBoxViewFct());
    ElemViewRegistrar::add("combobox", new ComboBoxViewFct());
    ElemViewRegistrar::add("depcomboboxes", new DepComboBoxesViewFct());
    ElemViewRegistrar::add("splitcombobox", new SplitComboBoxViewFct());
    ElemViewRegistrar::add("radiogroup", new RadioGroupViewFct());
    ElemViewRegistrar::add("datetimepicker", new DateTimePickerViewFct());
    ElemViewRegistrar::add("coords", new BoxViewFct());
    ElemViewRegistrar::add("counter", new CounterViewFct());
    ElemViewRegistrar::add("dmeter", new BoxViewFct());
    ElemViewRegistrar::add("photo", new BoxViewFct());
    ElemViewRegistrar::add("signfield", new BoxViewFct());
    ElemViewRegistrar::add("tabs", new TabsViewFct());
    ElemViewRegistrar::add("ngmform", new NgmFormViewFct());
}


}
}



