/**************************************************************************************************
 *                                                                                                *
 *    Project:  NextGIS Formbuilder                                                               *
 *    Authors:  Mikhail Gusev, gusevmihs@gmail.com                                                *
 *              Copyright (C) 2014-2018 NextGIS                                                   *
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

#include <QWidget>
#include <QFile>
#include <QStyleOption>
#include <QPainter>
#include <QMessageBox>

namespace Fb
{
namespace Util
{


// Load style sheet from a file into a string.
inline QString g_loadStyleSheet (QString path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        //qDebug
        return "";
    }
    QString str = file.readAll();
    file.close();
    return str;
}

// Use these common commands in paint evet implementations of all custom widgets which need to
// use custom style sheets.
inline void g_paintEventImpl (QWidget *widget)
{
    QStyleOption o;
    o.initFrom(widget);
    QPainter p(widget);
    widget->style()->drawPrimitive(QStyle::PE_Widget, &o, &p, widget);
}


}
}



