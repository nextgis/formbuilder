/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  form definitions
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

#ifndef FORM_H
#define FORM_H

#include <QWidget>

#include "json/json.h"

#include "fb_general.h"

class FBElem;
class FBForm;


/**
 * Attributes.
 * One attribute equals to one value in the project (in form.json of ngfp file).
 * That's why complex attribute (e.g. lists of values for Double-list elem) is
 * usually edited in complex way: with the help of special dialogs.
 */
class FBAttr: public QObject
{
    Q_OBJECT
    public:
     FBAttr ();
     virtual ~FBAttr ();
     virtual Json::Value toJson () = 0;
     virtual void fromJson (Json::Value jsonVal) = 0;
     virtual QWidget *getWidget () = 0;
    
    protected:
     FBElem *elemPtr;
    
    signals: 
     void valueChanged (); // in order to signalize to other attrs of this element
                           // that they must be changed
};

/*
class FBAttrWidget: public FBAttr
{
    Q_OBJECT
    protected slots:
     void onEditEnd ();
};

class FBAttrDialog: public FBAttr
{
    Q_OBJECT
    public:
     virtual QPushButton *getWidget () = 0;
    protected slots:
     void onEditStart ();
};
*/


/**
 * 
 * Elements.
 *
 */
// Default elem - has its own graphical representation so it will be possible
// for form to manage unknown-unsupported elements in common way.
class FBElem: public QWidget
{
    public:
     FBElem ();
     virtual ~FBElem ();
     virtual Json::Value toJson ();
     virtual FBErr fromJson ();
//     void changeStyle (FBScreenStyle style);
    
    protected:
     FBForm* formPtr;
};

/*
// Abstract simple element for displaying.
class FBElemDecor: protected FBElem
{
    public:
     virtual Json::Value toJson ();
     virtual FBErr fromJson ();
};

// Element which can write values to the layer fields.
class FBElemInput: protected FBElem
{
    public:
     virtual Json::Value toJson ();
     virtual FBErr fromJson ();

    protected:
     QString strFieldKeyName; // common attr for all elems with input
     QString strCaption; // common attr for all elems with input
};

// This compound element may contain other elements which are even also compound.
class FBElemCompound: protected FBElem
{
    public:
     void addElem (FBElem* afterElem); // afterElem can be NULL
     void removeElem (FBElem* elem);
     void removeAllElems ();

     virtual Json::Value toJson (); // also calls toJson() of inner elems
     virtual FBErr fromJson (); // also calls fromJson() of inner elems
    
    protected:
     QList<FBElem*> elemPtrs;
       
};
*/


/**
 * 
 * Helper class for elements movement.
 *
 */
class FBInsertWidget: public QWidget 
{
    
};


/**
 * Final form class. Form contains elements and provides the capability to
 * manage them using app events (via mouse).
 *
 * Visually form is "transparent" and only its elements are rendered to the
 * screen. Technically form is a QWidget which contains elements in its layout
 * like any simple Qt-widget.
 *
 * All elements are always followed each other in the form vertically. This
 * is because the structure of the final JSON file where form is saved. All
 * other layouts and groupings is made via specific elements.
 *
 * The special insert-element is placed between any pair of elements  so it can
 * be possible to move them via mouse.
 *
 * The form, as well as its elements and attributes, has specific methods of
 * converting self to/from JSON.
 *
 */
class FBForm: public QWidget
{
    public: // static fields
     static FBElem *SELECTED;
     static bool IS_SELECTED_MOVING;

    public:
     FBForm ();
     ~FBForm ();
     void addElem (FBElem* afterElem); // afterElem can be NULL
     void removeElem (FBElem* elem);
     void removeAllElems ();
     bool isVoid ();
     Json::Value toJson ();
     void fromJson (Json::Value jsonVal);

    private:
     bool modified; // will be set to true/false in order to signalize that the
                    // form's elements structure has been changed and there is
                    // a need to save it
};


#endif //FORM_H
