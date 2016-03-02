/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  elements declaration
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

#ifndef ELEMENTS_H
#define ELEMENTS_H

#include <QVBoxLayout>
#include <QLabel>

#include "form_core.h"
#include "attributes.h"

#define FB_ANDROIDSCREEN_FONTSIZE_NORMAL 14
#define FB_ANDROIDSCREEN_FONTTYPE "Candara"


// Element which can write values to the layer fields.
class FBElemInput: public FBElem
{
    Q_OBJECT
    public:
     FBElemInput (FBFactory *fctPtr);
     virtual ~FBElemInput () { }
     void updateFields (QStringList fieldsKeyNames);
    protected:
     FBAttrField *attrFieldPtr;
     QString caption;
};


/*
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
     // The elems are stored inside layout(s) and will be read for output as
     // it is done for the whole form - via layout's iteration.

};
*/

class FBElemText: public FBElem
{
    Q_OBJECT
    public:
     FBElemText (FBFactory *fctPtr);
     ~FBElemText () {}
     void changeStyle (QString styleName);
    protected slots:
     void changeAttrValue ();
     void updateAppearance ();
    protected:
     QLabel *labText;
     FBAttrText *attrTextPtr;
};

class FBElemTextedit: public FBElemInput
{
    Q_OBJECT
    public:
     FBElemTextedit (FBFactory *fctPtr);
     ~FBElemTextedit () {}
     void changeStyle (QString styleName);
    protected slots:
     void changeAttrValue ();
     void updateAppearance ();
    protected:
     QLabel *labText;
     FBAttrText *attrTextPtr;
};

class FBElemCombobox: public FBElemInput
{
    Q_OBJECT
    public:
     FBElemCombobox (FBFactory *fctPtr, QWidget *appWidget = NULL);
     ~FBElemCombobox () {}
     void changeStyle (QString styleName);
    protected slots:
     void changeAttrValue ();
     void updateAppearance ();
    protected:
     QLabel *labText;
     FBAttrListvalues *attrListvalsPtr;
};

class FBElemDatetime: public FBElemInput
{
    Q_OBJECT
    public:
     FBElemDatetime (FBFactory *fctPtr);
     ~FBElemDatetime () {}
     void changeStyle (QString styleName);
    protected slots:
     void changeAttrValue ();
     void updateAppearance ();
    protected:
     QLabel *labText;
};


#endif //ELEMENTS_H




