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

namespace Fb
{
namespace Mockup
{

class Container;


const int SEL_BORDER_SIZE = 2; // common size of a selection border
const int SEL_SPACE_SIZE = 6;  // common size (width or height) of a space (must include border)

enum class SpaceType
{
    Horizontal,
    Vertical
};


/*!
 * @brief A "space" is a widget which is situated in a container and "shows" a place where an
 * elemview can be inserted (by holding it with mouse) in this container.
 */
class Space final: public CustomWidget
{
    friend class Screen;
    friend class Container;

    Q_OBJECT // at least for qobject_cast

    public:

     explicit Space (Container *container, SpaceType type);
     Space () = delete;
     Space (QWidget *) = delete; // parent widget can only be of a specific type
     ~Space ();

     void setParent (QWidget *) = delete; // parent widget can only be of a specific type
     void setParent (Container *container)
     {
         reinterpret_cast<QWidget*>(this)->setParent(reinterpret_cast<QWidget*>(container));
     }

     void setShowStyle ();
     void setHideStyle ();
};


}
}
