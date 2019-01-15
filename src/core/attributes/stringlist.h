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

#include "core/attr.h"

namespace Fb
{
namespace Core
{


class StringList: public Attr
{
    Q_OBJECT

    public:

     explicit StringList (QString key_name, QStringList init_value,
                          int min_size = ATTR_STRLIST_DEF_MIN_SIZE,
                          int max_size = ATTR_STRLIST_DEF_MAX_SIZE,
                          AttrInputType input_type = AttrInputType::StringList);
     virtual ~StringList ();

     QVariant getValueAsVar () const;
     virtual void setValueAsVar (const QVariant &new_value) override;

     const QStringList &getValue () const;
     void setValue (QStringList new_value);

     inline int getMinSize () const { return min_size; }
     inline int getMaxSize () const { return max_size; }

    protected:

     QStringList value;
     int min_size;
     int max_size;
};


}
}
