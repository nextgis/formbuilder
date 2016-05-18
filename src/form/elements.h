/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  various elements declaration
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


// Abstract element which can write values to the layer fields.
class FBElemInput: public FBElem
{
    Q_OBJECT
    public:
     FBElemInput (FBFactory *fctPtr);
     virtual ~FBElemInput () { }
     static void updateFields (QStringList fieldsKeyNames);
     virtual void resetSelectedField (QString keyname);
     virtual QStringList getSelectedFields ();
    protected:
     FBAttrField *attrFieldPtr;
     //FBAttrText *attrCaption;
};

// Abstract element which can write values to the layer fields and these values
// can vary in different sessions of working with form on the device.
class FBElemInputVariate: public FBElemInput
{
    Q_OBJECT
    public:
     FBElemInputVariate (FBFactory *fctPtr);
     virtual ~FBElemInputVariate () { }
    protected:
     FBAttrBoolean *attrKeepLastPtr;
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

class FBElemSpace: public FBElem
{
    Q_OBJECT
    public:
     FBElemSpace (FBFactory *fctPtr);
     ~FBElemSpace () {}
     void changeStyle (QString styleName);
    protected slots:
     void changeAttrValue () { return; }
     void updateAppearance () { return; }
};

class FBElemTextedit: public FBElemInputVariate
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

class FBElemCombobox: public FBElemInputVariate
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

class FBElemDoublecombobox: public FBElemInputVariate
{
    Q_OBJECT
    public:
     FBElemDoublecombobox (FBFactory *fctPtr, QWidget *appWidget = NULL);
     ~FBElemDoublecombobox () {}
     void changeStyle (QString styleName);
    protected slots:
     void changeAttrValue ();
     void updateAppearance ();
    protected:
     QLabel *labText1;
     QLabel *labText2;
     FBAttrListvaluesDouble *attrListvalsDoublePtr;
};

class FBElemCheckbox: public FBElemInputVariate
{
    Q_OBJECT
    public:
     FBElemCheckbox (FBFactory *fctPtr);
     ~FBElemCheckbox () {}
     void changeStyle (QString styleName);
    protected slots:
     void changeAttrValue ();
     void updateAppearance ();
    protected:
     QLabel *labText;
     QLabel *labImg;
     QPixmap pixOnAndroid;
     QPixmap pixOffAndroid;
     FBAttrText *attrTextPtr;
     FBAttrBoolean *attrCheckPtr;
     QString curStyleName;
};

class FBElemRadiogroup: public FBElemInputVariate
{
    Q_OBJECT
    public:
     FBElemRadiogroup (FBFactory *fctPtr, QWidget *appWidget = NULL);
     ~FBElemRadiogroup () {}
     void changeStyle (QString styleName);
    protected slots:
     void changeAttrValue ();
     void updateAppearance ();
    protected:
     QPixmap pixOnAndroid;
     QPixmap pixOffAndroid;
     FBAttrListvaluesStrict *attrListvalsStrictPtr;
     QString curStyleName;
};

class FBElemDatetime: public FBElemInputVariate
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

class FBElemButton: public FBElemInput
{
    Q_OBJECT
    public:
     FBElemButton (FBFactory *fctPtr);
     ~FBElemButton () {}
     void changeStyle (QString styleName);
    protected slots:
     void changeAttrValue ();
     void updateAppearance ();
    protected:
     QLabel *labText;
     FBAttrText *attrTextPtr;
};

class FBElemPhoto: public FBElem
{
    Q_OBJECT
    public:
     FBElemPhoto (FBFactory *fctPtr);
     ~FBElemPhoto () {}
     void changeStyle (QString styleName);
    protected slots:
     void changeAttrValue ();
     void updateAppearance ();
    protected:
     QHBoxLayout *lhItems;
     QList<QLabel*> labsImg; // store these labels to delete them separetely
     FBAttrNumber *attrMaxCountPtr;
     QString curStyleName;
};

class FBElemSignature: public FBElem
{
    Q_OBJECT
    public:
     FBElemSignature (FBFactory *fctPtr);
     ~FBElemSignature () {}
     void changeStyle (QString styleName);
    protected slots:
     void changeAttrValue () { return; }
     void updateAppearance () { return; }
};


#endif //ELEMENTS_H




