/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  different screens
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

#ifndef SCREENS_H
#define SCREENS_H

#include <QPixmap>

#include "screen_core.h"

// Abstract decorators for common screen:
class FBDecoratorDefault: public FBDecorator
{
    public:
    FBDecoratorDefault (): FBDecorator() { }
    ~FBDecoratorDefault () { }
    virtual void clearWithDefaults (FBElem *elem);
    void updateTextFromStringAttr (FBElem *elem, QString attrKeyName);
    void updateCombobox (FBElem *elem);
    void updateDoubCombobox (FBElem *elem);
};
// Decorators for common screen:
class FBDecoratorText: public FBDecoratorDefault
{
    public:
    FBDecoratorText (): FBDecoratorDefault() { }
    ~FBDecoratorText () { }
    virtual void redecor (FBElem *elem);
    virtual void update (FBElem *elem);
};
class FBDecoratorSpace: public FBDecoratorDefault
{
    public:
    FBDecoratorSpace (): FBDecoratorDefault() { }
    ~FBDecoratorSpace () { }
    virtual void redecor (FBElem *elem);
    virtual void update (FBElem *elem) { return; }
};
class FBDecoratorTextedit: public FBDecoratorDefault
{
    public:
    FBDecoratorTextedit (): FBDecoratorDefault() { }
    ~FBDecoratorTextedit () { }
    virtual void redecor (FBElem *elem);
    virtual void update (FBElem *elem);
};
class FBDecoratorCombobox: public FBDecoratorDefault
{
    public:
    FBDecoratorCombobox (): FBDecoratorDefault() { }
    ~FBDecoratorCombobox () { }
    virtual void redecor (FBElem *elem);
    virtual void update (FBElem *elem);
};
class FBDecoratorDoubledcombobox: public FBDecoratorDefault
{
    public:
    FBDecoratorDoubledcombobox (): FBDecoratorDefault() { }
    ~FBDecoratorDoubledcombobox () { }
    virtual void redecor (FBElem *elem);
    virtual void update (FBElem *elem);
};
class FBDecoratorCheckbox: public FBDecoratorDefault
{
    public:
    FBDecoratorCheckbox (): FBDecoratorDefault() { }
    ~FBDecoratorCheckbox () { }
    virtual void redecor (FBElem *elem);
    virtual void update (FBElem *elem);
};
class FBDecoratorRadiogroup: public FBDecoratorDefault
{
    public:
    FBDecoratorRadiogroup (): FBDecoratorDefault() { }
    ~FBDecoratorRadiogroup () { }
    virtual void redecor (FBElem *elem);
    virtual void update (FBElem *elem);
};
class FBDecoratorButton: public FBDecoratorDefault
{
    public:
    FBDecoratorButton (): FBDecoratorDefault() { }
    ~FBDecoratorButton () { }
    virtual void redecor (FBElem *elem);
    virtual void update (FBElem *elem);
};


/**
 * Abstract screen for mobiles with portrait and landscape orientations.
 */
class FBScreenMobile: public FBScreen
{
    Q_OBJECT
    public:
     FBScreenMobile (QWidget *parent, float sizeFactor);
     virtual ~FBScreenMobile ();
     void changeDevice (int index); // common for all mobile, so not virtual
     void changeState (int index);
    protected:
     virtual void clearDecor ();
    protected:
     QList<QLabel*> labsScreenDecorVert;
     QList<QLabel*> labsScreenDecorHor;
//     FBForm *form2Ptr; // other set of elements
};


/**
 * Screen with Android style.
 */
