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

FBScreenIos::FBScreenIos (QWidget* parent):
    FBScreenMobile(parent)
{
    devices.append(FBDeviceMobile(
                   QPair<int,int>(640,960),3.5,-1.0,"iPhone 4",""));
}

FBScreenIos::~FBScreenIos ()
{
}

void FBScreenIos::updateStyle ()
{
    // See FBScreenAndroid.
    for (int i=0; i<labsScreenDecorVert.size(); i++)
    {
        lvMain1->removeWidget(labsScreenDecorVert[i]);
        delete labsScreenDecorVert[i];
    }
    labsScreenDecorVert.clear();
    for (int i=0; i<labsScreenDecorHor.size(); i++)
    {
        lhMain->removeWidget(labsScreenDecorHor[i]);
        delete labsScreenDecorHor[i];
    }
    labsScreenDecorHor.clear();

    this->setStyleSheet("QWidget {background-color: "
                           +QString(FB_COLOR_LIGHTGREY)+";"
                           "border-top-left-radius: 4px;"
                           "border-top-right-radius: 4px;"
                           "border-bottom-left-radius: 4px;"
                           "border-bottom-right-radius: 4px;}");

    if (formPtr != NULL)
        formPtr->updateStyle(FB_STYLENAME_IOS);
}
