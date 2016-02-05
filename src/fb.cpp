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


FB::~FB()
{
    delete ui;
}

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
    layView->setContentsMargins(4,4,4,4);
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
    
    // Project buttons.
    toolbNewVoid = addTopMenuButton(wProject,":/img/new_void.png",
                        tr("New"),tr("New void project"));
    QObject::connect(toolbNewVoid, SIGNAL(clicked()),
                     this, SLOT(onNewVoidClick()));
    toolbNewShape = addTopMenuButton(wProject,":/img/new_shp.png",
            tr("New from Shapefile"),tr("New project from \nShapefile"));
    QObject::connect(toolbNewShape, SIGNAL(clicked()),
                     this, SLOT(onNewShapeClick()));
    toolbNewNgw = addTopMenuButton(wProject,":/img/new_ngw.png",
      tr("New from NextGIS Web"),tr("New project from \nNextGIS Web connection"));
    QObject::connect(toolbNewNgw, SIGNAL(clicked()),
                     this, SLOT(onNewNgwClick()));
    toolbOpen = addTopMenuButton(wProject,":/img/open.png",
                         tr("Open"),tr("Open .ngfp file"));
    QObject::connect(toolbOpen, SIGNAL(clicked()),
                     this, SLOT(onOpenClick()));
    toolbSave = addTopMenuButton(wProject,":/img/save.png",
                          tr("Save"),tr("Save to .ngfp file"));
    QObject::connect(toolbSave, SIGNAL(clicked()),
                     this, SLOT(onSaveClick()));
    toolbSaveAs = addTopMenuButton(wProject,":/img/save_as.png",
                    tr("Save as"),tr("Save to .ngfp file \nas ..."));
    QObject::connect(toolbSaveAs, SIGNAL(clicked()),
                     this, SLOT(onSaveAsClick()));
    
    // All view pickers and combos.
    for (int i=0; i<FBWorkingArea::STYLES.size(); i++)
    {
        toolbsScreenStyle.append(addTopMenuButton(wView,
               FBWorkingArea::STYLES[i].second, FBWorkingArea::STYLES[i].first,
               FBWorkingArea::STYLES[i].first + tr(" screen style"),true));
        QObject::connect(toolbsScreenStyle.last(),SIGNAL(clicked()),
                         this,SLOT(onScreenStylePick()));
    }
    addTopMenuSplitter(wView);
    for (int i=0; i<FBWorkingArea::TYPES.size(); i++)
    {
        toolbsScreenType.append(addTopMenuButton(wView,
               FBWorkingArea::TYPES[i].second, FBWorkingArea::TYPES[i].first,
               FBWorkingArea::TYPES[i].first + tr(" screen type")));
        QObject::connect(toolbsScreenType.last(),SIGNAL(clicked()),
                         this,SLOT(onScreenTypePick()));
    }
    addTopMenuSplitter(wView);
    comboScreenRatio = addTopMenuCombo(wView,tr("Aspect ratio"),
                                       FBWorkingArea::RATIOS);
    QObject::connect(comboScreenRatio,SIGNAL(currentIndexChanged(int)),
                     this,SLOT(onScreenRatioSelect(int)));
    comboScreenResol = addTopMenuCombo(wView,tr("Resolution"),
                                       FBWorkingArea::RESOLS);
    QObject::connect(comboScreenResol,SIGNAL(currentIndexChanged(int)),
                     this,SLOT(onScreenResolSelect(int)));

    // Tools.
    toolbUndo = addTopMenuButton(wTools,":/img/undo.png",
                    tr("Undo"),tr("Cancel last form \noperation"),false);
    toolbRedo = addTopMenuButton(wTools,":/img/redo.png",
                    tr("Redo"),tr("Return last canceld \nform operation"),false);
    addTopMenuSplitter(wTools);
    toolbClearScreen = addTopMenuButton(wTools,":/img/clear_screen.png",
                      tr("Clear screen"),tr("Clear all screen \nelements"),false);
    toolbDeleteElem = addTopMenuButton(wTools,":/img/delete_elem.png",
                      tr("Delete element"),tr("Delete selected \nelement"),false);
    addTopMenuSplitter(wTools);
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

    butArrowLeft = new QPushButton(wMenuLeft);
    butArrowLeft->setIcon(QIcon(":/img/arrow_left.png"));
    butArrowLeft->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Minimum);
    //butArrowLeft->setAlignment(Qt::AlignTop);
    butArrowLeft->setFlat(true);
    butArrowLeft->setCursor(Qt::PointingHandCursor);
    QObject::connect(butArrowLeft,SIGNAL(clicked()),
                     this,SLOT(onLeftArrowClick()));

    treeLeftFull = new QTreeWidget(wMenuLeft);
    treeLeftFull->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);
    treeLeftFull->setFont(QFont("Candara",FB_GUI_FONTSIZE_NORMAL));
    treeLeftFull->setColumnCount(1);
    treeLeftFull->setHeaderHidden(true);
    treeLeftFull->setCursor(Qt::PointingHandCursor);
    treeLeftFull->setFocusPolicy(Qt::NoFocus); // so not to show dotted frame
    treeLeftFull->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    treeLeftShort = new QTreeWidget(wMenuLeft);
    treeLeftShort->setMaximumWidth(35);
    treeLeftShort->setMinimumWidth(35);
    treeLeftShort->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);
    treeLeftShort->setIndentation(0);
    treeLeftShort->setItemsExpandable(false);
    treeLeftShort->setColumnCount(1);
    treeLeftShort->setHeaderHidden(true);
    treeLeftShort->setCursor(Qt::PointingHandCursor);
    treeLeftShort->setFocusPolicy(Qt::NoFocus);
    treeLeftShort->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QHBoxLayout *lhMenuLeft = new QHBoxLayout(wMenuLeft);
    QVBoxLayout *lvMenuLeft1 = new QVBoxLayout();
    QVBoxLayout *lvMenuLeft2 = new QVBoxLayout();
    lvMenuLeft1->addWidget(treeLeftFull);
    lvMenuLeft1->addWidget(treeLeftShort);
    lvMenuLeft2->addWidget(butArrowLeft);
    lvMenuLeft2->addStretch();
    lhMenuLeft->addLayout(lvMenuLeft1);
    lhMenuLeft->addLayout(lvMenuLeft2);

    //registerAllElements();
    //for ()
    //{
        
    //}

    this->flipLeftMenu(true); // just open menu
    
    //----------------------------------------------------------------------
    //                              Right menu
    //----------------------------------------------------------------------
    // see left menu for comments

    wMenuRight = new QWidget(this);
    wMenuRight->setObjectName("wMenuRight");
    wMenuRight->setMaximumWidth(300);
    wMenuRight->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);
    //wMenuRight->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    butArrowRight = new QPushButton(wMenuRight);
    butArrowRight->setIcon(QIcon(":/img/arrow_right.png"));
    butArrowRight->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Minimum);
    butArrowRight->setFlat(true);
    butArrowRight->setCursor(Qt::PointingHandCursor);
    QObject::connect(butArrowRight,SIGNAL(clicked()),
            this,SLOT(onRightArrowClick()));

    labRight = new QLabel(wMenuRight);
    labRight->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    labRight->setAlignment(Qt::AlignCenter);
    labRight->setFont(QFont("Candara",FB_GUI_FONTSIZE_NORMAL));

    this->setRightMenuCaption(false);

    QHBoxLayout *lhMenuRight = new QHBoxLayout(wMenuRight);
    QVBoxLayout *lvRight1 = new QVBoxLayout();
    vlRight = new QVBoxLayout();
    lvRight1->addWidget(butArrowRight);
    lvRight1->addStretch();
    lhMenuRight->addLayout(lvRight1);
    vlRight->addWidget(labRight);
    vlRight->addStretch();
    lhMenuRight->addLayout(vlRight);

    this->flipRightMenu(true); // just open menu
    
    //----------------------------------------------------------------------
    //                              Working area
    //----------------------------------------------------------------------
    
    wWorkingArea = new FBWorkingArea(this);
    // Set default for app screen's style.
    this->onScreenStylePick(); // sets style to first occurs and rewrite
    this->onScreenTypePick();  // FBWorkingArea defaults
    
    //----------------------------------------------------------------------
    //                              Other gui
    //----------------------------------------------------------------------
    
    labBottom = new QLabel(this);
    labBottom->setText(" ...");
    labBottom->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labBottom->setFont(QFont("Candara",FB_GUI_FONTSIZE_NORMAL));
    labBottom->setText(tr("  Create new or open existing project ..."));

    dlgProgress = new FBDialogProgress(this);

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

    butArrowLeft->setStyleSheet("QPushButton:default{border:none;}"
                             "QPushButton:flat{border:none;}"
                             "QPushButton:checked{border:none;}");

    treeLeftFull->setStyleSheet("QTreeWidget"
                              "{"
                                  "border: none;"
                                  "icon-size: 20px;"
                              "}"
                              "QTreeView::branch:has-children:opened"
                              "{"
                                   "image: url(:/img/arrow_open.png);"
                              "}"
                              "QTreeView::branch:has-children:closed"
                              "{"
                                   "image: url(:/img/arrow_close.png);"
                              "}"
                              "QTreeView::branch:!has-children:opened"
                              "{"
                                   "image: none;"
                              "}"
                              "QTreeView::branch:!has-children:closed"
                              "{"
                                   "image: none;"
                              "}"
                              "QTreeWidget::item"
                              "{"
                                  "border: none;"
                                  "padding-top: 5;"
                                  "padding-bottom: 5;"
                              "}"
                              "QTreeWidget::item:has-children"
                              "{"
                                  //"text-decoration: underline;"
                                  "color: "+QString(FB_COLOR_LIGHTGREY)+";"
                              "}"
                              "QTreeWidget::item:!has-children"
                              "{"
                                  "color: black;"
                              "}"
                              "QTreeWidget::item:has-children:hover"
                              "{"
                                  "background-color: white;"
                              "}"
                              "QTreeWidget::item:!has-children:hover"
                              "{"
                                  "background-color: "+FB_COLOR_DARKBLUE+";"
                              "}"
                              "QTreeWidget::item:has-children:selected"
                              "{"
                                  "background-color: white;"
                                  "color: "+FB_COLOR_LIGHTGREY+";"
                              "}"
                              "QTreeWidget::item:!has-children:selected"
                              "{"
                                  "background-color: "+FB_COLOR_LIGHTBLUE+";"
                                  "color: black;"
                              "}");

    treeLeftShort->setStyleSheet(treeLeftFull->styleSheet());

    wMenuRight->setStyleSheet("QWidget#"+wMenuRight->objectName()+"{"
                              "border-top: 1px solid "+FB_COLOR_MEDIUMGREY+";"
                              "border-bottom: 1px solid "+FB_COLOR_MEDIUMGREY+";"
                              "border-left: 1px solid "+FB_COLOR_MEDIUMGREY+";"
                              "border-top-left-radius: 4px;"
                              "border-bottom-left-radius: 4px; }");

    butArrowRight->setStyleSheet("QPushButton:default{border:none;}"
                             "QPushButton:flat{border:none;}"
                             "QPushButton:checked{border:none;}");

    labBottom->setStyleSheet("QLabel {color: "+QString(FB_COLOR_DARKGREY)+"}");

    //----------------------------------------------------------------------
    //                      Other general gui settings
    //----------------------------------------------------------------------

    this->updateEnableness();
}


