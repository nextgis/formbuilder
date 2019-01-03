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

#include "screens.h"


/****************************************************************************/
/*                             FBScreenMobile                               */
/****************************************************************************/

FBScreenMobile::FBScreenMobile (QWidget *parent, float sizeFactor):
    FBScreen (parent,sizeFactor)
{

}

FBScreenMobile::~FBScreenMobile ()
{
}

void FBScreenMobile::clearDecor ()
{
    for (int i=0; i<labsScreenDecorVert.size(); i++)
    {
        lvMain2->removeWidget(labsScreenDecorVert[i]);
        delete labsScreenDecorVert[i];
    }
    labsScreenDecorVert.clear();
    for (int i=0; i<labsScreenDecorHor.size(); i++)
    {
        lhMain1->removeWidget(labsScreenDecorHor[i]);
        delete labsScreenDecorHor[i];
    }
    labsScreenDecorHor.clear();
}

void FBScreenMobile::changeDevice (int index)
{
    // TODO: check indexes.

    curDevice = index;

    if (index == 0)
    {
        FBScreen::changeDevice(0);
    }

    this->changeState(0); // always the first occur state
}

void FBScreenMobile::changeState (int index)
{
    // TODO: check indexes.

    curState = index;

    if (curDevice != 0)
    {
        int width;
        int height;

        width = devices[curDevice].getResolution().first;
        height = devices[curDevice].getResolution().second;

        QPair<int,int> realSize = this->calculateScreenSize(
                devices[curDevice].getDiagonal(),width,height);

        if (index == 0)
        {
            this->setFixedWidth(realSize.second);
            this->setFixedHeight(realSize.first);
        }
        else
        {
            this->setFixedWidth(realSize.first);
            this->setFixedHeight(realSize.second);
        }
    }

    // TODO: switch to another form (second set of elems) here.
}



