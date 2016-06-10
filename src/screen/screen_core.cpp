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

#include "screen_core.h"


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


/****************************************************************************/
/*                                                                          */
/*                               Screen                                     */
/*                                                                          */
/****************************************************************************/

FBScreen::FBScreen (QWidget *parent, float sizeFactor):
    QWidget(parent)
{
    this->sizeFactor = sizeFactor;

    canScrollToBottom = false;

    lhMain1 = new QHBoxLayout(this);
    lhMain1->setContentsMargins(0,0,0,0);
    lhMain1->setSpacing(0);

    lvMain2 = new QVBoxLayout();
    lvMain2->setContentsMargins(0,0,0,0);
    lvMain2->setSpacing(0);

    scrollMain = new QScrollArea(this);
    scrollMain->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Expanding);
    scrollMain->setStyleSheet("QScrollArea{border: 0px;}");
    scrollMain->setWidgetResizable(true);
    QObject::connect(scrollMain->verticalScrollBar(), SIGNAL(rangeChanged(int,int)),
            this, SLOT(scrollToBottom(int, int)));

    lvMain2->addWidget(scrollMain);
    lhMain1->addLayout(lvMain2);

    formPtr = NULL;

    devices.append(FBDeviceDefault());
    curDevice = 0;
    curState = 0;
}

FBScreen::~FBScreen ()
{
    // Note: the form will be deleted automatically, because it is owned by the
    // scroll widget, while scroll widget is owned by screen.
}


void FBScreen::setForm (FBForm* form)
{
    if (form == NULL)
        return;
    if (formPtr != NULL)
        this->deleteForm();
    scrollMain->setWidget(form);
    formPtr = form;
    // Update look of all elems.
    // ...
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
    formPtr = NULL;                                      // set via specific method
    return wForm;
}

FBForm *FBScreen::getFormPtr ()
{
    return formPtr;
}


void FBScreen::registerDecorator (QString elemKeyName, FBDecorator *decorator)
{
    if (decorators.find(elemKeyName) != decorators.end())
        return;
    decorators.insert(elemKeyName,decorator);
}

void FBScreen::registerAllDecorators ()
{
    this->deregisterAllDecorators(); // clear list
    this->registerDecorators();
}

void FBScreen::deregisterAllDecorators ()
{
    QMap<QString,FBDecorator*>::const_iterator it = decorators.constBegin();
    while (it != decorators.constEnd())
    {
        delete it.value();
        ++it;
    }
    decorators.clear();
}

FBDecorator *FBScreen::findDecorator (QString elemKeyName)
{
    QMap<QString,FBDecorator*>::iterator it = decorators.find(elemKeyName);
    if (it == decorators.end())
        return NULL;
    return it.value();
}


// Redecorate the whole screen.
// General public method to update screen appearance: it depends on the screen's
// style, device, state, form, etc.
void FBScreen::redecorate ()
{
    this->clearDecor();
    this->installDecor();
    this->redecorateForm();
}

// Redecorate only screen's form.
void FBScreen::redecorateForm ()
{
    if (formPtr == NULL)
        return;
    // Form itself cannot have any style. We update look of all elements in the form.
    QList<FBElem*> elems = formPtr->getAllElems();
    for (int i=0; i<elems.size(); i++)
    {
        // Find proper decorator among registered for this screen, otherwise
        // use default decorator so not to render element with decorator from
        // the old screen (if there was any).
        FBDecorator *decoratorPtr = this->findDecorator(elems[i]->getKeyName());
        if (decoratorPtr == NULL)
            decoratorPtr = formPtr->getDefaultDecoratorPtr();
        elems[i]->setDecorator(decoratorPtr);
        elems[i]->redecorateSelf();
        elems[i]->updateSelf(); // do not forget to update elem according to its attributes
    }
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


// Calculates the actual screen width and height, using the passed diagonal
// and screen resolutions size. Diagonal is used to determine the screen size
// while the resolution - to determine screen's aspect ratio. For example: two
// devices with DPI = 1 and DPI = 2 but with the same diagonal will have the
// same screen size.
// In the returned pair the first value is always larger than the second.
// The pair [0,0] is returned if incorrect args are passed.
QPair<int,int> FBScreen::calculateScreenSize (float d, float w, float h)
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
    return QPair<int,int>(x*sizeFactor,y*sizeFactor);
}


// Delete all decor widgets (not touching scrollarea and form).
void FBScreen::clearDecor ()
{
    // Note: no previous decor deleting here. The screen is just a grey area.
}


// Create all decor widgets (not touching scrollarea and form).
// Used only after undecor() or at the begining.
void FBScreen::installDecor ()
{
    // Reset general stylesheet of the screen: grey background.
    this->setStyleSheet("QWidget {background-color: rgb(238,238,238);"
                           "border-top-left-radius: 4px;"
                           "border-top-right-radius: 4px;"
                           "border-bottom-left-radius: 4px;"
                           "border-bottom-right-radius: 4px;}");
}


