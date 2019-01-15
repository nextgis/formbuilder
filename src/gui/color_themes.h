/**************************************************************************************************
 *                                                                                                *
 *    Project:  NextGIS Formbuilder                                                               *
 *    Authors:  Mikhail Gusev, gusevmihs@gmail.com                                                *
 *              Copyright (C) 2014-2019 NextGIS                                                   *
 *                                                                                                *
 *    This program is free software: you can redistribute it and/or modify it under the terms     *
 *    of the GNU General Public License as published by the Free Software Foundation, either      *
 *    version 2 of the License, or (at your option) any later version.                            *
 *                                                                                                *
 *    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;   *
 *    without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   *
 *    See the GNU General Public License for more details.                                        *
 *                                                                                                *
 *    You should have received a copy of the GNU General Public License along with this program.  *
 *    If not, see http://www.gnu.org/licenses/.                                                   *
 *                                                                                                *
 **************************************************************************************************/

#pragma once

#include <QObject>
#include <QMap>

namespace Fb
{
namespace Gui
{


enum class ColorTheme
{
    NoTheme, // system default
    White
};

struct ColorThemeData
{
    QString display_name;
    QString style_file_path;
    QString icons_dir;
};


const QMap<ColorTheme, ColorThemeData> COLOR_THEMES =
{
    {{ColorTheme::White}, {"White", ":/styles/theme_white.qss", ":/images/theme_white/"}}
};


inline QString g_getColorThemeStyleFilePath (ColorTheme key)
{
    auto data = COLOR_THEMES.value(key);
    return data.style_file_path;
}

inline QString g_getColorThemeIconsDir (ColorTheme key)
{
    auto data = COLOR_THEMES.value(key);
    return data.icons_dir;
}


}
}



