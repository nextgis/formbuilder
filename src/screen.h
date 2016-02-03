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

#include "fb_general.h"
//#include "form.h"


enum FBScreenStyle
{
    FBNoStyle, FBAndroid, //FBWeb, FBQGIS
};

enum FBScreenType
{
    FBMaximized, FBPhonePortrait, //FBPhoneLandscape, FBTabletPortrait,
    // FBTabletLandscape
};

enum FBScreenRatio
{
    //FB15to9, FB16to9, ...
};

enum FBScreenResolution
{
    //FB480x800, FB800x600, ...
};


/**
 * Final screen class which contains the form and render it.
 *
 * Screen can change its appearance: decoration, sizes, "resolution", etc.
 */
class FBWorkingArea: public QWidget
{
    public:

     FBWorkingArea (QWidget *parent);
     ~FBWorkingArea () {}

     //void setForm (FBForm* form);
     //void removeForm ();

     void changeStyle (FBScreenStyle newStyle);
     void changeType (FBScreenType newType);
    
    private:

     // visual
     QGridLayout *glWorkingArea;
     QList<QWidget*> wsWorkingArea;
     QWidget *wScreen;
     QVBoxLayout *vlScreen;
     QList<QLabel*> labsScreenDecor;
     QScrollArea *scrollScreen;

     // form
     //FBForm *form;

     // params
     FBScreenStyle style;
     FBScreenType type;
     FBScreenRatio ratio;
     FBScreenResolution resolution;
};


#endif //SCREEN_H
