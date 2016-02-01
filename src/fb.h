/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  main app class definitions
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

#ifndef FB_H
#define FB_H

#include <QWidget>
#include <QLabel>
#include <QTabWidget>
#include <QTreeWidget>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "fb_general.h"
#include "project.h"
#include "screen.h"


// Different sizes for GUI.
#define FB_GUI_FONTSIZE 11


namespace Ui
{
    class FB;
}

/**
 * App's window class. Aggregates all GUI of the app, except specific dialogues. 
 * Contains the working area to which the screen with its form is rendered.
 */
class FB: public QWidget
{
    Q_OBJECT

    public:
    
     explicit FB (QWidget *parent = 0);
     ~FB();
    
    protected:
    
    private:

     // main gui
     Ui::FB *ui;
     QVBoxLayout *lvAll;
    
     // top menu
     QTabWidget *tabMenuTop;
     QWidget *wProject;
     QWidget *wView;
     QWidget *wTools;
     QWidget *wSettings;
     QWidget *wAbout;
     
     // left menu
     QWidget *wMenuLeft;
     QTreeWidget *treeLeft1;
     QTreeWidget *treeLeft2;
     
     // right menu
     QWidget *wMenuRight;
     
     // working area
     QWidget *wWorkingArea;
     QScrollArea *scrWorkingArea;
     FBScreen *screen;
          
     // other gui
     QLabel *labBottom;

     // Current project of the app.
     // For future here can be an array of projects - but that needs changes
     // in FB behaviour and appearance.
     FBProject *project;
    
};

#endif //FB_H
