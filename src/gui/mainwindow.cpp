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

#include "mainwindow.h"

#include "sizes.h"
#include "gui_common.h"
#include "util/common.h"

#include <QMenuBar>
#include <QToolBar>
#include <QDockWidget>

using namespace Fb;
using namespace Gui;


MainWindow::MainWindow ():
    QMainWindow()
{
    this->setObjectName("main_window");

    //this->set
}

MainWindow::~MainWindow ()
{
}


void MainWindow::setColorTheme (ColorTheme theme)
{
    QString style_sheet = Util::g_loadStyleSheet(g_getColorThemeStyleFilePath(theme));
    this->setStyleSheet(style_sheet);
    cur_color_theme = theme;
}


QMenu *MainWindow::addMainMenuEntry (QString text)
{
    return this->menuBar()->addMenu(text);
}

QToolBar *MainWindow::addMainMenuToolBar (QString name)
{
    QToolBar *toolb = this->addToolBar(name);
    toolb->setIconSize({TOOLB_SIZE, TOOLB_SIZE});
    toolb->setFloatable(false);
    return toolb;
}

QDockWidget *MainWindow::addDockMenu (QString caption, Qt::DockWidgetArea position)
{
    QDockWidget *dock_widget = new QDockWidget(caption, this);
    dock_widget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    this->addDockWidget(position, dock_widget);
    return dock_widget;
}

