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
 
#include "gui_globals.h"

namespace Fb
{
namespace Gui
{


/*!
 * @brief Show a common message dialog with one "Ok" button.
 *
 * @param wParent Parent's window must be passed explicitly so the message box can be correctly
 * displayed as a modal dialog over it.
 */
void g_showMsgBox (QWidget *wParent, QString sText, bool isCritical)
{
    QMessageBox box(wParent);
    box.setText(sText);
    box.setStandardButtons(QMessageBox::Ok);
    if (!isCritical)
    {
        box.setWindowTitle(QObject::tr("Warning"));
        box.setIcon(QMessageBox::Warning);
    }
    else
    {
        box.setWindowTitle(QObject::tr("Error"));
        box.setIcon(QMessageBox::Critical);
    }
    box.exec();
}


/*!
 * @brief Show a common message dialog with "Ok" and "Cancel" buttons.
 *
 * @param wParent Parent's window must be passed explicitly so the message box can be correctly
 * displayed as a modal dialog over it.
 */
int g_showQuestionBox (QWidget *wParent, QString sText)
{
    QMessageBox box(wParent);
    box.setText(sText);
    box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    box.setWindowTitle(QObject::tr("Question"));
    box.setIcon(QMessageBox::Warning);
    return box.exec();
}


}
}



