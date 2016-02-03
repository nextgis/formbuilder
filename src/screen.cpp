/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  basic screen implementations
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
 
FBWorkingArea::FBWorkingArea (QWidget *parent): QWidget(parent)
{
    glWorkingArea = new QGridLayout(this);
    glWorkingArea->setContentsMargins(0,0,0,0);
    glWorkingArea->setSpacing(0);

    for (int i=0; i<3; i++)
    {
        for (int j=0; j<3; j++)
        {
            if (i == 1 && j == 1)
                continue;
            QWidget *w = new QWidget(this);
            w->setSizePolicy(QSizePolicy::Expanding,
                             QSizePolicy::Expanding);
            glWorkingArea->addWidget(w,i,j); // top row will be 0, 1, 2
            wsWorkingArea.append(w);
        }
    }

    wScreen = new QWidget(this);
    wScreen->setSizePolicy(QSizePolicy::Expanding,
                           QSizePolicy::Expanding);
    glWorkingArea->addWidget(wScreen,1,1);

    vlScreen = new QVBoxLayout(wScreen);
    vlScreen->setContentsMargins(0,0,0,0);
    vlScreen->setSpacing(0);

    scrollScreen = new QScrollArea(wScreen);
    scrollScreen->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Expanding);
    scrollScreen->setStyleSheet("QScrollArea{border: 0px;}");
    scrollScreen->setWidgetResizable(true);
    vlScreen->addWidget(scrollScreen);

    this->changeStyle(FBNoStyle);
    this->changeType(FBMaximized);
}



void FBWorkingArea::changeStyle (FBScreenStyle newStyle)
{
    if (style == FBAndroid)
    {

    }

    // ...

    else // FBNoStyle
    {
        //labsScreenDecor.clear();
        wScreen->setStyleSheet("QWidget {background-color: "
                               +QString(FB_COLOR_LIGHTGREY)+";"
                               "border-top-left-radius: 4px;"
                               "border-top-right-radius: 4px;"
                               "border-bottom-left-radius: 4px;"
                               "border-bottom-right-radius: 4px;}");
    }

    this->style = newStyle;
}


void FBWorkingArea::changeType (FBScreenType newType)
{
    if (newType == FBPhonePortrait)
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

    //...

    else // FBMaximized
    {
        for (int i=0; i<wsWorkingArea.size(); i++)
        {
            wsWorkingArea[i]->setMinimumSize(0,0);
            wsWorkingArea[i]->setSizePolicy(QSizePolicy::Minimum,
                                            QSizePolicy::Minimum);
        }
    }

    this->type = newType;
}





