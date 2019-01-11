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
#include "decorator.h"

#include <QVBoxLayout>

namespace Fb
{
namespace Mockup
{

class Space;
class ElemView;
class ElemFactory;
class Container;


const float SCREEN_SIZE_FACTOR = 130.0f; // heuristically selected value for "life-size" screen

/*!
 * @brief A mockup's screen. A purpose of Screen is to allow user to place and move elements on
 * it. Screen always holds only one "top-level" element which cannot be moved by user and which
 * contains all other elements. Technically a Screen is a valid Qt-widget hierarchy which consists
 * of the following "types" of widgets: Space, Container, ElemView and "decorations" (any other
 * widgets but which are situated in right places). See the according classes for details.
 */
class Screen final: public CustomWidget
{
    Q_OBJECT // at least for qobject_cast

    public:

     static QPair<int, int> calculateSize (float width, float height, float diagonal,
                                           float zoom_factor = 1.0f);

     explicit Screen (QWidget *parent);
     Screen () = delete;
     ~Screen ();

     inline const ElemView *getTopLevelElemView () const { return top_level_elemview; }
     inline const ElemView *getSelectedElemView () const { return elemview_selected; }

     void setDecorator (Decorator *new_decorator);
     bool setTopLevelElemView (ElemView *elemview);

     bool isElemViewSelected () const;
     bool isElemViewMoving () const;
     bool isTopLevelElemViewSelected () const;
     bool isSelectedElemViewAContainer () const;
     bool hasElemsExceptTopLevel () const;

     void deselectElemView ();

     void removeSelectedElemView ();

     void form_pickLastSpace ();
     void form_pickLastSpaceInContainer (ElemView *container_elem);
     void form_setCanScrollToBottom ();

     void startMoveNewElemView ();
     void doMoveNewElemView (const QPoint &coords);
     void endMoveNewElemView (ElemView *new_elemview);

    signals:

     void afterElemViewAdded (ElemView *elemview);
     void afterElemViewSelected (ElemView *elemview);
     void afterElemViewChanged (ElemView *elemview);
     void afterElemViewMoved (ElemView *elemview);
     void afterElemViewDeleted ();

    private:

     void mousePressEvent (QMouseEvent *event) override;
     void mouseMoveEvent (QMouseEvent *event) override;
     void mouseReleaseEvent (QMouseEvent *event) override;

     inline Space *getSpaceUnderCoords (const QPoint &coords);
     inline void highlightSpace (Space *space);
     inline Space *dehighlightSpace ();

     void beforeAddNewElemView (ElemView *elemview);

     QVBoxLayout *lay_this;

     ElemView *top_level_elemview;

     ElemView *elemview_selected;
     bool is_elemview_selected_moving;
     Space *space_showed;

     bool is_new_elemview_moving;

     Decorator *decorator_ptr;

    private slots:

     void onElemViewHasChanged (ElemView *elemview);
};


}
}
