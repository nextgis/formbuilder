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
     FBFactoryText (): FBFactory ("text",
          QObject::tr("Text label"), FBDecoration, ":/img/text.png") { }
     ~FBFactoryText () {}
     FBElem *create () { return new FBElemText(this); }
};

class FBFactoryTextedit: public FBFactory
{
    public:
     FBFactoryTextedit (): FBFactory ("text_edit",
          QObject::tr("Text edit"), FBInput, ":/img/textedit.png") { }
     ~FBFactoryTextedit () {}
     FBElem *create () { return new FBElemTextedit(this); }
};


#endif // FACTORIES_H




