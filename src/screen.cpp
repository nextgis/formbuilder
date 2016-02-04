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

QList<QPair<QString,QString> > FBWorkingArea::STYLES;
QList<QPair<QString,QString> > FBWorkingArea::TYPES;
QList<QString> FBWorkingArea::RATIOS;
QList<QString> FBWorkingArea::RESOLS;

void FBWorkingArea::initAll ()
{
    STYLES.append(QPair<QString,QString>(FB_SCREEN_ANDROID,
                                         ":/img/android.png"));
    STYLES.append(QPair<QString,QString>(FB_SCREEN_QGIS,
                                         ":/img/qgis.png"));
    STYLES.append(QPair<QString,QString>(FB_SCREEN_WEB,
                                         ":/img/web.png"));

    TYPES.append(QPair<QString,QString>(FB_SCREEN_MAXIMIZED,
                                        ":/img/maximized.png"));
    TYPES.append(QPair<QString,QString>(FB_SCREEN_PHONEPORT,
                                        ":/img/phone_port.png"));
    TYPES.append(QPair<QString,QString>(FB_SCREEN_PHONELAND,
                                        ":/img/phone_land.png"));
    TYPES.append(QPair<QString,QString>(FB_SCREEN_TABLETPORT,
                                        ":/img/tablet_port.png"));
    TYPES.append(QPair<QString,QString>(FB_SCREEN_TABLETLAND,
                                        ":/img/tablet_land.png"));

    RATIOS.append("15:9");
    RATIOS.append("16:9");
    RATIOS.append("3:4");

    RESOLS.append("480x800");
    RESOLS.append("1366x768");
    RESOLS.append("1280x1024");
}
 

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
            glWorkingArea->addWidget(w,i,j); // will be 7 items in array
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

    // We must set default style at the construction time, becuse
    // it will be no project rendering at first and at the same time
    // no style of target device specified.
    this->changeStyle("default"); // just any string to set defaults
    this->changeType("default");
}



void FBWorkingArea::changeStyle (QString styleStr)
{
    if (style == FB_SCREEN_ANDROID)
    {

    }

    // ...

    else // default style
    {
        //labsScreenDecor.clear();
        wScreen->setStyleSheet("QWidget {background-color: "
                               +QString(FB_COLOR_LIGHTGREY)+";"
                               "border-top-left-radius: 4px;"
                               "border-top-right-radius: 4px;"
                               "border-bottom-left-radius: 4px;"
                               "border-bottom-right-radius: 4px;}");
    }

    this->style = styleStr;
}


void FBWorkingArea::changeType (QString typeStr)
{
    if (typeStr == FB_SCREEN_PHONEPORT
            || typeStr == FB_SCREEN_TABLETPORT)
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

    else if (typeStr == FB_SCREEN_PHONELAND)
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

    //...

    else // default type = FB_SCREEN_MAXIMIZED
    {
        for (int i=0; i<wsWorkingArea.size(); i++)
        {
            //wsWorkingArea[i]->setMinimumSize(0,0);
            wsWorkingArea[i]->setSizePolicy(QSizePolicy::Minimum,
                                            QSizePolicy::Minimum);
        }
    }

    this->type = typeStr;
}


void FBWorkingArea::changeRatio (int index)
{

}


void FBWorkingArea::changeResol (int index)
{

}





