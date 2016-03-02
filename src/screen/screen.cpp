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

FBScreen::FBScreen (QWidget *parent):
    QWidget(parent)
{
    lgWorkingArea = new QGridLayout(this);
    lgWorkingArea->setContentsMargins(0,0,0,0);
    lgWorkingArea->setSpacing(0);

    for (int i=0; i<3; i++)
    {
        for (int j=0; j<3; j++)
        {
            if (i == 1 && j == 1)
                continue;
            QWidget *w = new QWidget(this);
            w->setSizePolicy(QSizePolicy::Expanding,
                             QSizePolicy::Expanding);
            lgWorkingArea->addWidget(w,i,j); // will be 7 items in array
            wsWorkingArea.append(w);
        }
    }

    wScreen = new QWidget(this);
    wScreen->setSizePolicy(QSizePolicy::Expanding,
                           QSizePolicy::Expanding);
    lgWorkingArea->addWidget(wScreen,1,1);

    lvScreen= new QVBoxLayout(wScreen);
    lvScreen->setContentsMargins(0,0,0,0);
    lvScreen->setSpacing(0);

    scrollScreen = new QScrollArea(wScreen);
    scrollScreen->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Expanding);
    scrollScreen->setStyleSheet("QScrollArea{border: 0px;}");
    scrollScreen->setWidgetResizable(true);
    lvScreen->addWidget(scrollScreen);

    formPtr = NULL;

    // Defaults.
    devices.append(FBDevice(
                       QPair<int,int>(0,0),0.0,0.0,"Default",""));
    states.append(FBState(
                  tr("Maximized"),tr("Maximize"),":/img/maximized.png"));
    curDevice = 0;
    curState = 0;
}

// Destructor.
// The form will be deleted automatically, because it is owned by the scroll widget,
// while scroll widget is owned by screen.
FBScreen::~FBScreen ()
{
}

void FBScreen::setForm (FBForm* form)
{
    if (form == NULL)
        return;
    if (formPtr != NULL)
        this->deleteForm();
    scrollScreen->setWidget(form);
    formPtr = form;
}

void FBScreen::deleteForm ()
{
    if (formPtr == NULL)
        return;
    scrollScreen->takeWidget();
    delete formPtr;
    formPtr = NULL;
}

FBForm *FBScreen::takeForm ()
{
    if (formPtr == NULL)
        return NULL;
    FBForm *wForm = (FBForm*)(scrollScreen->takeWidget()); // can cast, because it was
    formPtr = NULL;                                        // set via specific method
    return wForm;
}

FBForm *FBScreen::getFormPtr ()
{
    return formPtr;
}

void FBScreen::updateStyle ()
{
    // Clear old style.
    // Reset general stylesheet of the screen.
    wScreen->setStyleSheet("");

    // Grey background.
    wScreen->setStyleSheet("QWidget {background-color: "
                           +QString(FB_COLOR_LIGHTGREY)+";"
                           "border-top-left-radius: 4px;"
                           "border-top-right-radius: 4px;"
                           "border-bottom-left-radius: 4px;"
                           "border-bottom-right-radius: 4px;}");

    // Signalize to form and its elems to change style accordingly.
    if (formPtr != NULL)
        formPtr->updateStyle(FB_STYLENAME_DEFAULT);
}

void FBScreen::setDevice (int index)
{
    curDevice = index;
}

void FBScreen::setState (int index)
{
    // Maximize working area.
    for (int i=0; i<wsWorkingArea.size(); i++)
    {
        //wsWorkingArea[i]->setMinimumSize(0,0);
        wsWorkingArea[i]->setSizePolicy(QSizePolicy::Minimum,
                                        QSizePolicy::Minimum);
    }

    curState = index;
}

