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


// Specific attribute for stroring selected text from global list of string values. Used for Counter
// element.
class GlobalTextEnum: public Attr
{
    Q_OBJECT

    public:

     explicit GlobalTextEnum (QString key_name,
                              AttrInputType input_type = AttrInputType::GlobalTextEnum);
     virtual ~GlobalTextEnum () override;

     QVariant getValueAsVar () const override;
     virtual void setValueAsVar (const QVariant &value) override;

     QString getValue () const;
     void setValue (QString value_text);
     void resetValue ();

     static const QStringList &getValuesRange () { return s_values_range; }
     static void setValuesRange (const QStringList &values_range) { s_values_range = values_range; }

    protected:

     int findValueTextInRange (QString value_text) const;

     static QStringList s_values_range;
     QString m_value_text;
};


}
}
