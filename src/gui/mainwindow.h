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

#include "color_themes.h"

#include <QMainWindow>
#include <QMenu>
#include <QToolBar>

namespace Fb
{
namespace Gui
{


/*!
 * @brief A base for main-window classes with some common features.
 */
class MainWindow: public QMainWindow
{
    Q_OBJECT

    public:

     MainWindow ();
     ~MainWindow ();

     void setColorTheme (ColorTheme theme);

    protected:

//     template <typename Slot>
//     QAction *addMainMenuAction (QMenu *menu, QToolBar *toolb, Slot slot,
//                                 QString icon, QString name, QString tip);

     QMenu *addMainMenuEntry (QString text);
     QToolBar *addMainMenuToolBar (QString name);
     QDockWidget *addDockMenu (QString caption, Qt::DockWidgetArea position);

     ColorTheme cur_color_theme;
};


}
}
