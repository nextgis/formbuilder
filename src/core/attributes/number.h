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


class Number: public Attr
{
    Q_OBJECT

    public:

     explicit Number (QString key_name, int init_value, int min, int max,
                      AttrInputType input_type = AttrInputType::Number);
     virtual ~Number ();

     virtual QVariant getValueAsVar () const override;
     virtual void setValueAsVar (const QVariant &new_value) override;

     int getValue () const;
     void setValue (int new_value);

     inline int getMin () const { return min; }
     inline int getMax () const { return max; }

     void setMin (int new_min);
     void setMax (int new_max);

    protected:

     int value;
     int min;
     int max;
};


}
}
