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

#ifndef FB_COMMON_H
#define FB_COMMON_H

// Common colors for stylesheets: app's gui, screen, elements, etc.
#define FB_COLOR_LIGHTGREY "rgb(238,238,238)"
#define FB_COLOR_LIGHTMEDIUMGREY "rgb(210,210,210)"
#define FB_COLOR_MEDIUMGREY "rgb(170,170,170)"
#define FB_COLOR_DARKGREY "rgb(100,100,100)"
#define FB_COLOR_LIGHTBLUE "rgb(139,183,224)"
#define FB_COLOR_DARKBLUE "rgb(23,111,193)"

// Error codes.
enum FBErr
{
    FBErrNone, FBErrUnsupported, FBErrUnableCreateProject, FBErrNotInited,
    FBErrAlreadyInited, FBErrWrongVersion, FBErrIncorrectJson,
    FBErrIncorrectFileStructure,
};

#endif //FB_COMMON_H
