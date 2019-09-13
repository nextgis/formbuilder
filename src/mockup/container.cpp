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

#include "container.h"

#include "space.h"
#include "elemview.h"

#include <QMouseEvent>

using namespace Fb;
using namespace Mockup;


Container::Container (QWidget *parent, ContainerType type, ElemView *owner_elemview):
    CustomWidget(parent),
    type(type),
    owner_elemview(owner_elemview)
{
    if (owner_elemview == nullptr)
        throw; // TODO: named exception

    this->setObjectName("w_container");

    lay_main = new QVBoxLayout(this);
    lay_main->setContentsMargins(0, 0, 0, 0);
    lay_main->setSpacing(0);

    // Add the first insert space.
    if (type == ContainerType::Column)
    {
        // For column type add one horizontal space and one stretch after it.
        Space *space = new Space(this, SpaceType::Horizontal);
        lay_main->addWidget(space);
        lay_main->addStretch();
    }
    else
    {
        // ...
    }

    this->setStyleSheet("QWidget#w_container { background: transparent; }");
}

Container::~Container ()
{
}


// Get some "last" space of the container: e.g. for column container it is a last bottom elemview.
Space *Container::getLastSpace () const
{
    if (type == ContainerType::Column)
    {
        QWidget* w = lay_main->itemAt(lay_main->count() - 2)->widget(); // -2 because of a stretch
        return qobject_cast<Space*>(w);
    }
    else
    {
        // ...
    }

    return nullptr;
}


/*!
 * \brief Add elemview at the end of the container. The method will select the appropriate space to
 * insert depending on the type of the container (e.g. if the container is of "column" type - the
 * elemview will be inserted at the bottom of the column).
 */
bool Container::appendElemView (ElemView *elemview)
{
    Space *space = nullptr;

    if (type == ContainerType::Column)
    {
        space = this->getLastSpace();
    }
    else
    {
        // ...
    }

    if (space != nullptr)
        return this->insertElemView(elemview, space);
    return false;
}


/*!
 * \brief Insert the given elemview next to the given space (the position depends on the type of the
 * container). Note: any logical checks for a type of the inserted elemview must be made in the
 * according methods of concrete elems which own this container.
 */
bool Container::insertElemView (ElemView *elemview, Space *space)
{
    if (elemview == nullptr || space == nullptr)
        return false;
    if (elemview->parent() != nullptr) // insert only orphan element
        return false;
    if (elemview == owner_elemview) // this is not an attempt to add the owner elem
        return false;
    if (space->parent() != this) // the space must belong to this container
        return false;

    int space_index = lay_main->indexOf(space);
    int elem_new_index = space_index + 1;
    int space_new_index = space_index + 2;

    elemview->setParent(this);

//    elem->level = this->getOwnerElem()->level + 1;

    // Also insert new space (spaces) after (around) the added elem. This depends on the type of the
    // container.
    if (type == ContainerType::Column)
    {
        lay_main->insertWidget(elem_new_index, elemview);
        Space *new_space = new Space(this, SpaceType::Horizontal);
        lay_main->insertWidget(space_new_index, new_space);
    }
    else
    {
        // ...
    }

    return true;
}


/*!
 * \brief Remove elemview from the container but does not delete it. An elemview's parent is set to
 * nullptr.
 */
void Container::takeElemView (ElemView *elemview)
{
    if (elemview == nullptr)
        return;
    if (elemview->parent() != this)
        return;

    // Find space widget (or widgets) followed this elem and delete it (them).
    if (type == ContainerType::Column)
    {
        int space_index = lay_main->indexOf(elemview) + 1;
        QWidget *widget = lay_main->itemAt(space_index)->widget();
        Space *space = qobject_cast<Space*>(widget);
        lay_main->removeWidget(space);
        delete space;
    }
    else
    {
        // ...
    }

    lay_main->removeWidget(elemview);
    elemview->setParent(static_cast<Container*>(nullptr));
}


/*!
 * \brief Return true if the given elemview and space are near each other.
 */
bool Container::isNear (ElemView *elemview, Space *space) const
{
    if (elemview == nullptr || space == nullptr)
        throw; // TODO: named exception
    if (elemview->parent() != this || space->parent() != this)
        throw; // TODO: named exception

    int elem_index = lay_main->indexOf(elemview);
    int space_index = lay_main->indexOf(space);

    if (elem_index == -1 || space_index == -1)
        throw; // TODO: named exception

    if (type == ContainerType::Column)
    {
        int diff = elem_index - space_index;
        if (diff == -1 || diff == 1)
            return true;
    }
    else
    {
        // ...
    }

    return false;
}


/*!
 * \brief Return true if the container contains any ElemView widgets.
 */
bool Container::hasElemViews () const
{
    if (type == ContainerType::Column)
    {
        for (int i = 0; i < lay_main->count(); i++)
        {
            QWidget *widget = lay_main->itemAt(i)->widget();
            ElemView *elemview = qobject_cast<ElemView*>(widget);
            if (elemview != nullptr)
                return true;
        }
        return false;
    }
    else
    {
        // ...
    }

    return false;
}


// Fall to the parent. Parent is always either an another Elem (see its according event) or Elem's
// decorational widget (an event will be automatically passed through any amount of decor widgets).
void Container::mousePressEvent (QMouseEvent *event)
{
    event->ignore();
}

// See mousePressEvent() for explanation.
void Container::mouseMoveEvent (QMouseEvent *event)
{
    event->ignore();
}

// See mousePressEvent() for explanation.
void Container::mouseReleaseEvent (QMouseEvent *event)
{
    event->ignore();
}



