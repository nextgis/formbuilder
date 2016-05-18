/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  factories
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

#include "form_core.h"
#include "elements.h"

class FBFactoryText: public FBFactory
{
    public:
     FBFactoryText (): FBFactory (FB_ELEMNAME_TEXT_LABEL,
          QObject::tr("Text label"), FBDecoration, ":/img/text.png") { }
     ~FBFactoryText () { }
     FBElem *create () { return new FBElemText(this); }
};

class FBFactorySpace: public FBFactory
{
    public:
     FBFactorySpace (): FBFactory (FB_ELEMNAME_SPACE,
          QObject::tr("Space"), FBDecoration, ":/img/space.png") { }
     ~FBFactorySpace () { }
     FBElem *create () { return new FBElemSpace(this); }
};

class FBFactoryTextedit: public FBFactory
{
    public:
     FBFactoryTextedit (): FBFactory (FB_ELEMNAME_TEXT_EDIT,
          QObject::tr("Text edit"), FBInput, ":/img/textedit.png") { }
     ~FBFactoryTextedit () { }
     FBElem *create () { return new FBElemTextedit(this); }
};

class FBFactoryCombobox: public FBFactory
{
    public:
     FBFactoryCombobox (QWidget *appWidget = NULL): FBFactory (FB_ELEMNAME_COMBOBOX,
          QObject::tr("Combobox"), FBInput, ":/img/combo.png")
            { this->appWidget = appWidget; }
     ~FBFactoryCombobox () { }
     FBElem *create () { return new FBElemCombobox(this,appWidget); }
    private:
     QWidget *appWidget;
};

class FBFactoryDoublecombobox: public FBFactory
{
    public:
     FBFactoryDoublecombobox (QWidget *appWidget = NULL):
                    FBFactory (FB_ELEMNAME_DOUBLE_COMBOBOX,
          QObject::tr("Doubled combobox"), FBInput, ":/img/double_combo.png")
            { this->appWidget = appWidget; }
     ~FBFactoryDoublecombobox () { }
     FBElem *create () { return new FBElemDoublecombobox(this,appWidget); }
    private:
     QWidget *appWidget;
};

class FBFactoryCheckbox: public FBFactory
{
    public:
     FBFactoryCheckbox (): FBFactory (FB_ELEMNAME_CHECKBOX,
          QObject::tr("Checkbox"), FBInput, ":/img/check.png") { }
     ~FBFactoryCheckbox () { }
     FBElem *create () { return new FBElemCheckbox(this); }
};

class FBFactoryRadiogroup: public FBFactory
{
    public:
     FBFactoryRadiogroup (QWidget *appWidget = NULL): FBFactory (FB_ELEMNAME_RADIOGROUP,
          QObject::tr("Radiogroup"), FBInput, ":/img/radio.png")
            { this->appWidget = appWidget; }
     ~FBFactoryRadiogroup () { }
     FBElem *create () { return new FBElemRadiogroup(this,appWidget); }
    private:
     QWidget *appWidget;
};

class FBFactoryDatetime: public FBFactory
{
    public:
     FBFactoryDatetime (): FBFactory (FB_ELEMNAME_DATE_TIME,
          QObject::tr("Date & time"), FBInput, ":/img/date.png") { }
     ~FBFactoryDatetime () { }
     FBElem *create () { return new FBElemDatetime(this); }
};

class FBFactoryButton: public FBFactory
{
    public:
     FBFactoryButton (): FBFactory (FB_ELEMNAME_BUTTON,
          QObject::tr("Button"), FBInput, ":/img/button.png") { }
     ~FBFactoryButton () { }
     FBElem *create () { return new FBElemButton(this); }
};

class FBFactoryPhoto: public FBFactory
{
    public:
     FBFactoryPhoto (): FBFactory (FB_ELEMNAME_PHOTO,
          QObject::tr("Photo"), FBInput, ":/img/photo.png") { }
     ~FBFactoryPhoto () { }
     FBElem *create () { return new FBElemPhoto(this); }
};

class FBFactorySignature: public FBFactory
{
    public:
     FBFactorySignature (): FBFactory (FB_ELEMNAME_SIGNATURE,
          QObject::tr("Signature"), FBInput, ":/img/signature.png") { }
     ~FBFactorySignature () { }
     FBElem *create () { return new FBElemSignature(this); }
};

#endif // FACTORIES_H




