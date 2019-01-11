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

#include "custom_widget.h"

#include <QVBoxLayout>

namespace Fb
{
namespace Mockup
{

class Space;
class ElemView;
class Screen;


enum class ContainerType
{
    Column,
//    Row,
//    Grid
};

/*!
 * \brief A "container's" main ability is to hold elemviews inside and to help the user to select
 * where to put new elemview (using "spaces"). An order and a type of arrangement of Space widgets
 * define the way the elements are arranged. A container always has an owner elemview (but it can be
 * not a direct parent in terms of QObject child-parent system).
 */
class Container final: public CustomWidget
{
    Q_OBJECT // at least for qobject_cast

    public:

     explicit Container (QWidget *parent, ContainerType type, ElemView *owner_elemview);
     Container () = delete;
     Container (QWidget *) = delete;
     ~Container ();

     inline const ElemView *getOwnerElemView () const { return owner_elemview; }

     Space *getLastSpace () const;

     bool appendElemView (ElemView *elemview);
     bool insertElemView (ElemView *elemview, Space *space);
     void takeElemView (ElemView *elemview);
     bool isNear (ElemView *elemview, Space *space) const;
     bool hasElemViews () const;

    private:

     void mousePressEvent (QMouseEvent *event) override;
     void mouseMoveEvent (QMouseEvent *event) override;
     void mouseReleaseEvent (QMouseEvent *event) override;

     QVBoxLayout *lay_main;
     ContainerType type;
     ElemView *owner_elemview; // can be not a direct parent of this Container widget
};


}
}
