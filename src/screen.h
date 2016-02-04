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


#define FB_SCREEN_ANDROID "Android"
#define FB_SCREEN_QGIS "QGIS"
#define FB_SCREEN_WEB "Web"

#define FB_SCREEN_MAXIMIZED "Maximized"
#define FB_SCREEN_PHONEPORT "Phone portrait"
#define FB_SCREEN_PHONELAND "Phone landscape"
#define FB_SCREEN_TABLETPORT "Tablet portrait"
#define FB_SCREEN_TABLETLAND "Tablet landscape"


/**
 * Final screen class which contains the form and render it.
 *
 * Screen can change its appearance: decoration, sizes, "resolution", etc.
 */
class FBWorkingArea: public QWidget
{
    public: // static fields

     static void initAll ();
     static QList<QPair<QString,QString> > STYLES;
     static QList<QPair<QString,QString> > TYPES;
     static QList<QString> RATIOS;
     static QList<QString> RESOLS;

    public: // methods

     FBWorkingArea (QWidget *parent);
     ~FBWorkingArea () {}

     //void setForm (FBForm* form);
     //void removeForm ();

     void changeStyle (QString styleStr);
     void changeType (QString typeStr);
     void changeRatio (int indexOfRatio); // all indexes refers to global
     void changeResol (int indexOfResol); // static arrays
    
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
     QString style;
     QString type;
     QString ratio;
     QString resolution;
};


#endif //SCREEN_H
