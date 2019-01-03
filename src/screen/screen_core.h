/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  screen core definitions
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

#ifndef SCREEN_CORE_H
#define SCREEN_CORE_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>

#include "form/form_core.h"

#define FB_NAMEDDECOR_TEXT "txt"
#define FB_NAMEDDECOR_TEXT2 "txt2"
#define FB_NAMEDDECOR_IMG "img"
//#define FB_ELEMSTYLE_CAPTION "cpt"

struct FBState
{
    QString name;
    QString descr;
    QString imgPath;
    FBState (QString name, QString descr, QString imgPath)
    {
        this->name = name;
        this->descr = descr;
        this->imgPath = imgPath;
    }
};


/**
 * Abstract device class.
 *
 * Device defines the screen parameters of one type (e.g. Android) in order
 * to correctly display the form for different real target devices.
 * Device class also contains several states which can change such parameters
 * and simulate the changes of the real device, such as rotation.
 */
class FBDevice
{
   public:
    FBDevice (QPair<int,int> resolution, float diagonal, float dpi, QString name,
              QString imgPath);
    virtual ~FBDevice () {}

    QPair<int,int> getResolution () { return resolution; }
    float getDiagonal () { return diagonal; }
    float getDpi () { return dpi; }
    QString getName () { return name; }
    QString getImgPath () { return imgPath; }
    QList<FBState> getStates () { return states; }

   protected:
    //QPair<int,int> aspectRatio;
    QPair<int,int> resolution;
    float diagonal; // inches
    float dpi;
    QString name;
    QString imgPath;
    QList<FBState> states;
};

// Common desktop device. Has one "maximized" (fullscreen) state.
class FBDeviceDefault: public FBDevice
{
   public:
    FBDeviceDefault ();
    virtual ~FBDeviceDefault() {}
};


/**
 * Common screen class.
 *
 * Screen contains the form and renders it with some style (e.g. Android, iOS, ...).
 * Each screen can have a list of specific devices and at the same time all screens
 * have the default device: "maximized" desktop device.
 *
 * Screen also knows how to decor elements on the form: it stores a list of according
 * decorators.
 *
 * By default screen is a grey area with black elements.
 */
class FBScreen: public QWidget
{
    Q_OBJECT

    public:

     FBScreen (QWidget *parent, float sizeFactor);// = 1.0);
     virtual ~FBScreen ();

     void setForm (FBForm* form);
     void deleteForm ();
     FBForm *takeForm ();
     FBForm *getFormPtr ();

     void registerDecorator (QString elemKeyName, FBDecorator* decorator);
     void registerAllDecorators ();
     void deregisterAllDecorators ();
     FBDecorator *findDecorator (QString elemKeyName);
     void redecorate ();
     void redecorateForm ();

     virtual void changeDevice (int index);
     virtual void changeState (int index);
     QList<FBDevice> getDevices () { return devices; }
     FBDevice getCurDevice () { return devices[curDevice]; }

     void requestScrollToBottom () { canScrollToBottom = true; }

    protected:

     QPair<int,int> calculateScreenSize (float diagonal,
                               float widthResol, float heightResol);
     virtual void clearDecor ();
     virtual void installDecor ();

    protected:

     float sizeFactor;

     QHBoxLayout *lhMain1;
     QVBoxLayout *lvMain2; // the layout of the screen
     QScrollArea *scrollMain; // actual parent of form widget

     FBForm *formPtr; // actually form widget is owned by scroll area

     QList<FBDevice> devices;
     int curDevice;
     int curState;

     QMap<QString,FBDecorator*> decorators; // first = elemKeyName

    private:

     virtual void registerDecorators ();

    private slots:

     void scrollToBottom (int min, int max);

    private:

     bool canScrollToBottom;
};


#endif //SCREEN_CORE_H
