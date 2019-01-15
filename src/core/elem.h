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

#include "attr.h"
#include "field_types.h"

#include <QObject>
#include <QSet>

#include <set>

namespace Fb
{
namespace Core
{


/*!
 * @brief Element represents any visible feature (e.g. checkobx, form, map) that can be added and
 * tuned by user to/on a target application. Tuning is performed via attributes (see Core::Attr).
 * Each element has a set of attributes (which can be void) and a set of field slots for binding
 * with a layer in a Project (also can be void). Elems can be showed "on a screen" with the help
 * of the Mockup library.
 *
 * Note: this class is a part of "Factory method" design pattern implementation. See ElemFct and
 * ElemRegistrar classes.
 *
 * @see Mockup::ElemView
 */
class Elem: public QObject
{
    Q_OBJECT

    public:

     Elem () = delete;
     explicit Elem (QString key_name);
     virtual ~Elem ();

     inline const QString &getKeyName () const { return key_name; }
     inline const QMap<QString, Attr*> &getAttrs () const { return attrs; }
     inline const QMap<QString, std::set<FieldType>> &getFieldSlots () const { return field_slots; }

    public slots:

     void onAttrChanged (Attr *attr);

    signals:

     void hasChanged (Attr *attr);

    //protected:

     virtual void behave (Attr *attr = nullptr) = 0;

    protected:

     Attr* addAttr (Attr *attr);
     QString addFieldSlot(QString slot_key_name);
     void addTypeToFieldSlot (QString slot_key_name, FieldType field_type);
//     void removeTypeFromFieldSlot (QString slot_key_name, FieldType field_type);
     void clearTypesForFieldSlot (QString slot_key_name);

     QString key_name;

    private:

     QMap<QString, Attr*> attrs;
     QMap<QString, std::set<FieldType>> field_slots;
};

class ElemFct
{
    public:

     explicit ElemFct (QString key_name): key_name(key_name) { }
     virtual ~ElemFct () { }

     virtual Elem *create () const = 0;

    protected:

     QString key_name;
};

#define FB_ELEM_FACTORY(Class, ClassFactory) \
class ClassFactory: public ElemFct \
{ \
    public: \
     explicit ClassFactory (QString key_name): ElemFct(key_name) { } \
     virtual ~ClassFactory () { } \
     virtual Elem *create () const override { return new Class(key_name); } \
};


}
}



