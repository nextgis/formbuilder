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

#include "elemview.h"

#include "screen.h"
#include "space.h"
#include "container.h"

#include <QVBoxLayout>
#include <QMouseEvent>

using namespace Fb;
using namespace Mockup;


ElemView *ElemView::pressed = nullptr;
QCursor ElemView::last_cursor;

ElemView::ElemView (const Core::Elem *elem):
    CustomWidget(),
    elem(elem)
{
    if (elem == nullptr)
        throw; // TODO: named exception

    connect(elem, &Core::Elem::hasChanged, this, &ElemView::onElemHasChanged);

    // The name of THIS widget should not be changed in derived classes because we need a correct
    // selection/deselection style applying. But one can change a specific object name for w_base
    // or any other decorational widgets which are child widgets for THIS widget (except the
    // specific Container and Space widgets).
    this->setObjectName("w_elem");

    w_base = new QWidget(this);
    w_base->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    lay_base = new QVBoxLayout(w_base);
    lay_base->setContentsMargins(0, 0, 0, 0);
    lay_base->setSpacing(0);

    QVBoxLayout *lay_this = new QVBoxLayout(this);
    lay_this->setContentsMargins(SEL_BORDER_SIZE, SEL_BORDER_SIZE,
                                 SEL_BORDER_SIZE, SEL_BORDER_SIZE);
    lay_this->setSpacing(0);
    lay_this->addWidget(w_base);

    // Note: actual size or size policy of this widget will be defined in the derived classes.
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    this->setDeselectStyle();
}

ElemView::~ElemView ()
{
    emit needToRemoveElem(elem);
}


/*!
 * \brief Recursively determine whether this elemview contains the given container in any of the
 * children (or in their children, etc) or in itself.
 */
bool ElemView::contains (const Container *container) const
{
    if (container == nullptr)
        return false;

    // We start from the container and go upwards.
    const ElemView *elemview = container->getOwnerElemView();
    if (elemview == this)
        return true;

    // Get parent container for the current element. If an element is a top-level element then
    // its parent is a Screen, not a Container => we reach the top of the hierarchy.
    container = qobject_cast<Container*>(elemview->parent());
    if (container == nullptr) // this can occur only if the parent is a Screen widget
        return false;

    return this->contains(container);
}

/*!
 * \brief Determine whether this elemview contains the given elemview in any of the children
 * (or in their children, etc) or in itself.
 */
bool ElemView::contains (const ElemView *elemview) const
{
    if (elemview == nullptr)
        return false;
    if (elemview == this)
        return false;

    // We start from the passed elemview and go up to this elemview.
    while (true)
    {
        QObject *parent = elemview->parent();
        if (parent == nullptr)
            return false;

        Screen *screen = qobject_cast<Screen*>(parent);
        if (screen != nullptr)
            return false; // we reach the top of the hierarchy

        Container *container = qobject_cast<Container*>(parent); // definitely a container
        elemview = container->getOwnerElemView();
        if (elemview == this)
            return true; // we found this elemview
    }
}


void ElemView::setSelectStyle ()
{
    QString style = QString("QWidget#w_elem {background: none; "
                            "border: %1px dashed red;}").arg(SEL_BORDER_SIZE);
    this->setStyleSheet(style);
}

void ElemView::setDeselectStyle ()
{
    QString style = QString("QWidget#w_elem {background: none; "
                            "border: %1px transparent;}").arg(SEL_BORDER_SIZE);
    this->setStyleSheet(style);
}


// Grab mouse and fall to the parent. The parent is always a Container or a Screen widget. Thus the
// chain of event ignoring is started where the final target is always Screen::<event>. We need to
// reach the Sreen's event in order to be able to "see" all its children via event coordinates and
// to finally pick an appropriate Space widget. Note: if the click was at decorational widget
// inside this elemview - it will be skipped automatically and fall down because decorational
// widgets have no mouse events reimplementations.
void ElemView::mousePressEvent (QMouseEvent *event)
{
    if (pressed == nullptr) // so not to rewrite it in the parent elems
        pressed = this;

    last_cursor = this->cursor();
    this->setCursor(Qt::ClosedHandCursor);

    event->ignore();
}

// See mousePressEvent() for explanation.
void ElemView::mouseMoveEvent (QMouseEvent *event)
{
    event->ignore();
}

// See mousePressEvent() for explanation.
void ElemView::mouseReleaseEvent (QMouseEvent *event)
{
    pressed = nullptr;

    this->setCursor(last_cursor);

    event->ignore();
}


void ElemView::onElemHasChanged (Core::Attr *attr)
{
    this->atElemChange(attr); // will be overriden

    emit hasChanged(this);
}



