/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  main app class
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

#include "fb.h"
#include "ui_fb.h"


/****************************************************************************/
/*                                Constructor                               */
/****************************************************************************/
/**
 * Creates all GUI. Inits and registers all arrays and types.
 */
FB::FB(QWidget *parent): QWidget(parent), ui(new Ui::FB)
{
    ui->setupUi(this);
    
    //----------------------------------------------------------------------
    //                              Top menu
    //----------------------------------------------------------------------
    
    tabMenuTop = new QTabWidget(this);
    tabMenuTop->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    tabMenuTop->setMaximumHeight(120);
    tabMenuTop->setMinimumHeight(100);
    tabMenuTop->setFont(QFont("Candara",FB_GUI_FONTSIZE));

    wProject = new QWidget();
    tabMenuTop->addTab(wProject, tr(" Project "));
    QHBoxLayout *layProject = new QHBoxLayout(wProject);
    layProject->setContentsMargins(0,0,0,0);
    layProject->addStretch();
    
    wView = new QWidget();
    tabMenuTop->addTab(wView, tr("  View  "));
    QHBoxLayout *layView = new QHBoxLayout(wView);
    layView->addStretch();
    
    wTools = new QWidget();
    tabMenuTop->addTab(wTools, tr("  Tools  "));
    QHBoxLayout *layTools = new QHBoxLayout(wTools);
    layTools->addStretch();
    
    wSettings = new QWidget();
    tabMenuTop->addTab(wSettings, tr(" Settings "));
    QHBoxLayout *laySettings = new QHBoxLayout(wSettings);
    laySettings->addStretch();
    
    wAbout = new QWidget();
    tabMenuTop->addTab(wAbout, tr(" About "));
    QHBoxLayout *layAbout = new QHBoxLayout(wAbout);
    layAbout->addStretch();
    
    // All new project buttons.
    //for ()
    //{
        
    //}
    
    // Open button.
    
    
    // Save buttons.
    
    // All view buttons.
    //for ()
    //{
        
    //}
    
    // All tools buttons.
    //for ()
    //{
        
    //}
    
    // Settings.

    // About.
    
    //----------------------------------------------------------------------
    //                              Left menu
    //----------------------------------------------------------------------
    
    wMenuLeft = new QWidget(this);
    wMenuLeft->setObjectName("wMenuLeft");
    wMenuLeft->setMaximumWidth(240);
    wMenuLeft->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);
    QHBoxLayout *lhMenuLeft = new QHBoxLayout(wMenuLeft);

    //registerAllElements();
    //for ()
    //{
        
    //}
    
    //----------------------------------------------------------------------
    //                              Right menu
    //----------------------------------------------------------------------
    // see left menu for comments

    wMenuRight = new QWidget(this);
    wMenuRight->setObjectName("wMenuRight");
    wMenuRight->setMaximumWidth(300);
    wMenuRight->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);
    QHBoxLayout *lhMenuRight = new QHBoxLayout(wMenuRight);
    
    //----------------------------------------------------------------------
    //                              Working area
    //----------------------------------------------------------------------
    
    wWorkingArea = new QWidget(this);
    wWorkingArea->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    QVBoxLayout *lvWorkingArea = new QVBoxLayout(wWorkingArea);
    
    //----------------------------------------------------------------------
    //                              Other gui
    //----------------------------------------------------------------------
    
    labBottom = new QLabel(this);
    labBottom->setText(" ...");
    labBottom->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    //----------------------------------------------------------------------
    //                            Layout all window
    //----------------------------------------------------------------------

    QHBoxLayout *lhMiddle = new QHBoxLayout();
    lhMiddle->addWidget(wMenuLeft);
    lhMiddle->addWidget(wWorkingArea);
    lhMiddle->addWidget(wMenuRight);

    lvAll = new QVBoxLayout(this);
    lvAll->addWidget(tabMenuTop);
    lvAll->addLayout(lhMiddle);
    lvAll->addWidget(labBottom);
    lvAll->setContentsMargins(0,5,0,5);
    lvAll->setSpacing(10);
    
    //----------------------------------------------------------------------
    //                   Set application general stylesheets
    //----------------------------------------------------------------------

    // From docs:
    // " ... style authors are restricted by the different platforms' guidelines
    // and (on Windows XP and OS X) by the native theme engine."
    // Note: regardless of the next statement from docs the whole app color
    // setting works anyway:
    // "When using Qt Style Sheets, a widget does not automatically inherit
    // its font and color setting from its parent widget".
    this->setStyleSheet("QWidget { background: white;"
                                  " color: black; }");

    tabMenuTop->setStyleSheet("QTabWidget::pane {"
                              "border-top: 1px solid "
                              +QString(FB_COLOR_DARKGREY)+";"
                              "border-bottom: 1px solid "+FB_COLOR_DARKGREY+";"
                              //"border-left: 1px solid "+FB_COLOR_DARKGREY+";"
                              //"border-right: 1px solid "+FB_COLOR_DARKGREY+";"
                              "}"
                              "QTabWidget::tab-bar {"
                              "left: 5px; "
                              "top: 1px; }"
                              "QTabBar::tab {"
                              //"background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                              //"stop: 0 #E1E1E1, stop: 0.4 #DDDDDD,"
                              //"stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);"
                              "border: 1px solid "+FB_COLOR_DARKGREY+";"
                              "border-bottom-color: white; "
                              "border-top-left-radius: 4px;"
                              "border-top-right-radius: 4px;"
                              "min-width: 8ex;"
                              "padding: 2px;"
                              "color: black }"
                              //"QTabBar::tab:selected, QTabBar::tab:hover {"
                              //"background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                              //"stop: 0 #fafafa, stop: 0.4 #f4f4f4,"
                              //"stop: 0.5 #e7e7e7, stop: 1.0 #fafafa);}"
                              //"QTabBar::tab:selected {"
                              //"border-color: #9B9B9B;"
                              //"border-bottom-color: rgb(255,255,255); }"
                              "QTabBar::tab:!selected {"
                              "border: 1px solid white;"
                              "border-bottom-color: "+FB_COLOR_DARKGREY+";"
                              "margin-top: 2px; }");

    wMenuLeft->setStyleSheet("QWidget#"+wMenuLeft->objectName()+"{"
                             "border-top: 1px solid "+FB_COLOR_DARKGREY+";"
                             "border-bottom: 1px solid "+FB_COLOR_DARKGREY+";"
                             "border-right: 1px solid "+FB_COLOR_DARKGREY+";"
                             "border-top-right-radius: 4px;"
                             "border-bottom-right-radius: 4px;}");

    wMenuRight->setStyleSheet("QWidget#"+wMenuRight->objectName()+"{"
                              "border-top: 1px solid "+FB_COLOR_DARKGREY+";"
                              "border-bottom: 1px solid "+FB_COLOR_DARKGREY+";"
                              "border-left: 1px solid "+FB_COLOR_DARKGREY+";"
                              "border-top-left-radius: 4px;"
                              "border-bottom-left-radius: 4px; }");

    wWorkingArea->setStyleSheet("QWidget {background-color: "
                                +QString(FB_COLOR_LIGHTGREY)+";"
                                "border-top-left-radius: 4px;"
                                "border-top-right-radius: 4px;"
                                "border-bottom-left-radius: 4px;"
                                "border-bottom-right-radius: 4px;}");

    //----------------------------------------------------------------------
    //                              Other settings
    //----------------------------------------------------------------------

    project = NULL;
}


/****************************************************************************/
/*                                Destructor                                */
/****************************************************************************/
/**
 *
 */
FB::~FB()
{
    delete ui;
}
