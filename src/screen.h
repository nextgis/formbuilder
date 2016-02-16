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

#include "fb_common.h"
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
 * Screen class which contains the form and render it.
 *
 * Screen can change its appearance: decoration, sizes, "resolution", etc.
 */
class FBScreen: public QWidget
{
    public:

     FBScreen (QWidget *parent);
     virtual ~FBScreen ();
     void setForm (FBForm* form);
     void removeForm ();
     FBForm *takeForm ();
     FBForm *getFormPtr ();
     virtual void updateStyle ();
     QList<FBState> getStates () { return states; }
     QList<FBDevice> getDevices() { return devices; }
     virtual void setState (int index);
     virtual void setDevice (int index);

    protected:

     QGridLayout *glWorkingArea;
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

class FBScreenMobile: public FBScreen
{
    public:

     FBScreenMobile (QWidget *parent);
     virtual ~FBScreenMobile ();
     virtual void updateStyle () = 0;
     void setState (int index); // common for all mobile, so not virtual
     void setDevice (int index); // common for all mobile, so not virtual

    protected:

     QList<QLabel*> labsScreenDecor;

     QList<FBElem*> otherElemsSet; // the order of elems is important
};

class FBScreenAndroid: public FBScreenMobile
{
    public:
     FBScreenAndroid (QWidget* parent);
     ~FBScreenAndroid ();
     void updateStyle ();
};

/*

class FBScreenApple: public FBScreenMobile
{
};

class FBScreenWeb: public FBScreen
{
};

class FBScreenQgis: public FBScreen
{
};

*/


#endif //SCREEN_H
