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


/**
 * Constructor. Creates all GUI. Registers all factories and creates according
 * GUI components.
 */
FB::FB(QWidget *parent): QWidget(parent), ui(new Ui::FB)
{
    //----------------------------------------------------------------------
    //                              Main settings
    //----------------------------------------------------------------------

    project = NULL;

    //----------------------------------------------------------------------
    //                                 UI
    //----------------------------------------------------------------------

    ui->setupUi(this);
    
    //----------------------------------------------------------------------
    //                              Top menu
    //----------------------------------------------------------------------
    
    tabMenuTop = new QTabWidget(this);
    tabMenuTop->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    tabMenuTop->setMaximumHeight(120);
    tabMenuTop->setMinimumHeight(100);
    tabMenuTop->setFont(QFont("Candara",FB_GUI_FONTSIZE_NORMAL));

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
    toolbsNew.append(addTopMenuButton(wProject,":/img/new_void.png",
                                      tr("New"),tr("New void project")));
    dProjectNew = new FBDialogProjectNew(this); // now use only exec() for it
    //for ()
    //{
    //
    //}
    
    // Open and save buttons.
    toolbOpen = addTopMenuButton(wProject,":/img/open.png",
                                 tr("Open"),tr("Open .ngfp file"));
    toolbSave = addTopMenuButton(wProject,":/img/save.png",
                                 tr("Save"),tr("Save to .ngfp file"));
    toolbSaveAs = addTopMenuButton(wProject,":/img/save_as.png",
                                   tr("Save as"),tr("Save to .ngfp file as ..."));
    
    // All view pickers and combos.
    //for ()
    //{
    //
    //}

    // Tools.
//    toolbUndo = addTopMenuButton(wTools,":/img/undo.png",
//                         tr("Undo"),tr("Cancel last form operation"));
//    toolbRedo = addTopMenuButton(wTools,":/img/redo.png",
//                         tr("Redo"),tr("Return last canceled form operation"));
    toolbClearScreen = addTopMenuButton(wTools,":/img/clear_screen.png",
                         tr("Clear screen"),tr("Clear all screen elements"));
    toolbDeleteElem = addTopMenuButton(wTools,":/img/delete_elem.png",
                         tr("Delete element"),tr("Delete selected element"));
    //for ()
    //{
    //
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
    
    wWorkingArea = new FBWorkingArea(this);
    
    //----------------------------------------------------------------------
    //                              Other gui
    //----------------------------------------------------------------------
    
    labBottom = new QLabel(this);
    labBottom->setText(" ...");
    labBottom->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labBottom->setFont(QFont("Candara",FB_GUI_FONTSIZE_NORMAL));
    labBottom->setText(tr("  Create new or open existing project ..."));

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
                              +QString(FB_COLOR_MEDIUMGREY)+";"
                              "border-bottom: 1px solid "+FB_COLOR_MEDIUMGREY+";"
                              //"border-left: 1px solid "+FB_COLOR_MEDIUMGREY+";"
                              //"border-right: 1px solid "+FB_COLOR_MEDIUMGREY+";"
                              "}"
                              "QTabWidget::tab-bar {"
                              "left: 5px; "
                              "top: 1px; }"
                              "QTabBar::tab {"
                              //"background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                              //"stop: 0 #E1E1E1, stop: 0.4 #DDDDDD,"
                              //"stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);"
                              "border: 1px solid "+FB_COLOR_MEDIUMGREY+";"
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
                              "border-bottom-color: "+FB_COLOR_MEDIUMGREY+";"
                              "margin-top: 2px; }");

    wMenuLeft->setStyleSheet("QWidget#"+wMenuLeft->objectName()+"{"
                             "border-top: 1px solid "+FB_COLOR_MEDIUMGREY+";"
                             "border-bottom: 1px solid "+FB_COLOR_MEDIUMGREY+";"
                             "border-right: 1px solid "+FB_COLOR_MEDIUMGREY+";"
                             "border-top-right-radius: 4px;"
                             "border-bottom-right-radius: 4px;}");

    wMenuRight->setStyleSheet("QWidget#"+wMenuRight->objectName()+"{"
                              "border-top: 1px solid "+FB_COLOR_MEDIUMGREY+";"
                              "border-bottom: 1px solid "+FB_COLOR_MEDIUMGREY+";"
                              "border-left: 1px solid "+FB_COLOR_MEDIUMGREY+";"
                              "border-top-left-radius: 4px;"
                              "border-bottom-left-radius: 4px; }");

    labBottom->setStyleSheet("QLabel {color: "+QString(FB_COLOR_DARKGREY)+"}");

    //----------------------------------------------------------------------
    //                      Other general gui settings
    //----------------------------------------------------------------------

    updateEnableness();
}


/**
 * Destructor
 */
FB::~FB()
{
    delete ui;
}


/****************************************************************************/
/*                          Private FB slots                                */
/****************************************************************************/

void FB::onElemPress ()
{

}

void FB::onNewClick ()
{

}

void FB::onOpenClick ()
{

}

void FB::onSaveClick ()
{

}

