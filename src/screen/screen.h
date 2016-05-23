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
#include <QScrollBar>

#include "form/form_core.h"

#define FB_COEFF_SCREENSIZE 130.0 // real screen size coefficient

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

// Has two states: portrait and landscape.
class FBDeviceMobile: public FBDevice
{
    public:
     FBDeviceMobile (QPair<int,int> resolution, float diagonal, float dpi,
                     QString name, QString imgPath);
     virtual ~FBDeviceMobile() {}
};
class FBDeviceTablet: public FBDevice
{
    public:
     FBDeviceTablet (QPair<int,int> resolution, float diagonal, float dpi,
                     QString name, QString imgPath);
     virtual ~FBDeviceTablet() {}
};


/**
 * Common screen class.
 *
 * Screen contains the form and renders it with some style (e.g. Android, iOS, ...).
 * Each screen can have a list of specific devices and at the same time all screens
 * have the default device: "maximized" desktop device.
 *
 * By default screen is a grey void area.
 */
class FBScreen: public QWidget
{
    Q_OBJECT

    public:

     FBScreen (QWidget *parent);
     virtual ~FBScreen ();
     void setForm (FBForm* form);
     void deleteForm ();
     FBForm *takeForm ();
     FBForm *getFormPtr ();

     virtual void updateStyle ();
     virtual void changeDevice (int index);
     virtual void changeState (int index);

     QList<FBDevice> getDevices() { return devices; }
     FBDevice getCurDevice () { return devices[curDevice]; }

     void requestScrollToBottom () { canScrollToBottom = true; }

    protected:

     QVBoxLayout *lvMain; // the layout of the screen
     QScrollArea *scrollMain; // actual parent of form widget

     FBForm *formPtr; // actually form widget is owned by scroll area

     QList<FBDevice> devices;
     int curDevice;
     int curState;

    private slots:
     void scrollToBottom (int min, int max);
    private:
     bool canScrollToBottom;
};

// Abstract screen for mobiles with portrait and landscape orientations.
class FBScreenMobile: public FBScreen
{
    public:
     FBScreenMobile (QWidget *parent);
     virtual ~FBScreenMobile ();
     virtual void updateStyle () = 0;
     void changeDevice (int index); // common for all mobile, so not virtual
     void changeState (int index);

     static QPair<int,int> calculateScreenSize (float diagonal,
                                                float widthResol, float heightResol);

    protected:
     QList<QLabel*> labsScreenDecor;

     FBForm *form2Ptr; // other set of elements
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





#endif //SCREEN_H