/****************************************************************************/
/*                         Private main FB slots                            */
/****************************************************************************/


void FB::onElemPress ()
{

}


void FB::onNewVoidClick ()
{
    toolbNewVoid->setDown(true);
    if (askToLeaveUnsafeProject())
    {
        toolbNewVoid->setDown(false);
        return;
    }



    toolbNewVoid->setDown(false);
}


void FB::onNewShapeClick ()
{

}


void FB::onNewNgwClick ()
{

}


void FB::onOpenClick ()
{
    toolbOpen->setDown(true);
    if (askToLeaveUnsafeProject())
    {
        toolbOpen->setDown(false);
        return;
    }

    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::List);
    dialog.setDefaultSuffix(FB_PROJECT_EXTENSION);
    dialog.setNameFilter("*."+QString(FB_PROJECT_EXTENSION));
    dialog.setWindowTitle(tr("Open project ..."));
    QString lastPath = this->getSettingLastPath();
    if (lastPath == "")
    {
        dialog.setDirectory(QDir());
    }
    else
    {
        QFileInfo fileInfo(lastPath);
        dialog.setDirectory(fileInfo.absoluteDir());
    }
    QObject::connect(&dialog, SIGNAL(finished(int)), // to do common gui actions after
                     this, SLOT(onProjDialogFinished(int)));

    // Show dialog.
    if (dialog.exec())
    {
        QString strFullPath = dialog.selectedFiles().first();
        if (!strFullPath.endsWith("." + QString(FB_PROJECT_EXTENSION),
                                  Qt::CaseInsensitive))
        {
            this->showError(tr("Wrong file extension. Must be .")
                          + QString(FB_PROJECT_EXTENSION));
            return;
        }

        // Create new project.
        FBProject *projNew = new FBProject();
//        QObject::connect(projNew, SIGNAL(changeProgress(int)),
//                         dlgProgress, SLOT(onChangeProgress(int)));
        FBErr err = projNew->open(strFullPath);
        if (err != FBErrNone)
        {
            delete projNew;
            this->showErrorFull(tr("Unable to open project!"), err);
            return;
        }

        // Replace new project, deleting old one.
        delete project;
        project = projNew;

        // Delete old form.


        // Create and show new form.

        // Fill form with elems from project.

        // Common actions.
        this->updateSettings();
        this->updateMenus();
        this->updateEnableness();
        this->setBottomString(".............................");
    }
}