void FB::onSaveAsClick ()
{

}

void FB::onScreenStylePick ()
{

}

void FB::onScreenTypePick ()
{

}

void FB::onScreenRatioSelect ()
{

}

void FB::onScreenResolutionSelect ()
{

}

void FB::onUndoClick ()
{

}

void FB::onRedoClick ()
{

}

void FB::onClearScreenClick ()
{

}

void FB::onDeleteElemClick ()
{

}

void FB::onSettingLanguageSelect ()
{

}

void FB::onAboutGraphicsClick ()
{

}

void FB::onElemHighlight ()
{

}

void FB::onLeftArrowClick ()
{

}

void FB::onRightArrowClick ()
{

}


// Show messages.
void FB::showInfo(QString msg)
{
    showBox(msg,tr("Information"));
}
int FB::showWarning (QString msg)
{
    return showBox(msg,tr("Warning"));
}
void FB::showError (QString msg)
{
    showBox(msg,tr("Error"));
}
int FB::showBox (QString msg, QString caption)
{
    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.setWindowTitle(caption);
    QMessageBox::Icon icon;
    if (caption == "Warning")
    {
        icon = QMessageBox::Warning;
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    }
    else
    {
        if (caption == "Error")
        {
            icon = QMessageBox::Critical;
        }
        else
        {
            icon = QMessageBox::Information;
        }
        msgBox.setStandardButtons(QMessageBox::Ok);
    }
    msgBox.setIcon(icon);
    return msgBox.exec();
}


/****************************************************************************/
/*                        Private FB methods                                */
/****************************************************************************/


//void FB::addTopSwitcher ()
//{
//
//}

//void FB::addTopSwitcherGroup ()
//{
//
//}

//void FB::addTopCombo ()
//{
//
//}

// Create new button for any tab of the top menu.
QToolButton *FB::addTopMenuButton (QWidget *parentTab, QString imgPath, QString name,
                                   QString description)
{
    QToolButton *but = new QToolButton(parentTab);
    QHBoxLayout *parentHl = (QHBoxLayout*)parentTab->layout();
    but->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    but->setAutoRaise(true);
    but->setIcon(QIcon(imgPath));
    //but->setText(name);
    but->setFont(QFont("Candara",FB_GUI_FONTSIZE_SMALL));
    but->setIconSize(QSize(60,60));
    but->setMaximumWidth(90);
    //but->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    but->setCursor(Qt::PointingHandCursor);
    parentHl->insertWidget(parentHl->count()-1,but); // last is stretch item.
    but->setStyleSheet("QToolButton"
                       "{"
                           "border: none;"
                       "}"
                       "QToolButton:hover"
                       "{"
                           "background-color: "+QString(FB_COLOR_DARKBLUE)+";"
                       "}"
                       "QToolButton:pressed"
                       "{"
                           "background-color: "+FB_COLOR_DARKBLUE+";"
                       "}"
                       "QToolButton:disabled"
                       "{"
                            // TODO: Make real semitransparent style (~20%).
                       "}");
    but->setToolTip(description);
    return but;
}


void FB::updateEnableness ()
{
    if (project == NULL)
    {
        toolbSave->setEnabled(false);
        toolbSaveAs->setEnabled(false);
    }
    else
    {
        toolbSaveAs->setEnabled(true);
        if (project->wasFirstSaved())
        {
            toolbSave->setEnabled(true);
        }
    }
}



/****************************************************************************/
/*                                                                */
/****************************************************************************/
/**
 *
 */
FBDialogProjectNew::FBDialogProjectNew (QWidget *parent): QDialog(parent)
{
    this->setWindowTitle(tr("Set layer parameters in new project ..."));

    // TODO: set OS-specific colors and fonts (now are parent colors).
    this->setStyleSheet("QWidget { color: black }");
    this->setFont(QFont("Candara",FB_GUI_FONTSIZE_SMALL));

    QLabel *label1 = new QLabel(this);
    label1->setText(tr("Geometry type: "));

    comboGeom = new QComboBox(this);
    for (int i=0; i<FBProject::GEOM_TYPES.size(); i++)
    {
        comboGeom->addItem(FBProject::GEOM_TYPES[i]);
    }

    QPushButton *but1 = new QPushButton(this);
    but1->setText(tr("OK"));
    connect(but1,SIGNAL(clicked()),this,SLOT(accept()));

    QVBoxLayout *vlall = new QVBoxLayout(this);
    QHBoxLayout *hl1 = new QHBoxLayout();
    hl1->addWidget(label1);
    hl1->addWidget(comboGeom);
    vlall->addLayout(hl1);
    vlall->addWidget(but1);
}


/*
FBProject *FBDialogProjectNew::exec ()
{
    int ret = QDialog::exec();

    if (ret)
    {
        FBProject *newProj = new FBProjectVoid(comboGeom->currentText());
        FBErr err = newProj->create();
        if (err == FBErrNone)
        {
            return newProj;
        }
        else
        {
            delete newProj;
            emit sendMsg(tr("Unable create project"));
        }
    }

    return NULL;
}*/




