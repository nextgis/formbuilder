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

#include "elem.h"
#include "field_types.h"

#include <QObject>

namespace Fb
{
namespace Core
{

class Layer;


const int FIELD_NAME_MAX_LEN = 64;

/*!
 * Each field except name and type has a pointer to an element and its slot name to which it is
 * currently bound (nullptr and "" if this field is unbound). This is the only place where
 * the connection "element - field" is stored. Each element stores only binding permission
 * information for each slot, not the binding itself. Important: only one elem can be bound to each
 * single field (but an elem can have several slots => several bindings to different fields).
 */
class Field final: public QObject
{
    Q_OBJECT

    public:

     Field (Layer *owner, QString name, QString alias, FieldType type);
     ~Field ();

     inline QString getName () const { return name; }
     inline QString getAlias () const { return alias; }
     inline FieldType getType () const { return type; }
     inline void setType (FieldType new_type) { type = new_type; }
     inline const Elem *getElem () const { return elem; }
     inline QString getSlot () const { return slot; }

     bool isBound () const;
     bool canBind (const Elem *elem, QString slot) const;
     void bind (const Elem *new_elem, QString new_slot);
     void unbind ();

     static bool isNameCorrect (QString name);

    public slots:

     // TEMP:
     void onNeedToSetNewAlias (const QString &text);
     void onNeedToSetNewFieldType (int index);

    private:

     Layer *owner;
     QString name;
     QString alias;
     FieldType type;
     const Elem *elem;
     QString slot;
};


}
}



