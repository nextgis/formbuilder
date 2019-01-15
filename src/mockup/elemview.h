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

#include "custom_widget.h"
#include "core/elem.h"

#include <QVBoxLayout>

namespace Fb
{
namespace Mockup
{

class Space;
class Container;
class Screen;


/*!
 * @brief ElemView is a QWidget which is placed to the Screen thus allowing user to change Elems'
 * position and to insert them into each other. Each ElemView is created based on the passed Elem.
 *
 * Note: this class is a part of "Factory method" design pattern implementation. See ElemViewFct
 * and ElemViewRegistrar classes.
 *
 * @see Core::Elem
 */
class ElemView: public CustomWidget
{
    friend class Container;
    friend class Screen;

    Q_OBJECT // at least for qobject_cast

    public:

     static ElemView *pressed;
     static QCursor last_cursor;

     ElemView () = delete;
     explicit ElemView (const Core::Elem *elem);
     ElemView (QWidget *) = delete; // parent widget can only be of a specific type
     virtual ~ElemView ();

     void setParent (QWidget *) = delete; // parent widget can only be of a specific type
     void setParent (Container *container)
     {
         reinterpret_cast<QWidget*>(this)->setParent(reinterpret_cast<QWidget*>(container));
     }
     void setParent (Screen *screen)
     {
         reinterpret_cast<QWidget*>(this)->setParent(reinterpret_cast<QWidget*>(screen));
     }

     inline QWidget *getBaseDecorWidget () const { return w_base; }
     inline QVBoxLayout *getBaseDecorLayout () const { return lay_base; } // a layout of w_base
     inline const QString &getKeyName () const { return elem->getKeyName(); }
     inline const Core::Elem *getElem () const { return elem; }

     bool contains (const Container *container) const;
     bool contains (const ElemView *elemview) const;

     virtual const Container *getContainer () const = 0;
     virtual const QVariant getDataForDecor () const { return QVariant(); }

    signals:

     void hasChanged (ElemView *elemview);
     void needToRemoveElem (const Core::Elem *elem);

    protected:

     virtual void atElemChange (Core::Attr *attr) = 0;

     void setSelectStyle ();
     void setDeselectStyle ();

     virtual void mousePressEvent (QMouseEvent *event) override;
     virtual void mouseMoveEvent (QMouseEvent *event) override;
     virtual void mouseReleaseEvent (QMouseEvent *event) override;

    protected slots:

     void onElemHasChanged (Core::Attr *attr);

    protected:

     QWidget *w_base;
     QVBoxLayout *lay_base;

     const Core::Elem *elem;
};


class ElemViewFct
{
    public:

     ElemViewFct () { }
     virtual ~ElemViewFct () { }

     virtual ElemView *create (const Core::Elem *elem) const = 0;
};


#define FB_ELEMVIEW_FACTORY(Class, ClassFactory) \
class ClassFactory: public ElemViewFct \
{ \
    public: \
     ClassFactory (): ElemViewFct() { } \
     virtual ~ClassFactory () { } \
     virtual ElemView *create (const Core::Elem *elem) const override \
     { \
         return new Class(elem); \
     } \
};


}
}
