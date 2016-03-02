/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  mobile screens implementations
 * Author:   Mikhail Gusev, gusevmihs@gmail.com
 ******************************************************************************
*   Copyright (C) 2014-2016 NextGIS
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 2 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#include "screen.h"


/****************************************************************************/
/*                             FBScreenMobile                               */
/****************************************************************************/

FBScreenMobile::FBScreenMobile (QWidget *parent):
    FBScreen (parent)
{
    // WARNING. Do not forget to change indexes in the state setting method
    // if editing this list.
    states.append(FBState(
          tr("Portrait"),tr("Portrait orientation"),":/img/phone_port.png"));
    states.append(FBState(
          tr("Landscape"),tr("Landscape orientation"),":/img/phone_land.png"));

    // TODO: remove maximized screen for phones here and in setState()?
}

FBScreenMobile::~FBScreenMobile ()
{
}

void FBScreenMobile::setDevice (int index)
{
    if (index < 0 || index > devices.size()-1)
            //|| index == curDevice)
        return;

    // Apply screen settings.


    curDevice = index;
}

void FBScreenMobile::setState (int index)
{
    if (index < 0 || index > states.size()-1)
            //|| index == curState)
        return;

    if (index == 1) // portrait
    {
        for (int i=0; i<3; i++) wsWorkingArea[i]->setSizePolicy(
                    QSizePolicy::Minimum, QSizePolicy::Minimum);
        for (int i=5; i<8; i++) wsWorkingArea[i]->setSizePolicy(
                    QSizePolicy::Minimum, QSizePolicy::Minimum);
        wsWorkingArea[3]->setSizePolicy(
                    QSizePolicy::Expanding, QSizePolicy::Expanding);
        wsWorkingArea[4]->setSizePolicy(
                    QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    else if (index == 2) // landscape
    {
        for (int i=0; i<3; i++) wsWorkingArea[i]->setSizePolicy(
                    QSizePolicy::Expanding, QSizePolicy::Expanding);
        for (int i=5; i<8; i++) wsWorkingArea[i]->setSizePolicy(
                    QSizePolicy::Expanding, QSizePolicy::Expanding);
        wsWorkingArea[3]->setSizePolicy(
                    QSizePolicy::Minimum, QSizePolicy::Minimum);
        wsWorkingArea[4]->setSizePolicy(
                    QSizePolicy::Minimum, QSizePolicy::Minimum);
    }

    // TODO: remove maximized screen for phones here?
    else
    {
        FBScreen::setState(0);
    }

    curState = index;
}

