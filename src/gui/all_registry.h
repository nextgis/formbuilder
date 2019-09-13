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

#include "attrwatcher_registrar.h"

// Include here all attr watcher factories:
#include "attrwatchers/boolean_watcher.h"
#include "attrwatchers/number_watcher.h"
#include "attrwatchers/string_watcher.h"
#include "attrwatchers/enum_watcher.h"
#include "attrwatchers/globaltextenum_watcher.h"
#include "attrwatchers/datetime_watcher.h"
#include "attrwatchers/doubleitems_watcher.h"
#include "attrwatchers/tripleitems_watcher.h"
#include "attrwatchers/depdoubleitems_watcher.h"
#include "attrwatchers/pagelist_watcher.h"

namespace Fb
{
namespace Gui
{


inline void g_registerAttrWatchers ()
{
    AttrWatcherRegistrar::add(Core::AttrInputType::Boolean, new BooleanWatcherFct());
    AttrWatcherRegistrar::add(Core::AttrInputType::Number, new NumberWatcherFct());
    AttrWatcherRegistrar::add(Core::AttrInputType::String, new StringWatcherFct());
    AttrWatcherRegistrar::add(Core::AttrInputType::Enum, new EnumWatcherFct());
    AttrWatcherRegistrar::add(Core::AttrInputType::GlobalTextEnum, new GlobalTextEnumWatcherFct());
    AttrWatcherRegistrar::add(Core::AttrInputType::DateTime, new DateTimeWatcherFct());
    AttrWatcherRegistrar::add(Core::AttrInputType::DoubleItems, new DoubleItemsWatcherFct());
    AttrWatcherRegistrar::add(Core::AttrInputType::TripleItems, new TripleItemsWatcherFct());
    AttrWatcherRegistrar::add(Core::AttrInputType::DepDoubleItems, new DepDoubleItemsWatcherFct());
    AttrWatcherRegistrar::add(Core::AttrInputType::PageList, new PageListWatcherFct());
}


}
}
