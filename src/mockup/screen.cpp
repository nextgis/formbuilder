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

#include "screen.h"

#include "space.h"
#include "elemview.h"
#include "container.h"

#include "elemviews/ngmform_view.h"

#include <QtMath>

#include <QMouseEvent>

using namespace Fb;
using namespace Mockup;


/*!
 * |brief Calculates the actual screen size best fitted for the current application window, using
 * the passed diagonal and screen resolutions size. Diagonal is used to determine the screen size
 * while the resolution - to determine screen's aspect ratio. For example: two devices with DPI = 1
 * and DPI = 2 but with the same diagonal will have the same screen size.
 * Zoom factor is used for optional screen scaling.
 * In the returned pair the first value is always larger than the second.
 * The pair [0, 0] is returned if incorrect args are passed.
 */
QPair<int, int> Screen::calculateSize (float width, float height, float diagonal,
                                       float zoom_factor) // STATIC
{
    QPair<int, int> size = {0, 0};

    if (diagonal <= 0.0f || width <= 0.0f || height <= 0.0f)
        return size;

    float ratio;
    if (width > height)
        ratio = width / height;
    else
        ratio = height / width;

    float y = qSqrt(diagonal * diagonal / (ratio * ratio + 1));
    float x = ratio * y;

    size.first = (int)(x * SCREEN_SIZE_FACTOR * zoom_factor);
    size.second = (int)(y * SCREEN_SIZE_FACTOR * zoom_factor);

    return size;
}


Screen::Screen (QWidget *parent):
    CustomWidget(parent),
    top_level_elemview(nullptr),
    elemview_selected(nullptr),
    is_elemview_selected_moving(false),
    space_showed(nullptr),
    is_new_elemview_moving(false),
    decorator_ptr(nullptr)
{
    lay_this = new QVBoxLayout(this);
    lay_this->setContentsMargins(0, 0, 0, 0);
    lay_this->setSpacing(0);
}

Screen::~Screen ()
{
}


/*!
 * \brief Set decorator and does NOT take ownership on it.
 */
void Screen::setDecorator (Decorator *new_decorator)
{
    decorator_ptr = new_decorator;
}


/*!
 * \brief Set a top-level (and probably a full-screen) elemview which will be a parent for all other
 * elemviews. Only this single elemview will have the Screen widget as its parent (all other
 * elemviews will belong to the Container widgets of each other).
 */
bool Screen::setTopLevelElemView (ElemView *elemview)
{
    if (top_level_elemview != nullptr)
        return false;
    if (elemview == nullptr)
        return false;

    this->beforeAddNewElemView(elemview);

    elemview->setParent(this);
    lay_this->addWidget(elemview);

    top_level_elemview = elemview;
    return true;
}


bool Screen::isElemViewSelected () const
{
    return elemview_selected == nullptr ? false : true;
}

bool Screen::isElemViewMoving () const
{
    return is_elemview_selected_moving || is_new_elemview_moving;
}

bool Screen::isTopLevelElemViewSelected () const
{
    return elemview_selected == top_level_elemview ? true : false;
}

bool Screen::isSelectedElemViewAContainer () const
{
    if (elemview_selected == nullptr)
        return false;
    return elemview_selected->getContainer() == nullptr ? false : true;
}

bool Screen::hasElemsExceptTopLevel () const
{
    if (top_level_elemview == nullptr)
        return false;
    const Container *container = top_level_elemview->getContainer();
    if (container == nullptr)
        return false;
    return container->hasElemViews();
}


/*!
 * \brief Deselect currently selected elemview.
 */
void Screen::deselectElemView ()
{
    if (is_elemview_selected_moving || is_new_elemview_moving)
        return;
    if (elemview_selected == nullptr)
        return;

    elemview_selected->setDeselectStyle();

    elemview_selected = nullptr;
}


/*!
 * \brief Delete currently selected elemview with all its inner elemviews (if there are some).
 */
void Screen::removeSelectedElemView ()
{
    if (is_elemview_selected_moving || is_new_elemview_moving)
        return;
    if (elemview_selected == nullptr)
        return;
    if (elemview_selected == top_level_elemview)
        return;

    // Note: here we delete an elemview with all its child elemviews, containers and spaces. The
    // according warning message should be already shown to the user in GUI.
    Container *container = static_cast<Container*>(elemview_selected->parent());
    container->takeElemView(elemview_selected);
    delete elemview_selected;
    elemview_selected = nullptr;

    emit afterElemViewDeleted();
}


/*!
 * @brief Specific method which picks last space in a "form" elemview. Can be used to add elemviews
 * via Control button in the main window GUI. Do not use in other circumstances.
 */
void Screen::form_pickLastSpace ()
{
    space_showed = static_cast<NgmFormView*>(top_level_elemview)->getContainer()->getLastSpace();
}

void Screen::form_pickLastSpaceInContainer (ElemView *container_elem)
{
    const Container *container = container_elem->getContainer();
    if (container == nullptr)
        return;
    space_showed = container->getLastSpace();
}

/*!
 * @brief Use to scroll top-level "form" widget to bottom. Do not use in other circumstances.
 */
void Screen::form_setCanScrollToBottom ()
{
    static_cast<NgmFormView*>(top_level_elemview)->setCanScrollToBottom(true);
}


