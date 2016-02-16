/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  registrar for elem types
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

#include "form_core.h"

QList<FBFactory*> FBRegistrar::fctsElem;

void FBRegistrar::initAll ()
{
    FBRegistrar::deinitAll(); // clear list
/*
    FBRegistrar::fctsElem.append(new FBFctElemText("text",
               tr("Text label"), tr("Decoration"), ":/img/text.png"));
    FBRegistrar::fctsElem.append(new FBFctElemTextedit("textedit",
               tr("Text edit"), tr("Input"), ":/img/textedit.png"));
    FBRegistrar::fctsElem.append(new FBFctElemCombobox("combobox",
               tr("Combobox"), tr("Input"), ":/img/combo.png"));
    FBRegistrar::fctsElem.append(new FBFctElemCheckbox("checkbox",
               tr("Checkbox"), tr("Input"), ":/img/check.png"));
*/
    // DEVELOPERS: add new elem factories here.
    // WARNING. Keynames of elements must be unique.
    //...
}

void FBRegistrar::deinitAll ()
{
    for (int i=0; i<FBRegistrar::fctsElem.size(); i++)
    {
        delete FBRegistrar::fctsElem[i];
    }
    FBRegistrar::fctsElem.clear();
}

FBFactory *FBRegistrar::getFctByName (QString keyName)
{
    for (int i=0; i<FBRegistrar::fctsElem.size(); i++)
    {
        if (FBRegistrar::fctsElem[i]->getKeyName() == keyName)
        {
            return FBRegistrar::fctsElem[i];
        }
    }
    return NULL;
}


