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

#include <QObject>
#include <QVariant>

namespace Fb
{
namespace Core
{


const int ATTR_STRLIST_DEF_MIN_SIZE = 0;
const int ATTR_STRLIST_DEF_MAX_SIZE = 65536;
//const int ATTR_STRLIST_MAX_STRLEN = 256;

enum class AttrInputType
{
    Undefined,
    Boolean,
    String,
    Number,
    Enum,
    DateTime,
    StringList,
    PageList,
    DoubleItems,
    TripleItems,
    DepDoubleItems,
};


/*!
 * @brief Attribute is a wrapper (via aggregation) around some value which can be a property of an
 * Element and which should be used by user to tune this element. Each concrete attribute must have
 * a keyname and a type of input into itself. Usually Attrs are created inside Elem's constructor.
 *
 * @see Gui::AttrWatcher
 */
class Attr: public QObject
{
    Q_OBJECT

    public:

     Attr () = delete;
     explicit Attr (QString key_name, AttrInputType input_type = AttrInputType::Undefined);
     virtual ~Attr ();

     virtual QVariant getValueAsVar () const = 0;
//     virtual void setValueAsVar (const QVariant &new_value) = 0;

     inline const QString &getKeyName () const { return key_name; }
     inline AttrInputType getInputType () const { return input_type; }
     inline bool isEnabled () const { return is_enabled; }
     inline void setEnabled (bool b) { is_enabled = b; }

    protected:

     QString key_name;
     AttrInputType input_type;
     bool is_enabled;
};


}
}
