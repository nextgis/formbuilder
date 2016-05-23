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


/****************************************************************************/
/*                                                                          */
/*                              Device                                      */
/*                                                                          */
/****************************************************************************/

FBDevice::FBDevice (QPair<int,int> resolution, float diagonal, float dpi, QString name,
                    QString imgPath)
{
    this->resolution = resolution;
    this->diagonal = diagonal;
    this->dpi = dpi;
    this->name = name;
    this->imgPath = imgPath;
}

FBDeviceDefault::FBDeviceDefault ():
    FBDevice(QPair<int,int>(-1,-1),-1.0,-1.0,QObject::tr("Default"),"")
{
    states.append(FBState(QObject::tr("Maximized"),
                          QObject::tr("Maximize"),":/img/maximized.png"));
}

FBDeviceMobile::FBDeviceMobile (QPair<int,int> resolution, float diagonal, float dpi,
                                QString name, QString imgPath):
    FBDevice(resolution,diagonal,dpi,name,imgPath)
{
    states.append(FBState(QObject::tr("Portrait"),
                          QObject::tr("Portrait orientation"),":/img/phone_port.png"));
    states.append(FBState(QObject::tr("Landscape"),
                          QObject::tr("Landscape orientation"),":/img/phone_land.png"));
}

FBDeviceTablet::FBDeviceTablet (QPair<int,int> resolution, float diagonal, float dpi,
                                QString name, QString imgPath):
    FBDevice(resolution,diagonal,dpi,name,imgPath)
{
    states.append(FBState(QObject::tr("Portrait"),
                          QObject::tr("Portrait orientation"),":/img/tablet_port.png"));
    states.append(FBState(QObject::tr("Landscape"),
                          QObject::tr("Landscape orientation"),":/img/tablet_land.png"));
}


/****************************************************************************/
/*                                                                          */
/*                               Screen                                     */
/*                                                                          */
/****************************************************************************/

FBScreen::FBScreen (QWidget *parent):
    QWidget(parent)
{
    canScrollToBottom = false;

    lvMain = new QVBoxLayout(this);
    lvMain->setContentsMargins(0,0,0,0);
    lvMain->setSpacing(0);

    scrollMain = new QScrollArea(this);
    scrollMain->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Expanding);
    scrollMain->setStyleSheet("QScrollArea{border: 0px;}");
    scrollMain->setWidgetResizable(true);
    QObject::connect(scrollMain->verticalScrollBar(), SIGNAL(rangeChanged(int,int)),
            this, SLOT(scrollToBottom(int, int)));

    lvMain->addWidget(scrollMain);

    formPtr = NULL;

    devices.append(FBDeviceDefault());
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
    scrollMain->setWidget(form);
    formPtr = form;
}

void FBScreen::deleteForm ()
{
    if (formPtr == NULL)
        return;
    scrollMain->takeWidget();
    delete formPtr;
    formPtr = NULL;
}

FBForm *FBScreen::takeForm ()
{
    if (formPtr == NULL)
        return NULL;
    FBForm *wForm = (FBForm*)(scrollMain->takeWidget()); // can cast, because it was
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
    // Grey background.
    this->setStyleSheet("QWidget {background-color: "
                           +QString(FB_COLOR_LIGHTGREY)+";"
                           "border-top-left-radius: 4px;"
                           "border-top-right-radius: 4px;"
                           "border-bottom-left-radius: 4px;"
                           "border-bottom-right-radius: 4px;}");

    // Signalize to form and its elems to change style accordingly.
    if (formPtr != NULL)
        formPtr->updateStyle(FB_STYLENAME_DEFAULT);
}


void FBScreen::changeDevice (int index)
{
    curDevice = 0;
    // Default screen appearance: "maximized screen".
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    this->setMinimumSize(0,0);
    this->setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
}


void FBScreen::changeState (int index)
{
    // Nothing to do with screen appearance.
    curState = 0;
}


// Scroll scrollArea to the end.
// Can not do it with setValue() method.
// http://www.qtcentre.org/threads/32852-How-can-I-always-keep-the-scroll-bar-at-
// the-bottom-of-a-QScrollArea
void FBScreen::scrollToBottom (int min, int max)
{
    if (canScrollToBottom)
    {
        scrollMain->verticalScrollBar()->setValue(max);
        canScrollToBottom = false;
    }
}


