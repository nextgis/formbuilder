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

#include "layer.h"
#include "elem.h"
//#include "layout.h"

#include <QScopedPointer>
#include <QSharedPointer>

namespace Fb
{
namespace Core
{

// GLOBAL TODO 1: create a complete Elem's hierarchy with layouts and all required methods to modify
// them. Use it in desktop applications along with Mockup's classes (currently only Mockup's
// classes are used e.g. in Serializer's methods to store Elems' mutual arrangement).
// Important: should be implemented via Model-View pattern where View is a Mockup library.

// GLOBAL TODO 2 (depends on 1): create a need_to_save flag and do all dirty-flag work inside
// Project class (and derived classes). For now this is implemented in FbWindow class.

/*
enum class LayoutType
{
    Undefined,
    None, // no container
    Vertical, // one vertical container
    Horizontal,
    Grid,
    TwoColumns,
    VerPages, // several vertical containers with one seen at time
    NgApp, // specific layout with predefined places for map, layres box, settings box, etc
};


class Layout: public QObject
{
    Q_OBJECT // for qobject_cast

    public:

     Layout (): QObject() { }
     virtual ~Layout() { }

     virtual bool insertElem (QObject *object) = 0;
     virtual bool insertElem (int row, int col, QObject *object) = 0;
     virtual bool insertElem (int page_index, QObject *object) = 0;
};

class VerLayout: public Layout
{
    Q_OBJECT // for qobject_cast

    public:

     VerLayout (): Layout() { }
     virtual ~VerLayout() { }

     QList<QObject*> contents;
};

class VerPagesLayout: public Layout
{
    Q_OBJECT // for qobject_cast

    public:

     VerPagesLayout (): Layout() { }
     virtual ~VerPagesLayout() { }

     QList<QList<QObject*>> contents;
};
*/


/*!
 * @brief A Project is a full set of things which user creates and modifies in this app in order
 * it all works properly on a target device during data collecting process. Any project at least
 * consists of layers (where data will be placed) and elements (with which help data will be
 * placed).
 * @see Core::Layer
 * @see Core::Elem
 */
class Project: public QObject
{
    Q_OBJECT

    public:

     Project ();
     virtual ~Project ();

     Layer *temp_getLayer () { return *layers.begin(); }

     void addElem (Elem *elem);
     void deleteElem (Elem *elem);

    public slots:

     void onNeedToRemoveElem (const Elem *elem);

    protected:

     bool is_initialized;

     QSet<Elem*> elems; // TODO: this should be in future Form class?
     QSet<Layer*> layers;
//     QList<List> lists;
//     QList<QImage> images;
};


}
}



