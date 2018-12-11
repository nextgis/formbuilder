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

#include "space.h"

#include "container.h"

using namespace Fb;
using namespace Mockup;


Space::Space (Container *container, SpaceType type):
    CustomWidget(container)
{
    this->setObjectName("w_space");

    if (type == SpaceType::Horizontal)
    {
        this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        this->setFixedHeight(SEL_SPACE_SIZE);
    }
    else
    {
        this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        this->setFixedWidth(SEL_SPACE_SIZE);
    }

    this->setHideStyle();
}

Space::~Space ()
{
}


void Space::setShowStyle ()
{
    QString style = QString("QWidget#w_space { background: none; border: %1px dashed red; }")
                            .arg(SEL_BORDER_SIZE);
    this->setStyleSheet(style);
}

void Space::setHideStyle ()
{
    QString style = QString("QWidget#w_space { background: none; border: %1px transparent; }")
                            .arg(SEL_BORDER_SIZE);
    this->setStyleSheet(style);
}



