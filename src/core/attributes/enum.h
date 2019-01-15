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

#include "core/attr.h"

namespace Fb
{
namespace Core
{


class Enum: public Attr
{
    Q_OBJECT

    public:

     explicit Enum (QString key_name, const QStringList &values_range, int init_value_index,
                    AttrInputType input_type = AttrInputType::Enum);
     virtual ~Enum ();

     QVariant getValueAsVar () const;
     virtual void setValueAsVar (const QVariant &new_value) override;

     int getValue () const;
     void setValue (int new_value_index);

     const QStringList &getValuesRange () const { return values_range; }

    protected:

     bool isValueIndexInRange ();

     const QStringList values_range; // is constant in this attribute
     int value_index; // will be changed in this attribute
};


}
}