void FB::onSaveClick ()
{

}


void FB::onSaveAsClick ()
{

}


// No problems to call as simple method, while we check obj for NULL here.
void FB::onScreenStylePick ()
{
    QObject *obj = this->sender();
    if (obj == NULL)
    {
        // We do not need to have default screen style, for example
        // at app construction time - we must render grey screen.
        // So there is no change style call here.
        return;
    }
    int index = 0;
    for (int i=0; i<toolbsScreenStyle.size(); i++)
    {
        // Find the buuton which is pressed now.
        if (toolbsScreenStyle[i] == static_cast<QToolButton*>(obj))
            index = i;
        else
            toolbsScreenStyle[i]->setDown(false);
    }
    static_cast<QToolButton*>(obj)->setDown(true);
    wWorkingArea->changeStyle(toolbsScreenStyle[index]->text());
}


// No problems to call as simple method, while we check obj for NULL here.
void FB::onScreenTypePick ()
{
    QObject *obj = this->sender();
    if (obj == NULL)
    {
        // We must always have the default screen type - so here we set
        // the first occur type in the array, which must be "maximized".
        toolbsScreenType.first()->setDown(true);
        wWorkingArea->changeStyle(toolbsScreenType.first()->text());
        return;
    }
    int index = 0;
    for (int i=0; i<toolbsScreenType.size(); i++)
    {
        // Find the buuton which is pressed now.
        if (toolbsScreenType[i] == static_cast<QToolButton*>(obj))
            index = i;
        else
            toolbsScreenType[i]->setDown(false);
    }
    static_cast<QToolButton*>(obj)->setDown(true);
    wWorkingArea->changeType(toolbsScreenType[index]->text());
}

