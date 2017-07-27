/***************************************************************************************************
 *    Project:  NextGIS Formbuilder
 *    Author:   Mikhail Gusev, gusevmihs@gmail.com
 ***************************************************************************************************
 *    Copyright (C) 2014-2017 NextGIS
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
 **************************************************************************************************/

#ifndef GUI_GLOBALS_H
#define GUI_GLOBALS_H

#include <QString>
#include <QMessageBox>
#include <QDebug>


// TODO: move this macro to another file after refactoring.
#define FB_DEBUG(s) qDebug() << QString("[") + this->staticMetaObject.className() + "] " + s


namespace Fb
{
namespace Gui
{

void g_showMsgBox (QWidget *wParent, QString sText, bool isCritical);

int g_showQuestionBox (QWidget *wParent, QString sText);

}
}

#endif //GUI_GLOBALS_H



