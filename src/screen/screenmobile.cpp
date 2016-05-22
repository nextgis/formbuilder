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

}

FBScreenMobile::~FBScreenMobile ()
{
}

void FBScreenMobile::changeDevice (int index)
{
    // TODO: check indexes.

    curDevice = index;

    if (index == 0)
    {
        FBScreen::changeDevice(0);
    }
    else
    {
        this->changeState(0); // always the first occur state
    }
}

void FBScreenMobile::changeState (int index)
{
    // TODO: check indexes.

    curState = index;

    int width;
    int height;

    width = devices[curDevice].getResolution().first;
    height = devices[curDevice].getResolution().second;

    QPair<int,int> realSize = FBScreenMobile::calculateScreenSize(
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

    // TODO: switch to another form (second set of elems) here.
}


QPair<int,int> FBScreenMobile::calculateScreenSize (float d, float w, float h)
{
    QPair<int,int> ret;
    ret.first = 0;
    ret.second = 0;
    if (d <= 0 || w <= 0 || h <= 0)
        return ret;
    float r; // aspect ratio
    if (w > h)
        r = w/h;
    else
        r = h/w;
    float x, y;
    y = sqrt(d*d/(r*r+1));
    x = r*y;
    return QPair<int,int>(x*FB_COEFF_SCREENSIZE,y*FB_COEFF_SCREENSIZE);
}




