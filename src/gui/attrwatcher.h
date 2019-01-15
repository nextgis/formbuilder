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
#include "mainwindow.h"

#include <QWidget>

namespace Fb
{
namespace Gui
{


/*!
 * @brief An AttrWatcher's main ability is to create a QWidget for inputting data into an attribute
 * (Core::Attr) based on its input type. These widgets are placed to the attributes table at
 * runtime.
 *
 * Note: this class is a part of "Factory method" design pattern implementation. See AttrWatcherFct
 * and AttrWatcherRegistrar classes.
 *
 * @see Core::Attr
 * @see Gui::PropsTableWidget
 */
class AttrWatcher: public QObject
{
    Q_OBJECT

    public:

     AttrWatcher () = delete;
     explicit AttrWatcher (Core::Attr *attr);
     virtual ~AttrWatcher ();

     virtual QWidget *createWidget (MainWindow *window = nullptr) const = 0;

     inline const Core::Attr *getAttr () const { return attr; }

    signals:

     void attrChangedByWidget (Core::Attr *attr);

    protected:

//     template<class T>
//     T* AttrWatcher::getConcrAttr (Core::Attr *attr)
//     {
//         T *concr_attr = qobject_cast<T*>(attr);
//         if (concr_attr == nullptr)
//         {
//             //qDebug
//             return nullptr;
//         }
//         return concr_attr;
//     }

     Core::Attr *attr;
};

class AttrWatcherFct
{
    public:

     AttrWatcherFct () { }
     virtual ~AttrWatcherFct () { }

     virtual AttrWatcher *create (Core::Attr *attr) const = 0;
};

#define FB_ATTRWATCHER_FACTORY(Class, ClassFactory) \
class ClassFactory: public AttrWatcherFct \
{ \
    public: \
     explicit ClassFactory (): AttrWatcherFct() { } \
     virtual ~ClassFactory () { } \
     virtual AttrWatcher *create (Core::Attr *attr) const override \
     { \
         return new Class(attr); \
     } \
};


}
}
