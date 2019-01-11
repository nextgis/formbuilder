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

#include "elemview.h"

namespace Fb
{
namespace Mockup
{


/*!
 * \brief A class which has two main methods: decorate() and update(). Each concrete decorator
 * defines how to initially decorate (create a set of "decoration" widgets) and later update that
 * decoration (change a look of those widgets at the elemview changes). Note: each QWidget which has
 * an ElemView as its parent and is not of a Container type is regarded as a "decoration" widget.
 */
class Decorator: public QObject // inherits QObject at least for tr() context in derived classes
{
    Q_OBJECT

    public:

     Decorator ();
     virtual ~Decorator ();

     virtual void decorate (ElemView *elemview) = 0;
     virtual void update (ElemView *elemview) = 0;

//     void clean (Elem *elem);
};


}
}
