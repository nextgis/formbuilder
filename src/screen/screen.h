/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  screen definitions
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

#ifndef SCREEN_H
#define SCREEN_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>

#include "form/form_core.h"

struct FBState
{
    QString name;
    QString descr;
    QString imgPath;

    FBState (QString name, QString descr, QString imgPath)
        { this->name = name; this->descr = descr; this->imgPath = imgPath; }
};

struct FBDevice
{
    //QPair<int,int> aspectRatio;
    QPair<int,int> resolution;
    float diagonal; // inches
    float dpi;
    QString name;
    QString imgPath;

    FBDevice (QPair<int,int> resolution, float diagonal, float dpi, QString name,
              QString imgPath)
        { this->resolution = resolution; this->diagonal = diagonal; this->dpi = dpi;
        this->name = name; this->imgPath = imgPath; }

    QString getDisplayString ()
        { return name; }
};


/**
 * Screen contains the form and render it.
 * WARNING. Each concrete screen must have at least one device and according
 * state (orientation).
 */

// Default void grey screen.
class FBScreen: public QWidget
{
    public:

     FBScreen (QWidget *parent);
     virtual ~FBScreen ();
     void setForm (FBForm* form);
     void deleteForm ();
     FBForm *takeForm ();
     FBForm *getFormPtr ();
     virtual void updateStyle ();
     QList<FBDevice> getDevices() { return devices; }
     QList<FBState> getStates () { return states; }
     virtual void setState (int index);
     virtual void setDevice (int index);

    protected:

     QGridLayout *lgWorkingArea;
     QList<QWidget*> wsWorkingArea;
     QVBoxLayout *lvScreen; // the layout of screen widget
     QWidget *wScreen; // parent widget for decor labels (if some) and scroll area
     QScrollArea *scrollScreen; // actual parent of form widget

     FBForm *formPtr; // actually form widget is owned by scroll area

     QList<FBState> states;
     int curState;
     QList<FBDevice> devices;
     int curDevice;
};

// Abstract screen for mobiles with portrait and landscape orientations.
class FBScreenMobile: public FBScreen
{
    public:
     FBScreenMobile (QWidget *parent);
     virtual ~FBScreenMobile ();
     virtual void updateStyle () = 0;
     void setDevice (int index); // common for all mobile, so not virtual
     void setState (int index); // common for all mobile, so not virtual

    protected:
     QList<QLabel*> labsScreenDecor;
     QList<FBElem*> otherElemsSet; // the order of elems is important
};

// Screen with Android style.
class FBScreenAndroid: public FBScreenMobile
{
    public:
     FBScreenAndroid (QWidget* parent);
     ~FBScreenAndroid ();
     void updateStyle ();
};

// Screen with iPhone/iPad style.
class FBScreenIos: public FBScreenMobile
{
    public:
     FBScreenIos (QWidget* parent);
     ~FBScreenIos ();
     void updateStyle ();
};

/*
class FBScreenWeb: public FBScreen
{
};

class FBScreenQgis: public FBScreen
{
};
*/


#endif //SCREEN_H
