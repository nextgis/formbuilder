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

#include "mockup/elemview.h"
#include "mockup/container.h"

#include <QStackedWidget>

namespace Fb
{
namespace Mockup
{

using TabsViewDecorData = QPair<QList<QString>, int>;

class TabsView: public ElemView
{
    Q_OBJECT

    public:

     explicit TabsView (const Core::Elem *elem);
     virtual ~TabsView ();

     virtual const Container *getContainer () const override;
     virtual const QVariant getDataForDecor () const override;

     const QList<Container*> &getAllContainers () const { return containers; }

 //   protected:

     virtual void atElemChange (Core::Attr *attr) override;

     void appendPage ();
     void removeLastPage ();
     int getPageIndex (Core::Attr *attr_cur_page);

     QStackedWidget *stack_main;

     QList<Container*> containers;
     int cur_container_index;

     QList<QString> headers;
};

FB_ELEMVIEW_FACTORY(TabsView, TabsViewFct)


}
}
