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


struct TripleItemsValue
{
    QStringList inners;
    QStringList outers_left;
    QStringList outers_right;
    int def_index;
};

class TripleItems: public Attr
{
    Q_OBJECT

    public:

     explicit TripleItems (QString key_name, const TripleItemsValue &init_value,
                           int min_size = ATTR_STRLIST_DEF_MIN_SIZE,
                           int max_size = ATTR_STRLIST_DEF_MAX_SIZE,
                           bool must_have_default = false);
     virtual ~TripleItems ();

     virtual QVariant getValueAsVar () const override;
     const TripleItemsValue &getValue () const;
     void setValue (const TripleItemsValue &new_value);

     inline int getMinSize () const { return min_size; }
     inline int getMaxSize () const { return max_size; }
     inline bool getMustHaveDefault () const { return must_have_default; }

    protected:

     TripleItemsValue value;
     int min_size;
     int max_size;
     bool must_have_default;
};


}
}

Q_DECLARE_METATYPE(Fb::Core::TripleItemsValue)



