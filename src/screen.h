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
#include "form.h"


#define FB_SCREEN_ANDROID "Android"
#define FB_SCREEN_QGIS "QGIS"
#define FB_SCREEN_WEB "Web"

#define FB_SCREEN_MAXIMIZED "Maximized"
#define FB_SCREEN_PHONEPORT "Phone portrait"
#define FB_SCREEN_PHONELAND "Phone landscape"
#define FB_SCREEN_TABLETPORT "Tablet portrait"
#define FB_SCREEN_TABLETLAND "Tablet landscape"


/*
enum FBScreenType
{
    FBWindow, FBPhone, FBTablet
};
QMap<FBScreenType,QPair<QString,QString> > screenTypes;

QMap<int,QPair<short,short> > screenRatios;

QMap<int,QPair<int,int> > screenResols;


class FBFactoryScreen
{
    public:
     static QList<FBFactoryScreen*> fctsScreen;
     static void registerScreen (FBFactoryScreen* fct) {fctsScreen.append(fct);}
     FBFactoryScreen (QString name, QString imgPath);
    protected:
     QList<FBScreenType> types;
     QList<QString> ratios;
     QList<QString> resols;
};

class FBScreen: public QWidget
{
    public:

     FBScreen (FBWorkingArea *parent);
     ~FBScreen () {}
     virtual void applyToScreen (QWidget *wScreen, QScrollArea *scrollScreen);
    protected:

};

class FBScreenAndroid: public FBScreen
{
    public:
     virtual void applyToScreen (QWidget *wScreen, QScrollArea *scrollScreen);
    protected:
     QVBoxLayout *vlScreen;
     QList<QLabel*> labsScreenDecor;
};
*/

/**
 * Final screen class which contains the form and render it.
 *
 * Screen can change its appearance: decoration, sizes, "resolution", etc. We do
 * not make derived classes for each screen, because that means that we must
 * recreate such classes' objects and reinsert them into main window each time we
 * want to change appearance, type, etc., while the form will be still untouched.
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

     void setForm (FBForm* form);
     void removeForm ();

     void changeStyle (QString styleStr);
     void changeType (QString typeStr);
     void changeRatio (int indexOfRatio); // all indexes refers to global
     void changeResol (int indexOfResol); // static arrays
    
    private:

     // visual
     QGridLayout *glWorkingArea;
     QList<QWidget*> wsWorkingArea;
     QWidget *wScreen; // parent widget for decor labels and scroll area
     QVBoxLayout *vlScreen; // the layout of screen widget
     QList<QLabel*> labsScreenDecor;

     // form
     QScrollArea *scrollScreen; // actual parent of form
     FBForm *form;

     // params
     QString style;
     QString type;
     QString ratio;
     QString resolution;
};


#endif //SCREEN_H