void FB::onScreenRatioSelect (int index)
{
    wWorkingArea->changeRatio(index); // index fully corresponds to array of ratios
}

void FB::onScreenResolSelect (int index)
{
    wWorkingArea->changeResol(index); // index fully corresponds to array of resols
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
    this->flipLeftMenu(!treeLeftFull->isVisible());
}

void FB::onRightArrowClick ()
{
    this->flipRightMenu(!labRight->isVisible());
}


// Show messages.
int FB::showBox (QString msg, QString caption)
{
    QMessageBox msgBox(this);
    msgBox.setStyleSheet("");
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

int FB::showErrorFull (QString msgMain, FBErr err)
{
    QMessageBox msgBox(this);
    msgBox.setStyleSheet("");
    msgBox.setWindowTitle(tr("Error"));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText(msgMain);
    msgBox.setInformativeText(this->getErrString(err));
    msgBox.setDetailedText(FBProject::CUR_ERR_INFO);
    FBProject::CUR_ERR_INFO = "";
    return msgBox.exec();
}


bool FB::askToLeaveUnsafeProject ()
{
    return (project != NULL
            && project->isSaveRequired()
            && this->showWarning(tr("Project hasn't been saved. Continue?"))
                != QMessageBox::Ok);
}


// Some GUI actions after closing project dialogs.
void FB::onProjDialogFinished (int code)
{
    toolbNewVoid->setDown(false);
    toolbNewShape->setDown(false);
    toolbNewNgw->setDown(false);
    toolbOpen->setDown(false);
    toolbSave->setDown(false);
    toolbSaveAs->setDown(false);
}


/****************************************************************************/
/*                        Private FB methods                                */
/****************************************************************************/


// Create new button for any tab of the top menu.
QToolButton *FB::addTopMenuButton (QWidget *parentTab, QString imgPath,
                                   QString name, QString description, bool small)
{
    QToolButton *but = new QToolButton(parentTab);
    QHBoxLayout *hlParent = (QHBoxLayout*)parentTab->layout();
    but->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    but->setAutoRaise(true);
    but->setIcon(QIcon(imgPath));
    but->setText(name); // necessarily do this because it will store correspondance
                        // to screen arrays for screen menu buttons
    but->setFont(QFont("Candara",FB_GUI_FONTSIZE_SMALL));
    if (small)
    {
        but->setIconSize(QSize(60,60));
        but->setMaximumWidth(45);
    }
    else
    {
        but->setIconSize(QSize(60,60));
        but->setMaximumWidth(90);
    }
    //but->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    but->setCursor(Qt::PointingHandCursor);
    hlParent->insertWidget(hlParent->count()-1,but); // last is stretch item.
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
                           "background-color: "+FB_COLOR_LIGHTBLUE+";"
                       "}"
                       "QToolButton:disabled"
                       "{"
                            // TODO: Make real semitransparent style (~20%).
                       "}");
    but->setToolTip(description);
    return but;
}

