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

#include "elemview_registrar.h"
#include "elemviews/default_view.h"

using namespace Fb;
using namespace Mockup;


ElemViewFct* ElemViewRegistrar::def_elemview_fct = new DefaultViewFct();
QMap<QString, ElemViewFct*> ElemViewRegistrar::elemview_fcts;

void ElemViewRegistrar::add (QString elem_key_name, ElemViewFct *fct)
{
    if (elemview_fcts.contains(elem_key_name))
    {
        //qDebug
        return;
    }
    elemview_fcts.insert(elem_key_name, fct);
}

const ElemViewFct *ElemViewRegistrar::get (QString elem_key_name)
{
    if (!elemview_fcts.contains(elem_key_name))
    {
        //qDebug
        return def_elemview_fct;
    }
    return elemview_fcts.value(elem_key_name);
}



