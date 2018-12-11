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

#include "custom_spinbox.h"

#include <QLineEdit>

using namespace Fb;
using namespace Gui;


CustomSpinBox::CustomSpinBox ():
    QSpinBox()
{
}

CustomSpinBox::CustomSpinBox (QWidget *parent):
    QSpinBox(parent)
{
}

CustomSpinBox::~CustomSpinBox ()
{
}


void CustomSpinBox::focusInEvent (QFocusEvent *event)
{
    this->lineEdit()->setCursorPosition(this->lineEdit()->text().length());
    QSpinBox::focusInEvent(event);
}


