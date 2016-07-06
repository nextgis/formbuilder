/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  factories for various elements
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

#ifndef FACTORIES_H
#define FACTORIES_H

#include "elements.h"

class FBFctelemText: public FBFctelem
{
    public:
     FBFctelemText (): FBFctelem () { }
     ~FBFctelemText () { }
     FBElem *create () { return new FBElemText(); }
};

class FBFctelemSpace: public FBFctelem
{
    public:
     FBFctelemSpace (): FBFctelem () { }
     ~FBFctelemSpace () { }
     FBElem *create () { return new FBElemSpace(); }
};

class FBFctelemTextedit: public FBFctelem
{
    public:
     FBFctelemTextedit (): FBFctelem () { }
     ~FBFctelemTextedit () { }
     FBElem *create () { return new FBElemTextedit(); }
};

class FBFctelemCombobox: public FBFctelemAppwidget
{
    public:
     FBFctelemCombobox (QWidget *appWidget): FBFctelemAppwidget (appWidget) { }
     ~FBFctelemCombobox () { }
     FBElem *create () { return new FBElemCombobox(appWidget); }
};

class FBFctelemDoublecombobox: public FBFctelemAppwidget
{
    public:
     FBFctelemDoublecombobox (QWidget *appWidget): FBFctelemAppwidget (appWidget) { }
     ~FBFctelemDoublecombobox () { }
     FBElem *create () { return new FBElemDoublecombobox(appWidget); }
};

class FBFctelemCheckbox: public FBFctelem
{
    public:
     FBFctelemCheckbox (): FBFctelem () { }
     ~FBFctelemCheckbox () { }
     FBElem *create () { return new FBElemCheckbox(); }
};

class FBFctelemRadiogroup: public FBFctelemAppwidget
{
    public:
     FBFctelemRadiogroup (QWidget *appWidget): FBFctelemAppwidget (appWidget) { }
     ~FBFctelemRadiogroup () { }
     FBElem *create () { return new FBElemRadiogroup(appWidget); }
};

class FBFctelemDatetime: public FBFctelemAppwidget
{
    public:
     FBFctelemDatetime (QWidget *appWidget): FBFctelemAppwidget (appWidget) { }
     ~FBFctelemDatetime () { }
     FBElem *create () { return new FBElemDatetime(appWidget); }
};

class FBFctelemButton: public FBFctelem
{
    public:
     FBFctelemButton (): FBFctelem () { }
     ~FBFctelemButton () { }
     FBElem *create () { return new FBElemButton(); }
};

class FBFctelemPhoto: public FBFctelem
{
    public:
     FBFctelemPhoto (): FBFctelem () { }
     ~FBFctelemPhoto () { }
     FBElem *create () { return new FBElemPhoto(); }
};

class FBFctelemSignature: public FBFctelem
{
    public:
     FBFctelemSignature (): FBFctelem () { }
     ~FBFctelemSignature () { }
     FBElem *create () { return new FBElemSignature(); }
};

#endif // FACTORIES_H

