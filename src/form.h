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

#include "fb_general.h"


/**
 * 
 */
class FBAttr: QObject
{
    Q_OBJECT
    
    public:
     FBAttr ();
     ~FBAttr ();
    
    protected:
    
    private:
    
    signals: 
     void valueChanged (); // in order to signalize to other attrs of this element
                           // that they must be changed
    
};


/**
 * 
 */
class FBElem: public QWidget
{
    public:
     FBElem ();
     ~FBElem ();
     
     void changeStyle (FBScreenStyle styleNew);
    
    protected:
    
    private:
     FBScreenStyle style;
    
};

// ...
class FBElemInput: protected FBElem
{
    public:
    
    protected:
    
    private:
     QString strCaption; 
};

// This compound element may contain other elements which are even also compound.
class FBElemComplex: protected FBElem
{
    public:
    
    protected:
    
    private:
       
};


/**
 * 
 */
class FBInsertWidget: public QWidget 
{
    
};


/**
 * Final form class. Form contains elements and provides the capability to
 * manage them using app events (via mouse).
 * The form will be rendered to the screen. Visually form is "transparent" and 
 * only its elements are rendered to the screen.
 * All elements are always followed each other in the form vertically. This
 * is because the structure of the final JSON file where form is saved. All
 * other layouts and groupings is made via specific elements.
 */
class FBForm: public QWidget
{
    public:
     FBForm ();
     ~FBForm ();
     
     static FBElem *SELECTED;
     static bool IS_SELECTED_MOVING;
    
     void addElem (FBElem* afterElem);
    
    protected:
    
    private:
     bool modified; // will be set to true/false in order to signalize that the
                    // form's elements structure has been changed and there is
                    // a need to save it
};


#endif //FORM_H
