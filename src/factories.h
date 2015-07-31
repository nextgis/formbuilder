/******************************************************************************
 *
 * Name:     factories.h
 * Project:  NextGIS Form Builder
 * Purpose:  Factories classes declaration.
 * Author:   NextGIS
 *
 ******************************************************************************
 * Copyright (c) 2014, 2015 NextGIS
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#ifndef FACTORIES_H
#define FACTORIES_H

#include "elements.h"

#include <QTreeWidget>

class FBFactory: public QTreeWidgetItem
{
    public:
     FBFactory(bool isFull, QString strJsonName, QString imgPath, QString text);
     virtual ~FBFactory() {}
     virtual FBElem *Create() = 0;
     QString getJsonName () { return strJsonName; }
     QString getDisplayName () { return strDisplayName; }
    protected:
     QString strJsonName;
     QString strDisplayName;
     QString strIconPath;
     bool bIsFull;
};


// ================================================================================== //
//                                   Decoration                                       //
// ===================================================================================//

class FBTextLabelFactory: public FBFactory
{
    public:
     FBTextLabelFactory (bool isFull);
     ~FBTextLabelFactory () {}
     FBElem *Create ();
};

class FBSpaceFactory: public FBFactory
{
    public:
     FBSpaceFactory (bool isFull);
     ~FBSpaceFactory () {}
     FBElem *Create ();
};


// ================================================================================== //
//                                     Input                                          //
// ===================================================================================//

class FBTextEditFactory: public FBFactory
{
    public:
     FBTextEditFactory (bool isFull);
     ~FBTextEditFactory () {}
     FBElem *Create ();
};

class FBComboBoxFactory: public FBFactory
{
    public:
     FBComboBoxFactory (bool isFull);
     ~FBComboBoxFactory () {}
     FBElem *Create ();
};

class FBDoubleComboBoxFactory: public FBFactory
{
    public:
     FBDoubleComboBoxFactory (bool isFull);
     ~FBDoubleComboBoxFactory () {}
     FBElem *Create ();
};

class FBCheckBoxFactory: public FBFactory
{
    public:
     FBCheckBoxFactory (bool isFull);
     ~FBCheckBoxFactory () {}
     FBElem *Create ();
};

class FBRadioGroupFactory: public FBFactory
{
    public:
     FBRadioGroupFactory (bool isFull);
     ~FBRadioGroupFactory () {}
     FBElem *Create ();
};

class FBButtonFactory: public FBFactory
{
    public:
     FBButtonFactory (bool isFull);
     ~FBButtonFactory () {}
     FBElem *Create ();
};

class FBDateTimeFactory: public FBFactory
{
    public:
     FBDateTimeFactory (bool isFull);
     ~FBDateTimeFactory () {}
     FBElem *Create ();
};

class FBPhotoFactory: public FBFactory
{
    public:
     FBPhotoFactory (bool isFull);
     ~FBPhotoFactory () {}
     FBElem *Create ();
};

class FBSignatureFactory: public FBFactory
{
    public:
     FBSignatureFactory (bool isFull);
     ~FBSignatureFactory () {}
     FBElem *Create ();
};


#endif // FACTORIES_H



