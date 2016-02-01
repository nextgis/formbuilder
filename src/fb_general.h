/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  common declarations
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

#ifndef FB_CORE_H
#define FB_CORE_H

// TODO: think about how to set these constants via build system:
#define _FB_VERSION 2.1

// Colors for stylesheets.
#define FB_COLOR_LIGHTGREY "rgb(238,238,238)"
#define FB_COLOR_DARKGREY "rgb(170,170,170)"


// Different final screens can have one screen style, e.g. there are android 
// vertical, horizontal and maximized screens.
enum FBScreenStyle
{
    FBScreenAndroid,
};

#endif //FB_CORE_H
