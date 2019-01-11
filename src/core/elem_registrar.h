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

#include "elem.h"

namespace Fb
{
namespace Core
{

enum class ElemCategory
{
    Undefined,
    Decoration,
    Input,
    Specific,
    Grouping,
    AppWidget,
    FullScreenWidget,
    Application
};

struct ElemData
{
    ElemCategory category;
    bool is_system; // user cannot create this element if true
    bool is_for_supported; // TODO: this should be moved to specific platform's info
    ElemFct *fct;  // TODO: smart pointer
    QString icon_file_name; // just file name without a path
    QString icon_dis_file_name;
    QString display_name;
    QString description;
};


class ElemRegistrar final
{
    public:

     static void add (QString key_name, ElemData elem_data);
     static const ElemData *get (QString key_name);
     static const std::map<QString, ElemData> &getAll ();

    private:

     static std::map<QString, ElemData> elements;
};


}
}



