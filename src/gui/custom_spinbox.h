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

#include <QSpinBox>

namespace Fb
{
namespace Gui
{


/*!
 * \brief We need this reimplementation of the QSpinBox in order to place the cursor to the end
 * each time QSpinBox receives focus (otherwise it is placed to the beginning of the line).
 */
class CustomSpinBox: public QSpinBox
{
    public:

     CustomSpinBox ();
     explicit CustomSpinBox (QWidget *parent);
     ~CustomSpinBox ();

     virtual void focusInEvent (QFocusEvent *event) override;
};


}
}