class FBScreenAndroid: public FBScreenMobile
{
    Q_OBJECT
    public:
     FBScreenAndroid (QWidget* parent, float sizeFactor);
     ~FBScreenAndroid ();
    protected:
     virtual void installDecor ();
    private:
     virtual void registerDecorators ();
};
// Abstract decorators for Android screen:
class FBDecoratorAndroid: public FBDecoratorDefault
{
    public:
    FBDecoratorAndroid (): FBDecoratorDefault() { }
    ~FBDecoratorAndroid () { }
    virtual void clearWithDefaults (FBElem *elem);
    //    virtual void updateCaption (FBElem *elem);
};
//Decorators for Android screen:
class FBDecAndrText: public FBDecoratorAndroid
{
    public:
    FBDecAndrText (): FBDecoratorAndroid() { }
    ~FBDecAndrText () { }
    virtual void redecor (FBElem *elem);
    virtual void update (FBElem *elem);
};
class FBDecAndrSpace: public FBDecoratorAndroid
{
    public:
    FBDecAndrSpace (): FBDecoratorAndroid() { }
    ~FBDecAndrSpace () { }
    virtual void redecor (FBElem *elem);
};
class FBDecAndrTextedit: public FBDecoratorAndroid
{
    public:
    FBDecAndrTextedit (): FBDecoratorAndroid() { }
    ~FBDecAndrTextedit () { }
    virtual void redecor (FBElem *elem);
    virtual void update (FBElem *elem);
};
class FBDecAndrCombobox: public FBDecoratorAndroid
{
    public:
    FBDecAndrCombobox (): FBDecoratorAndroid()
    {
        pixCombo = QPixmap(":screen/img/android/for_combo.png");
    }
    ~FBDecAndrCombobox () { }
    virtual void redecor (FBElem *elem);
    virtual void update (FBElem *elem);
    protected:
    QPixmap pixCombo;
};
class FBDecAndrDoubCombobox: public FBDecAndrCombobox
{
    public:
    FBDecAndrDoubCombobox (): FBDecAndrCombobox() { }
    ~FBDecAndrDoubCombobox () { }
    virtual void redecor (FBElem *elem);
    virtual void update (FBElem *elem);
};
class FBDecAndrCheckbox: public FBDecoratorAndroid
{
    public:
    FBDecAndrCheckbox (): FBDecoratorAndroid()
    {
        pixCheckOn = QPixmap(":screen/img/android/for_check_on.png");
        pixCheckOff = QPixmap(":screen/img/android/for_check_off.png");
    }
    ~FBDecAndrCheckbox () { }
    virtual void redecor (FBElem *elem);
    virtual void update (FBElem *elem);
    protected:
    QPixmap pixCheckOn;
    QPixmap pixCheckOff;
};
class FBDecAndrRadiogroup: public FBDecoratorAndroid
{
    public:
    FBDecAndrRadiogroup (): FBDecoratorAndroid()
    {
        pixRadioOn = QPixmap(":screen/img/android/for_radio_on.png");
        pixRadioOff = QPixmap(":screen/img/android/for_radio_off.png");
    }
    ~FBDecAndrRadiogroup () { }
    virtual void redecor (FBElem *elem);
    virtual void update (FBElem *elem);
    protected:
    QPixmap pixRadioOn;
    QPixmap pixRadioOff;
};
class FBDecAndrDatetime: public FBDecoratorAndroid
{
    public:
    FBDecAndrDatetime (): FBDecoratorAndroid()
    {
        pixDate = QPixmap(":screen/img/android/for_date.png");
    }
    ~FBDecAndrDatetime () { }
    virtual void redecor (FBElem *elem);
    virtual void update (FBElem *elem);
    protected:
    QPixmap pixDate;
};
class FBDecAndrButton: public FBDecoratorAndroid
{
    public:
    FBDecAndrButton (): FBDecoratorAndroid() { }
    ~FBDecAndrButton () { }
    virtual void redecor (FBElem *elem);
    virtual void update (FBElem *elem);
};
class FBDecAndrPhoto: public FBDecoratorAndroid
{
    public:
    FBDecAndrPhoto (): FBDecoratorAndroid()
    {
        pixPhoto = QPixmap(":screen/img/android/for_photo.png");
    }
    ~FBDecAndrPhoto () { }
    virtual void redecor (FBElem *elem);
    virtual void update (FBElem *elem);
    protected:
    QPixmap pixPhoto;
};
class FBDecAndrSignature: public FBDecoratorAndroid
{
    public:
    FBDecAndrSignature (): FBDecoratorAndroid()
    {
        pixSign = QPixmap(":screen/img/android/for_signature.png");
    }
    ~FBDecAndrSignature () { }
    virtual void redecor (FBElem *elem);
    protected:
    QPixmap pixSign;
};
class FBDecAndrCounter: public FBDecoratorAndroid
{
    public:
    FBDecAndrCounter (): FBDecoratorAndroid()
    {
        //pix = QPixmap(":screen/img/android/for_counter.png");
    }
    ~FBDecAndrCounter () { }
    virtual void redecor (FBElem *elem);
    virtual void update (FBElem *elem);
    protected:
    QPixmap pix;
};
class FBDecAndrCoordinates: public FBDecoratorAndroid
{
    public:
    FBDecAndrCoordinates (): FBDecoratorAndroid()
    {
        pix = QPixmap(":screen/img/android/for_coordinates.png");
    }
    ~FBDecAndrCoordinates () { }
    virtual void redecor (FBElem *elem);
    virtual void update (FBElem *elem);
    protected:
    QPixmap pix;
};


/**
 * Screen with iPhone/iPad style.
 */
class FBScreenIos: public FBScreenMobile
{
    Q_OBJECT
    public:
     FBScreenIos (QWidget* parent, float sizeFactor);
     ~FBScreenIos ();
};


/**
 * ...
 */
class FBScreenWeb: public FBScreen
{
    Q_OBJECT
    public:
     FBScreenWeb (QWidget* parent, float sizeFactor);
     ~FBScreenWeb ();
    private:
     virtual void registerDecorators ();
};


/**
 * ...
 */
class FBScreenQgis: public FBScreen
{
    Q_OBJECT
    public:
     FBScreenQgis (QWidget* parent, float sizeFactor);
     ~FBScreenQgis ();
    private:
     virtual void registerDecorators ();
};


#endif //SCREENS_H