// See mousePressEvent().
void Screen::startMoveNewElemView ()
{
    is_new_elemview_moving = true;
}

// See mouseMoveEvent().
void Screen::doMoveNewElemView (const QPoint &coords)
{
    if (!is_new_elemview_moving)
        return;

    QPoint screen_coords = this->mapFromGlobal(coords);

    Space *space = getSpaceUnderCoords(screen_coords);
    if (space == nullptr)
        return;
    if (space == space_showed)
        return;

    this->highlightSpace(space);
}

// See mouseReleaseEvent().
void Screen::endMoveNewElemView (ElemView *new_elemview)
{
    is_new_elemview_moving = false;

    if (space_showed == nullptr)
        return;
    Space *space_found = dehighlightSpace();

    if (new_elemview == nullptr)
        return;

    this->beforeAddNewElemView(new_elemview);

    Container *container = static_cast<Container*>(space_found->parent());
    bool ok = container->insertElemView(new_elemview, space_found);
    if (ok)
        emit afterElemViewAdded(new_elemview);
}


// Mouse events of the Screen widget is a final point where mouse events of all ElemView and
// Container widgets are captured and handled: thus an elemview's manipulation with mouse on the
// Screen is achieved. See ElemView::mousePressEvent() where the chain of mouse events is started.
void Screen::mousePressEvent (QMouseEvent *event)
{
    if (ElemView::pressed == nullptr)
        return;

    if (elemview_selected != nullptr)
        elemview_selected->setDeselectStyle();
    ElemView::pressed->setSelectStyle();

    elemview_selected = ElemView::pressed;
    is_elemview_selected_moving = true;

    emit afterElemViewSelected(elemview_selected);
}

// See mousePressEvent() for explanation.
void Screen::mouseMoveEvent (QMouseEvent *event)
{
    if (!is_elemview_selected_moving)
        return;

    QPoint coords = this->mapFromGlobal(QPoint(event->globalX(), event->globalY()));

    // Search for any visible Space widget which is under the cursor now.
    Space *space = getSpaceUnderCoords(coords);
    if (space == nullptr)
        return;
    if (space == space_showed)
        return;

    // Avoid showing selected spaces in inner containers of elemviews which are children (of any
    // level) of an elemview which is currently being selected. This check is only for elemviews
    // which have container inside self.
    Container *container = static_cast<Container*>(space->parent());
    if (elemview_selected->getContainer() != nullptr && elemview_selected->contains(container))
        return;

    // We found an approppriate space for elemview insertion.
    this->highlightSpace(space);
}

// See mousePressEvent() for explanation.
void Screen::mouseReleaseEvent (QMouseEvent *event)
{
    is_elemview_selected_moving = false;

    if (space_showed == nullptr)
        return;
    Space *space_found = this->dehighlightSpace();

    // Get containers for elemview which we want to move and container where we want to place an
    // elemview. This can be the same container.
    Container *container_of_elem_selected = static_cast<Container*>(elemview_selected->parent());
    Container *container_of_space_found = static_cast<Container*>(space_found->parent());

    // Check that we do not insert right near the selected elemview (reason: the near space widget
    // will be deleted in takeElemView() method while we will try to use it in insertElemView()).
    if (container_of_elem_selected == container_of_space_found)
        if (container_of_elem_selected->isNear(elemview_selected, space_found))
            return;

    // Finally "move" elemview to another place.
    container_of_elem_selected->takeElemView(elemview_selected);
    bool ok = container_of_space_found->insertElemView(elemview_selected, space_found);
    if (ok)
        emit afterElemViewMoved(elemview_selected);
}


// Some common steps for define a child widget under mouse coords:
Space *Screen::getSpaceUnderCoords (const QPoint &coords)
{
    QWidget *widget = this->childAt(coords);
    Space *space = qobject_cast<Space*>(widget);
    if (space != nullptr)
        return space;

    // Additional logic: if there is a container under cursor - suggest some space. This is useful
    // when there is a "void" screen and user tries to add new elemviews.
    Container *container = qobject_cast<Container*>(widget);
    if (container != nullptr)
        return container->getLastSpace();

    return nullptr;
}

// Some common steps for highlighting space:
void Screen::highlightSpace (Space *space)
{
    if (space_showed != nullptr)
        space_showed->setHideStyle();
    space->setShowStyle();
    space_showed = space;
}

// Some common steps for dehighlighting space:
Space *Screen::dehighlightSpace ()
{
    space_showed->setHideStyle();
    Space *space_found = space_showed;
    space_showed = nullptr;
    return space_found;
}


// Some common steps before add new elemview into the screen:
void Screen::beforeAddNewElemView (ElemView *elemview)
{
    elemview->atElemChange(nullptr); // initial update of an elem view: get initial look

    connect(elemview, &ElemView::hasChanged, this, &Screen::onElemViewHasChanged);

    if (decorator_ptr != nullptr)
    {
        decorator_ptr->decorate(elemview);
        decorator_ptr->update(elemview);
    }
}


// Some attributes were changed in the elemview and we must update its appearance using the screen's
// decorator.
void Screen::onElemViewHasChanged (ElemView *elemview)
{
    if (decorator_ptr != nullptr)
    {
        decorator_ptr->update(elemview);
    }

    emit afterElemViewChanged(elemview);
}



