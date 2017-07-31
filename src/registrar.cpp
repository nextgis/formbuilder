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

#include "fb.h"

#include "form/factories.h"
#include "screen/screens.h"

/*****************************************************************************/
/*                                                                           */
/*                               Elements                                    */
/*                                                                           */
/*****************************************************************************/

// Initialize and register the list of factories.
// The method will also register these factories for the form.
// Pass the app widget to this method so some elems and their attributes can use
// it for correctly displaying their input/editing capabilities.
void FB::registerElements ()
{
    this->deregisterElements();

    fctsElems.append(QPair<FBFctelem*,QString>(new FBFctelemText(),":/img/text.png"));
    fctsElems.append(QPair<FBFctelem*,QString>(new FBFctelemSpace(),":/img/space.png"));
    fctsElems.append(QPair<FBFctelem*,QString>(new FBFctelemTextedit(),":/img/textedit.png"));
    fctsElems.append(QPair<FBFctelem*,QString>(new FBFctelemCombobox(this),":/img/combo.png"));
    fctsElems.append(QPair<FBFctelem*,QString>(new FBFctelemDoublecombobox(this),":/img/double_combo.png"));
    fctsElems.append(QPair<FBFctelem*,QString>(new FBFctelemCheckbox(),":/img/check.png"));
    fctsElems.append(QPair<FBFctelem*,QString>(new FBFctelemRadiogroup(this),":/img/radio.png"));
    fctsElems.append(QPair<FBFctelem*,QString>(new FBFctelemDatetime(this),":/img/date.png"));
//    fctsElems.append(QPair<FBFctelem*,QString>(new FBFctelemButton(),":/img/button.png"));
    fctsElems.append(QPair<FBFctelem*,QString>(new FBFctelemPhoto(),":/img/photo.png"));
    fctsElems.append(QPair<FBFctelem*,QString>(new FBFctelemSignature(),":/img/signature.png"));
    fctsElems.append(QPair<FBFctelem*,QString>(new FBFctelemCounter(),":/img/counter.png"));
    fctsElems.append(QPair<FBFctelem*,QString>(new FBFctelemCoordinates(),":/img/coordinates.png"));
    fctsElems.append(QPair<FBFctelem*,QString>(new FBFctelemSplitcombobox(this),":/img/split_combo.png"));

    // FOR DEVELOPERS: add factories here:
    // ...

    for (int i=0; i<fctsElems.size(); i++)
    {
        FBForm::registerElem(fctsElems[i].first);
    }
}

void FB::deregisterElements ()
{
    for (int i=0; i<fctsElems.size(); i++)
    {
        delete fctsElems[i].first;
    }
    fctsElems.clear();
}


/*****************************************************************************/
/*                                                                           */
/*                                 Screens                                   */
/*                                                                           */
/*****************************************************************************/

/*
QList<QPair<FBFctscreen*,QString> > FB::FCT_SCREENS;

void FB::registerScreens ()
{

}

void FB::deregisterScreens ()
{

}
*/

