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

#include "elemview.h"

#include <QObject>
#include <QMap>

namespace Fb
{
namespace Mockup
{


class ElemViewRegistrar final
{
    public:

     static void add (QString elem_key_name, ElemViewFct *fct);
     static const ElemViewFct *get (QString elem_key_name);

    private:

     static ElemViewFct* def_elemview_fct; // TODO: smart pointer
     static QMap<QString, ElemViewFct*> elemview_fcts; // TODO: smart pointer
};


}
}