QComboBox *FB::addTopMenuCombo (QWidget *parentTab, QString caption,
                                QStringList values)
{
    QComboBox *combo = new QComboBox(parentTab);
    combo->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    combo->setFont(QFont("Segoe UI",FB_GUI_FONTSIZE_NORMAL));
    for (int i=0; i<values.size(); i++)
    {
        combo->addItem(values[i]);
    }

    QLabel *lab = new QLabel(parentTab);
    lab->setText(caption);
    lab->setAlignment(Qt::AlignCenter);
    lab->setFont(QFont("Candara",FB_GUI_FONTSIZE_SMALL));
    lab->setStyleSheet("QLabel {color: "+QString(FB_COLOR_MEDIUMGREY)+"}");
    lab->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);

    QHBoxLayout *hlParent = (QHBoxLayout*)parentTab->layout();
    QVBoxLayout *vl = new QVBoxLayout();
    vl->setContentsMargins(0,0,0,0);
    vl->setSpacing(10);
    vl->addWidget(combo);
    vl->addWidget(lab);
    hlParent->insertLayout(hlParent->count()-1,vl);

    return combo;
}


void FB::addTopMenuSplitter (QWidget *parentTab)
{
    QWidget *wid = new QWidget(wView);
    wid->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    wid->setFixedWidth(1);
    wid->setStyleSheet("QWidget {background-color: "
                             +QString(FB_COLOR_LIGHTMEDIUMGREY)+"}");
    QHBoxLayout *hlParent = (QHBoxLayout*)parentTab->layout();
    hlParent->insertWidget(hlParent->count()-1,wid); // last is stretch item.
}


