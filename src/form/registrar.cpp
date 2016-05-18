/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  registrar for types
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

#include "factories.h"

QList<FBFactory*> FBFactory::fctsElem;

// Initialize and register the list of factories.
// Pass the app widget to this method so some elems and their attributes can use
// it for correctly displaying their input/editing capabilities.
void FBFactory::initAll (QWidget *appWidget)
{
    FBFactory::deinitAll(); // clear list

    fctsElem.append(new FBFactoryText());
    fctsElem.append(new FBFactorySpace());
    fctsElem.append(new FBFactoryTextedit());
    fctsElem.append(new FBFactoryCombobox(appWidget));
    fctsElem.append(new FBFactoryDoublecombobox(appWidget));
    fctsElem.append(new FBFactoryCheckbox());
    fctsElem.append(new FBFactoryRadiogroup(appWidget));
    fctsElem.append(new FBFactoryDatetime());
    fctsElem.append(new FBFactoryButton());
    fctsElem.append(new FBFactoryPhoto());
    fctsElem.append(new FBFactorySignature());

    // FOR DEVELOPERS. Register new factories here.
    //...
}

void FBFactory::deinitAll ()
{
    for (int i=0; i<fctsElem.size(); i++)
    {
        delete fctsElem[i];
    }
    fctsElem.clear();
}

FBFactory *FBFactory::getFctByName (QString keyName)
{
    for (int i=0; i<fctsElem.size(); i++)
    {
        if (fctsElem[i]->getKeyName() == keyName)
        {
            return fctsElem[i];
        }
    }
    return NULL;
}


