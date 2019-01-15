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

#include <QScrollArea>

namespace Fb
{
namespace Mockup
{


class NgmFormView: public ElemView
{
    Q_OBJECT

    public:

     explicit NgmFormView (const Core::Elem *elem);
     virtual ~NgmFormView ();

     virtual const Container *getContainer () const override { return container; }

     void setCanScrollToBottom (bool can) { can_scroll_to_bottom = can; }

    protected slots:

     void onScrollBarRangeChanged (int, int);

    protected:

     virtual void atElemChange (Core::Attr *attr) override { Q_UNUSED(attr) }

     Container *container;

     QScrollArea *scr_main;
     bool can_scroll_to_bottom;
};

FB_ELEMVIEW_FACTORY(NgmFormView, NgmFormViewFct)


}
}
