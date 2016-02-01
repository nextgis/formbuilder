/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  screen definitions
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

#ifndef SCREEN_H
#define SCREEN_H

#include <QWidget>

#include "fb_general.h"
#include "form.h"


/**
 * Abstract screen class. Each final screen will be rendered to the working area
 * of the application with its specific decorations. Contains the scrolled area 
 * to which the form will be rendered.
 */
class FBScreen: public QWidget
{
    public:
    
     void setForm (FBForm* form);
     void changeStyle (FBScreenStyle styleNew);
     void maximize ();
    
    protected:
    
    private:
     FBForm *form; 
     FBScreenStyle style;
    
};

#endif //SCREEN_H