void FB::flipLeftMenu (bool isFull)
{
    treeLeftFull->setVisible(isFull);
    treeLeftShort->setVisible(!isFull);
    if (!isFull)
        butArrowLeft->setIcon(QIcon(":/img/arrow_right.png"));
    else
        butArrowLeft->setIcon(QIcon(":/img/arrow_left.png"));

    // TODO: open and select elems in the one tree, that was selected in another.
}

void FB::flipRightMenu (bool isFull)
{
    labRight->setVisible(isFull);
    for (int i=0; i<tablesRight.size(); i++)
    {
        tablesRight[i]->setVisible(isFull);
    }
    if (!isFull)
        butArrowRight->setIcon(QIcon(":/img/arrow_left.png"));
    else
        butArrowRight->setIcon(QIcon(":/img/arrow_right.png"));
}


QTableWidget* FB::addRightMenuTable (int rowCount)
{
    QTableWidget *table = new QTableWidget(wMenuRight);
    table->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);
    table->setFocusPolicy(Qt::NoFocus);
    table->setFont(QFont("Candara",FB_GUI_FONTSIZE_NORMAL));
    table->verticalHeader()->hide();
    table->horizontalHeader()->hide();
    table->setRowCount(rowCount);
    table->setColumnCount(2);
    table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    table->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    table->setStyleSheet("QTableWidget"
                         "{"
                             "border: none;"
                             "selection-background-color: "
                         +QString(FB_COLOR_LIGHTBLUE)+";"
                             "gridline-color: white;"
                             "color: "+FB_COLOR_LIGHTGREY+";"
                         "}"
                         "QTableWidget::item"
                         "{"
                    //"background-color:"
                    //"qlineargradient(x1: 0, y1: 0.5, x2: 1, y2: 0.5,"
                    //"stop: 0 rgb(240,240,240), stop: 1 rgb(255,255,255));"
                      "border: 1px solid "+FB_COLOR_LIGHTMEDIUMGREY+";"
                         "}"
                         "QTableWidget::item:selected"
                         "{"
                             "border: 1px solid white;"
                           "background-color: "+FB_COLOR_LIGHTBLUE+";"
                          "}"
                          "QTableWidget::item:focus"
                          "{"
                                //"border: 2px solid red;"
                           "}");

    vlRight->insertWidget(vlRight->count()-1,table); // before stretch item
    tablesRight.append(table);
    return table;
}


void FB::setRightMenuCaption (bool isElemSelected)
{
    /*
    if (isElemSelected)
    {
        labRight->setStyleSheet("QLabel{border: none;color: "
                                +QString(FB_COLOR_DARKBLUE)+";}");
        FBElem *elemSelected = wWorkingArea->getForm()->getElemSelectedPtr();
        if (elemSelected != NULL)
            labRight->setText(elemSelected->getDisplayName());
        else
            labRight->setText("...");
    }
    else
    {*/
        labRight->setStyleSheet("QLabel{border: none;color: "
                                +QString(FB_COLOR_LIGHTBLUE)+";}");
        labRight->setText(tr("Select an element ..."));
    /*}*/

}


void FB::updateEnableness ()
{
    if (project == NULL)
    {
        toolbSave->setEnabled(false);
        toolbSaveAs->setEnabled(false);
        toolbClearScreen->setEnabled(false);
        toolbDeleteElem->setEnabled(false);
    }
    else
    {
        toolbSaveAs->setEnabled(true);
        toolbClearScreen->setEnabled(true);
        if (project->wasFirstSaved())
        {
            toolbSave->setEnabled(true);
        }
    }
}


void FB::updateMenus ()
{

}


void FB::setBottomString (QString str)
{
    labBottom->setText(" " + str);
}


void FB::updateSettings ()
{

}


QString FB::getSettingLastPath ()
{
    return "";
}


QString FB::getErrString (FBErr err)
{
    return "...";
}


/****************************************************************************/
/*                           New project dialog                             */
/****************************************************************************/

FBDialogProjectNew::~FBDialogProjectNew ()
{

}

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


/****************************************************************************/
/*                            Progress dialog                               */
/****************************************************************************/

FBDialogProgress::~FBDialogProgress()
{

}

FBDialogProgress::FBDialogProgress (QWidget *parent): QDialog(parent)
{

}











