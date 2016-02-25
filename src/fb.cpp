/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  main app+gui class
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

#include "elements.h"


FB::~FB()
{
    delete ui;
}

FB::FB(QWidget *parent): QWidget(parent), ui(new Ui::FB)
{
    isInited = false;
    project = NULL;

    ui->setupUi(this);
}

void FB::initGui ()
{
    if (isInited)
        return;
    isInited = true;

    //----------------------------------------------------------------------
    //                              Working area
    //----------------------------------------------------------------------

    wScreen = new FBScreen(this); // currently with no form
    wScreen->updateStyle();
    wScreen->setDevice(0);
    wScreen->setState(0); // default maximized grey area

    //----------------------------------------------------------------------
    //                              Top menu
    //----------------------------------------------------------------------

    tabMenuTop = new QTabWidget(this);
    tabMenuTop->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    tabMenuTop->setMaximumHeight(120);
    tabMenuTop->setMinimumHeight(100);
    tabMenuTop->setFont(QFont(FB_GUI_FONTTYPE,FB_GUI_FONTSIZE_NORMAL));

    wProject = new QWidget();
    tabMenuTop->addTab(wProject, tr(" Project "));
    QHBoxLayout *layProject = new QHBoxLayout(wProject);
    layProject->setContentsMargins(0,0,0,0);
    layProject->addStretch();

    wView = new QWidget();
    tabMenuTop->addTab(wView, tr("  View  "));
    lhView = new QHBoxLayout(wView);
    lhView->setContentsMargins(4,4,4,4);
    lhView->addStretch();

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
    toolbNewVoid = this->addTopMenuButton(wProject,":/img/new_void.png",
                        tr("New"),tr("New void project"));
    QObject::connect(toolbNewVoid, SIGNAL(clicked()),
                     this, SLOT(onNewVoidClick()));
    toolbNewShape = this->addTopMenuButton(wProject,":/img/new_shp.png",
            tr("New from Shapefile"),tr("New project from \nShapefile"));
    QObject::connect(toolbNewShape, SIGNAL(clicked()),
                     this, SLOT(onNewShapeClick()));
    toolbNewNgw = this->addTopMenuButton(wProject,":/img/new_ngw.png",
      tr("New from NextGIS Web"),tr("New project from \nNextGIS Web connection"));
    QObject::connect(toolbNewNgw, SIGNAL(clicked()),
                     this, SLOT(onNewNgwClick()));
    toolbOpen = this->addTopMenuButton(wProject,":/img/open.png",
                         tr("Open"),tr("Open .ngfp file"));
    QObject::connect(toolbOpen, SIGNAL(clicked()),
                     this, SLOT(onOpenClick()));
    toolbSave = this->addTopMenuButton(wProject,":/img/save.png",
                          tr("Save"),tr("Save to .ngfp file"));
    QObject::connect(toolbSave, SIGNAL(clicked()),
                     this, SLOT(onSaveClick()));
    toolbSaveAs = this->addTopMenuButton(wProject,":/img/save_as.png",
                    tr("Save as"),tr("Save to .ngfp file \nas ..."));
    QObject::connect(toolbSaveAs, SIGNAL(clicked()),
                     this, SLOT(onSaveAsClick()));

    // All view pickers and combos.
    toolbScreenAndroid = this->addTopMenuButton(wView,
               ":/img/android.png", tr("Android"), tr("Android screen"),false);
    QObject::connect(toolbScreenAndroid,SIGNAL(clicked()),
                         this,SLOT(onScreenAndroidPick()));
    toolbScreenIos = this->addTopMenuButton(wView,
               ":/img/ios.png", tr("iOS"), tr("iOS screen"),false);
    QObject::connect(toolbScreenIos,SIGNAL(clicked()),
                         this,SLOT(onScreenIosPick()));
    toolbScreenWeb = this->addTopMenuButton(wView,
               ":/img/web.png", tr("Web"), tr("Web screen"),false);
    QObject::connect(toolbScreenWeb,SIGNAL(clicked()),
                         this,SLOT(onScreenWebPick()));
    toolbScreenQgis = this->addTopMenuButton(wView,
               ":/img/qgis.png", tr("QGIS"), tr("QuantumGIS screen"),false);
    QObject::connect(toolbScreenQgis,SIGNAL(clicked()),
                         this,SLOT(onScreenQgisPick()));
    this->addTopMenuSplitter(wView);
    QStringList stubList;
    comboScreenDevice = this->addTopMenuCombo(wView, tr("Device\nsettings"),
                                              stubList);
    QObject::connect(comboScreenDevice, SIGNAL(activated(int)),
                     this, SLOT(onScreenDeviceSelect(int)));
    // TODO: add widget with device info here ...
    this->addTopMenuSplitter(wView);
    this->updateMenuView(); // just for first appearance

    // Tools.
    toolbUndo = this->addTopMenuButton(wTools,":/img/undo.png",
                    tr("Undo"),tr("Cancel last form \noperation"),false);
    toolbRedo = this->addTopMenuButton(wTools,":/img/redo.png",
                    tr("Redo"),tr("Return last canceld \nform operation"),false);
    this->addTopMenuSplitter(wTools);
    toolbClearScreen = this->addTopMenuButton(wTools,":/img/clear_screen.png",
                    tr("Clear screen"),tr("Clear all screen \nelements"),false);
    QObject::connect(toolbClearScreen, SIGNAL(clicked()),
                     this, SLOT(onClearScreenClick()));
    toolbDeleteElem = this->addTopMenuButton(wTools,":/img/delete_elem.png",
                    tr("Delete element"),tr("Delete selected \nelement"),false);
    QObject::connect(toolbDeleteElem, SIGNAL(clicked()),
                     this, SLOT(onDeleteElemClick()));
    this->addTopMenuSplitter(wTools);
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
    treeLeftFull->setFont(QFont(FB_GUI_FONTTYPE,FB_GUI_FONTSIZE_NORMAL));
    treeLeftFull->setColumnCount(1);
    treeLeftFull->setHeaderHidden(true);
    treeLeftFull->setCursor(Qt::PointingHandCursor);
    treeLeftFull->setFocusPolicy(Qt::NoFocus); // so not to show dotted frame
    treeLeftFull->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QObject::connect(treeLeftFull,SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this,SLOT(onAddElemPress(QTreeWidgetItem*,int)));

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
    QObject::connect(treeLeftShort,SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this,SLOT(onAddElemPress(QTreeWidgetItem*,int)));

    QHBoxLayout *lhMenuLeft = new QHBoxLayout(wMenuLeft);
    QVBoxLayout *lvMenuLeft1 = new QVBoxLayout();
    QVBoxLayout *lvMenuLeft2 = new QVBoxLayout();
    lvMenuLeft1->addWidget(treeLeftFull);
    lvMenuLeft1->addWidget(treeLeftShort);
    lvMenuLeft2->addWidget(butArrowLeft);
    lvMenuLeft2->addStretch();
    lhMenuLeft->addLayout(lvMenuLeft1);
    lhMenuLeft->addLayout(lvMenuLeft2);

    this->updateLeftTrees(); // fill trees with elements' representations
    this->flipLeftMenu(false);

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
    labRight->setFont(QFont(FB_GUI_FONTTYPE,FB_GUI_FONTSIZE_NORMAL));

    QHBoxLayout *lhMenuRight = new QHBoxLayout(wMenuRight);
    QVBoxLayout *lvRight1 = new QVBoxLayout();
    lvRight = new QVBoxLayout();
    //lvRight->setContentsMargins(5,5,5,5);
    lvRight->setSpacing(12);
    lvRight1->addWidget(butArrowRight);
    lvRight1->addStretch();
    lhMenuRight->addLayout(lvRight1);
    lvRight->addWidget(labRight);
    lvRight->addStretch();
    lhMenuRight->addLayout(lvRight);

    this->updateRightMenu(); // initial menu settings
    this->flipRightMenu(false);

    //----------------------------------------------------------------------
    //                              Other GUI
    //----------------------------------------------------------------------

    labBottom = new QLabel(this);
    labBottom->setText(" ...");
    labBottom->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labBottom->setFont(QFont(FB_GUI_FONTTYPE,FB_GUI_FONTSIZE_NORMAL));
    this->updateProjectString();

    dlgProgress = new FBDialogProgress(this);

    //----------------------------------------------------------------------
    //                            Layout all window
    //----------------------------------------------------------------------

    lhMiddle = new QHBoxLayout();
    lhMiddle->addWidget(wMenuLeft);
    lhMiddle->addWidget(wScreen);
    lhMiddle->addWidget(wMenuRight);

    lvAll = new QVBoxLayout(this);
    lvAll->setContentsMargins(0,5,0,5);
    lvAll->setSpacing(10);
    lvAll->addWidget(tabMenuTop);
    lvAll->addLayout(lhMiddle);
    lvAll->addWidget(labBottom);

    //----------------------------------------------------------------------
    //                         Common GUI settings
    //----------------------------------------------------------------------

    this->updateEnableness();
}


// Sets the general "white" style for all GUI. If not to call this - all fonts and
// colors will be system-default, except several buttons and menus.
void FB::setFbStyle ()
{
    // From docs:
    // " ... style authors are restricted by the different platforms' guidelines
    // and (on Windows XP and OS X) by the native theme engine."
    // Note: regardless of the next statement from docs the whole app color
    // setting works anyway:
    // "When using Qt Style Sheets, a widget does not automatically inherit
    // its font and color setting from its parent widget".

    this->setStyleSheet("QWidget { background: white;"
                        " color: black; }"); // explicitly set dark font color!

    tabMenuTop->setStyleSheet("QTabWidget::pane {"
                              "border-top: 1px solid "
                              +QString(FB_COLOR_MEDIUMGREY)+";"
                              "border-bottom: 1px solid "+FB_COLOR_MEDIUMGREY+";}"
                              "QTabWidget::tab-bar {"
                              "left: 5px; "
                              "top: 1px; }"
                              "QTabBar::tab {"
                              "border: 1px solid "+FB_COLOR_MEDIUMGREY+";"
                              "border-bottom-color: white; "
                              "border-top-left-radius: 4px;"
                              "border-top-right-radius: 4px;"
                              "min-width: 8ex;"
                              "padding: 2px;"
                              "color: black }"
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
                                  "color: "+QString(FB_COLOR_DARKGREY)+";"
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
                                  "color: "+FB_COLOR_DARKGREY+";"
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
}


/****************************************************************************/
/*                                                                          */
/*                         Private main FB slots                            */
/*                                                                          */
/****************************************************************************/

void FB::onAddElemPress (QTreeWidgetItem* item, int column)
{
    FBForm *form = wScreen->getFormPtr();
    if (form == NULL)
        return;

    if (item->childCount() != 0) // check if it is not a group header in tree
        return;

    QString keyName = item->data(0,Qt::UserRole).toString();
    FBFactory *fct = FBFactory::getFctByName(keyName);
    if (fct == NULL)
        return;

    FBElem *elem = fct->create();
    form->addElem(elem,NULL);

    this->updateElemForApp(elem);

    this->onElemSelect();
}


void FB::onNewVoidClick ()
{
    toolbNewVoid->setDown(true);
    if (this->askToLeaveUnsafeProject())
    {
        toolbNewVoid->setDown(false);
        return;
    }

    FBDialogProjectNew dialog(this);
    QObject::connect(&dialog,SIGNAL(finished(int)),
                  this,SLOT(onProjDialogFinished(int)));

    if (dialog.exec())
    {
        QString geomType = dialog.getSelectedGeom();

        FBProjectVoid *projVoid = new FBProjectVoid(FBProject::GEOM_TYPES[geomType]);
        this->newProjectCommonActions(projVoid, ""); // just void string stub
    }
}


void FB::onNewShapeClick ()
{
    toolbNewShape->setDown(true);
    if (this->askToLeaveUnsafeProject())
    {
        toolbNewShape->setDown(false);
        return;
    }

    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::List);
    dialog.setDefaultSuffix("shp");
    dialog.setNameFilter("*.shp");
    dialog.setWindowTitle(tr("Creating new project. Select Shapefile ..."));
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
    QObject::connect(&dialog, SIGNAL(finished(int)),
                     this, SLOT(onProjDialogFinished(int)));

    if (dialog.exec())
    {
        QString pathShapefile;
        QStringList sPaths = dialog.selectedFiles();
        pathShapefile = sPaths[0];

        FBProjectShapefile *projShp = new FBProjectShapefile();
        this->newProjectCommonActions(projShp, pathShapefile);
    }
}


void FB::onNewNgwClick ()
{
    toolbNewNgw->setDown(true);
    if (this->askToLeaveUnsafeProject())
    {
        toolbNewNgw->setDown(false);
        return;
    }

    FBDialogProjectNgw dialog(this,"","");
    QObject::connect(&dialog, SIGNAL(finished(int)),
                     this, SLOT(onProjDialogFinished(int)));

    if (dialog.exec())
    {
        QString pathNgwUrl;
        QString strUrl, strLogin, strPass;
        int nId;
        Json::Value jsonLayerMeta;
        pathNgwUrl = dialog.getSelectedNgwResource(
                    strUrl, strLogin, strPass, nId, jsonLayerMeta);

        FBProjectNgw *projNgw = new FBProjectNgw(
                    strUrl, strLogin, strPass, nId, jsonLayerMeta);
        this->newProjectCommonActions(projNgw, pathNgwUrl);
    }
}


void FB::onOpenClick ()
{
    toolbOpen->setDown(true);
    if (this->askToLeaveUnsafeProject())
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
    QObject::connect(&dialog, SIGNAL(finished(int)),
                     this, SLOT(onProjDialogFinished(int)));

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

        // Create project replacing old one.
        FBProject *projOpen = new FBProject();
        FBErr err = projOpen->open(strFullPath);
        if (err != FBErrNone)
        {
            delete projOpen;
            this->showErrorFull(tr("Unable to open project!"), err);
            return;
        }
        if (project != NULL)
            delete project;
        project = projOpen;

        // Create and show new form, replacing old one. Fill it with elems from
        // project.
        wScreen->deleteForm();
        FBForm *form = this->createForm();
        form->fromJson(FBProject::readForm(strFullPath));
        wScreen->setForm(form);

        // Update elems for app.
        QMap<int,FBElem*> map = form->getElems();
        QMap<int,FBElem*>::const_iterator it = map.constBegin();
        while (it != map.constEnd())
        {
            this->updateElemForApp(it.value());
            ++it;
        }

        this->pickDefaultScreen(); // will be helpful if there is void screen now
        this->updateRightMenu();
        this->updateEnableness();
        this->updateProjectString();
        this->updateSettings();
    }
}


void FB::onSaveClick ()
{

}


void FB::onSaveAsClick ()
{

}


void FB::onScreenAndroidPick ()
{
    FBScreenAndroid *screen = new FBScreenAndroid(this);
    this->recreateScreen(screen,false);
    this->afterPickScreen(toolbScreenAndroid);
}
void FB::onScreenIosPick ()
{
    FBScreenIos *screen = new FBScreenIos(this);
    this->recreateScreen(screen,false);
    this->afterPickScreen(toolbScreenIos);
}
void FB::onScreenWebPick ()
{
    FBScreen *screen = new FBScreen(this);
    this->recreateScreen(screen,false);
    this->afterPickScreen(toolbScreenWeb);
}
void FB::onScreenQgisPick ()
{
    FBScreen *screen = new FBScreen(this);
    this->recreateScreen(screen,false);
    this->afterPickScreen(toolbScreenQgis);
}


void FB::onScreenStatePick ()
{
    // We must always have the default screen type - so here we set
    // the first occur state in the array.
    QObject *obj = this->sender();
    if (obj == NULL)
    {
        return;
    }
    for (int i=0; i<toolbsScreenState.size(); i++)
    {
        toolbsScreenState[i]->setDown(false);
    }
    static_cast<QToolButton*>(obj)->setDown(true);
    this->updateScreen();
}


void FB::onScreenDeviceSelect (int index)
{
    this->updateScreen();
}


void FB::onUndoClick ()
{

}

void FB::onRedoClick ()
{

}

void FB::onClearScreenClick ()
{
    toolbClearScreen->setDown(true);
    FBForm *form = wScreen->getFormPtr();
    if (form == NULL || form->isVoid())
        return;
    if (this->showWarning(tr("Clear screen from all elements?"))
            != QMessageBox::Ok)
        return;
    form->clear();
    this->updateRightMenu();
    toolbClearScreen->setDown(false);
}

void FB::onDeleteElemClick ()
{
    toolbDeleteElem->setDown(true);
    FBForm *form = wScreen->getFormPtr();
    if (form == NULL || form->isVoid())
        return;
    form->deleteSelected();
    this->updateRightMenu();
    toolbDeleteElem->setDown(false);
}

void FB::onSettingLanguageSelect ()
{

}

void FB::onAboutGraphicsClick ()
{

}

void FB::onElemSelect ()
{
    this->updateRightMenu();
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
    msgBox.setStyleSheet(""); // TODO: why does not work?
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


// Some common GUI actions after closing project dialogs.
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
/*                                                                          */
/*                        Private FB methods                                */
/*                                                                          */
/****************************************************************************/

// Common actions for element types, which can be made only in main app class.
// Update elem with the app's data, because only this class aggregates project,
// form and screen modules.
void FB::updateElemForApp (FBElem* elem)
{
    FBElemInput *e = qobject_cast<FBElemInput*>(elem);
    if (e != NULL)
    {
        if (project == NULL)
            return;
        e->updateFields(project->getFields().keys());
    }
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


// Common actions for form creation.
FBForm *FB::createForm ()
{
    FBForm *form = new FBForm();
    QObject::connect(form, SIGNAL(elemPressed()),
                     this, SLOT(onElemSelect()));
    return form;
}


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
    but->setFont(QFont(FB_GUI_FONTTYPE,FB_GUI_FONTSIZE_SMALL));
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
    // TODO: Make real semitransparent style (~20%).
    but->setStyleSheet("QToolButton"
                       "{border: none;}"
                       "QToolButton:hover"
                       "{background-color: "+QString(FB_COLOR_DARKBLUE)+";}"
                       "QToolButton:pressed{"
                           "background-color: "+FB_COLOR_LIGHTBLUE+";}"
                       "QToolButton:disabled{}");
    but->setToolTip(description);
    return but;
}

QComboBox *FB::addTopMenuCombo (QWidget *parentTab, QString caption,
                                QStringList values)
{
    QComboBox *combo = new QComboBox(parentTab);
    combo->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    combo->setMaximumWidth(125);
    combo->setFont(QFont("Segoe UI",FB_GUI_FONTSIZE_NORMAL));
    for (int i=0; i<values.size(); i++)
    {
        combo->addItem(values[i]);
    }

    QLabel *lab = new QLabel(parentTab);
    lab->setText(caption);
    lab->setAlignment(Qt::AlignCenter);
    lab->setFont(QFont(FB_GUI_FONTTYPE,FB_GUI_FONTSIZE_SMALL));
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

    // TODO: open and select elems in the one tree, that was selected in another,
    // i.e. synchronize trees.
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


QTableWidget* FB::addRightMenuTable ()
{
    QTableWidget *table = new QTableWidget(wMenuRight);
    table->setMinimumHeight(0);
    //table->setMaximumHeight(50);
    table->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    table->setFocusPolicy(Qt::NoFocus);
    table->setFont(QFont(FB_GUI_FONTTYPE, FB_GUI_FONTSIZE_NORMAL));
    table->verticalHeader()->hide();
    table->horizontalHeader()->hide();
    table->setRowCount(0);
    table->setColumnCount(2);
    table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setStyleSheet("QTableWidget"
                         "{border: none;"
                         "selection-background-color: "
                         +QString(FB_COLOR_LIGHTBLUE)+";"
                         "gridline-color: white;"
                         "color: "+FB_COLOR_DARKGREY+";}"
                         "QTableWidget::item"
                         "{border: 1px solid "+FB_COLOR_LIGHTMEDIUMGREY+";}"
                         "QTableWidget::item:selected"
                         "{border: 1px solid white;"
                         "background-color: "+FB_COLOR_LIGHTBLUE+";}"
                         "QTableWidget::item:focus"
                         "{"
                            //"border: 2px solid red;"
                         "}");
    lvRight->insertWidget(lvRight->count()-1,table); // before stretch item
    tablesRight.append(table);
    return table;
}


void FB::updateEnableness ()
{
    if (project == NULL)
    {
        wMenuLeft->setEnabled(false);
        wMenuRight->setEnabled(false);
        toolbSave->setEnabled(false);
        toolbSaveAs->setEnabled(false);
        toolbScreenAndroid->setEnabled(false);
        toolbScreenIos->setEnabled(false);
        toolbScreenWeb->setEnabled(false);
        toolbScreenQgis->setEnabled(false);
        comboScreenDevice->setEnabled(false);
        for (int i=0; i<toolbsScreenState.size(); i++)
        {
            toolbsScreenState[i]->setEnabled(false);
        }
        toolbUndo->setEnabled(false);
        toolbRedo->setEnabled(false);
        toolbClearScreen->setEnabled(false);
        toolbDeleteElem->setEnabled(false);
    }
    else
    {
        wMenuLeft->setEnabled(true);
        wMenuRight->setEnabled(true);
        toolbSaveAs->setEnabled(true);
        toolbClearScreen->setEnabled(true);
        toolbDeleteElem->setEnabled(true);
        toolbScreenAndroid->setEnabled(true);
        toolbScreenIos->setEnabled(true);
        toolbScreenWeb->setEnabled(true);
        toolbScreenQgis->setEnabled(true);
        comboScreenDevice->setEnabled(true);
        for (int i=0; i<toolbsScreenState.size(); i++)
        {
            toolbsScreenState[i]->setEnabled(true);
        }
        if (project->wasFirstSaved())
        {
            toolbSave->setEnabled(true);
        }
    }
}


void FB::updateLeftTrees ()
{
    treeLeftFull->clear();
    treeLeftShort->clear();

    // Create elements' representations for all trees.
    QMap<FBElemtype,QTreeWidgetItem*> groupItems;
    QList<FBFactory*> fcts = FBFactory::getAllFcts();
    for (int i=0; i<fcts.size(); i++)
    {
        if (!groupItems.contains(fcts[i]->getType()))
        {
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0,this->getGroupStr(fcts[i]->getType()));
            groupItems.insert(fcts[i]->getType(),item);
            treeLeftFull->addTopLevelItem(item);
        }

        QTreeWidgetItem *item1 = new QTreeWidgetItem();
        item1->setText(0,fcts[i]->getDisplayName());
        item1->setData(0,Qt::UserRole,fcts[i]->getKeyName());
        item1->setIcon(0,QIcon(fcts[i]->getImgPath()));
        groupItems[fcts[i]->getType()]->addChild(item1);

        QTreeWidgetItem *item2 = new QTreeWidgetItem();
        item2->setData(0,Qt::UserRole,fcts[i]->getKeyName());
        item2->setIcon(0,QIcon(fcts[i]->getImgPath()));
        item2->setToolTip(0,fcts[i]->getDisplayName());
        treeLeftShort->addTopLevelItem(item2);
    }

    treeLeftFull->sortItems(0,Qt::AscendingOrder);
}


void FB::updateRightMenu ()
{
    // Destroy all previous tables.
    for (int i=0; i<tablesRight.size(); i++)
    {
        // TODO: do we need to use QTableWidget::removeCellWidget(i,1) or
        // clearContents() or setRowCount(0) here to correctly delete tables?
        lvRight->removeWidget(tablesRight[i]);
        delete tablesRight[i];
    }
    tablesRight.clear();

    // Get form elements.
    QList<FBElem*> elemsSelected;
    FBForm *form = wScreen->getFormPtr();
    if (form != NULL)
        elemsSelected = form->getSelectedElems();

    if (elemsSelected.size() == 0)
    {
        // Show default string.
        labRight->setStyleSheet("QLabel{border: none;color: "
                                +QString(FB_COLOR_LIGHTBLUE)+";}");
        labRight->setText(tr("Select an element ..."));
    }

    else if (elemsSelected.size() == 1)
    { 
        // Create a table for one selected elem's attributes.
        // TODO: create different tables for different attribute roles.
        QTableWidget* table = this->addRightMenuTable();

        // Get attrs of selected elem.
        FBElem *elem = elemsSelected.first();
        QSet<FBAttr*> attrs = elem->getAttrs();
        QSet<FBAttr*>::const_iterator it = attrs.constBegin();
        while (it != attrs.constEnd())
        {
            QString alias = (*it)->getDisplayName();
            QTableWidgetItem *itemAlias = new QTableWidgetItem(alias);
            itemAlias->setFlags(Qt::ItemIsEnabled);
            itemAlias->setToolTip(alias);
            if ((*it)->getRole() == FBImportant)
            {
                QFont font;
                font.setBold(true);
                itemAlias->setFont(font);
            }
            QWidget *widget = (*it)->getWidget();
            table->setRowCount(table->rowCount()+1);
            table->setItem(table->rowCount()-1,0,itemAlias);
            table->setCellWidget(table->rowCount()-1,1,widget);
            ++it;
        }

        table->sortItems(0);

        // We must hide table if the right menu is in minimized state
        // for now.
        if (!labRight->isVisible())
        {
            table->hide();
        }

        // Show caption of elem.
        labRight->setStyleSheet("QLabel{border: none;color: "
                                +QString(FB_COLOR_DARKBLUE)+";}");
        labRight->setText(elem->getDisplayName());
    }

    else // size > 1
    {
        // Show string that many elems selected.
        labRight->setStyleSheet("QLabel{border: none;color: "
                                +QString(FB_COLOR_LIGHTBLUE)+";}");
        labRight->setText(tr("Elements selected: ")
                          + QString::number(elemsSelected.size()));
    }
}


void FB::setBottomString (QString strToShorten, QString strToPrepend)
{
    if (strToShorten.size() > FB_BOTTOMSTRING_LEN_MAX)
    {
        strToShorten.remove(0, strToShorten.size() - FB_BOTTOMSTRING_LEN_MAX);
        strToShorten.prepend(" ...");
    }
    labBottom->setText(" " + strToPrepend + strToShorten);
}


void FB::updateProjectString ()
{
    QString strToShorten = "";
    QString strToPrepend = "";

    if (project == NULL)
    {
        strToShorten = tr("Create new or open existing project ...");
    }
    else
    {
        strToShorten = project->getProjectfilePath();
        if (strToShorten == "")
        {
            strToPrepend = tr("Need to save data to ngfp file. Current dataset: ");
            strToShorten = project->getDatasetPath();
        }
        else
        {
            strToPrepend = tr("Current project: ");
        }
    }

    this->setBottomString(strToShorten,strToPrepend);
}


void FB::updateMenuView ()
{
    // Devices combobox.
    comboScreenDevice->clear();
    QList<FBDevice> devices = wScreen->getDevices();
    for (int i=0; i<devices.size(); i++)
    {
        comboScreenDevice->addItem(QIcon(devices[i].imgPath),
                                   devices[i].getDisplayString());
    }

    // States buttons.
    for (int i=0; i<toolbsScreenState.size(); i++)
    {
        lhView->removeWidget(toolbsScreenState[i]);
        delete toolbsScreenState[i];
    }
    toolbsScreenState.clear();
    QList<FBState> states = wScreen->getStates();
    for (int i=0; i<states.size(); i++)
    {
        toolbsScreenState.append(this->addTopMenuButton(wView,states[i].imgPath,
                                 states[i].name,states[i].descr));
        QObject::connect(toolbsScreenState.last(),SIGNAL(clicked()),
                         this,SLOT(onScreenStatePick()));
    }

    // Select defaults in GUI.
    // Do not call slots connecting to buttons and combobox.
    for (int i=0; i<toolbsScreenState.size(); i++)
    {
        toolbsScreenState[i]->setDown(false);
    }
    toolbsScreenState[0]->setDown(true);
    comboScreenDevice->setCurrentIndex(0); // will not trigger slot
}


QString FB::getGroupStr (FBElemtype type)
{
    QString str = tr("Unknown");
    switch (type)
    {
        case FBDecoration: str = tr("Decoration"); break;
        case FBInput: str = tr("Input"); break;
        case FBGrouping: str = tr("Grouping"); break;
        case FBLayout: str = tr("Layout"); break;
    }
    return str;
}


// Some common GUI actions: after change screen settings.
void FB::afterPickScreen (QToolButton *toolbDown)
{
    this->updateMenuView();
    this->updateScreen();
    toolbScreenAndroid->setDown(false);
    toolbScreenIos->setDown(false);
    toolbScreenWeb->setDown(false);
    toolbScreenQgis->setDown(false);
    if (toolbDown != NULL)
        toolbDown->setDown(true);
}


// Recreate screen to default.
void FB::pickDefaultScreen ()
{
    // TODO: think about what other screen to set in this situation - may be
    // read it from the current project structure. Currently we set Android
    // screen.
    FBScreenAndroid *screen = new FBScreenAndroid(this);
    this->recreateScreen(screen,false); // with form copy, if was some
    this->afterPickScreen(toolbScreenAndroid);
}


// Recreate screen to void. Just "reset to grey screen".
void FB::pickVoidScreen ()
{
    FBScreen *screen = new FBScreen(this);
    this->recreateScreen(screen,true); // do not copy form
    this->afterPickScreen(NULL);
}


// Create new screen with copying old form or not.
void FB::recreateScreen (FBScreen *newScreen, bool destroyForm)
{
    if (newScreen == NULL)
        return;
    if (!destroyForm)
    {
        FBForm *curForm = wScreen->takeForm();
        newScreen->setForm(curForm);
    }
    lhMiddle->removeWidget(wScreen);
    delete wScreen;
    lhMiddle->insertWidget(1,newScreen);
    newScreen->updateStyle(); // this will also update the style of form elems
    wScreen = newScreen;
}


// Update device and state settings of the screen.
void FB::updateScreen ()
{
    int index;
    index = comboScreenDevice->currentIndex();
    if (index == -1)
    {
        index = 0;
    }
    wScreen->setDevice(index);
    index = 0;
    for (int i=0; i<toolbsScreenState.size(); i++)
    {
        if (toolbsScreenState[i]->isDown())
        {
            index = i;
            break;
        }
    }
    wScreen->setState(index);
}


// Just common steps for all new projects creation.
void FB::newProjectCommonActions (FBProject *proj, QString path)
{
    // Replacing old project if new one was correctly created.
    FBErr err = proj->create(path);
    if (err != FBErrNone)
    {
        delete proj;
        this->showErrorFull(tr("Unable to create new project! "), err);
        return;
    }
    if (project != NULL)
        delete project;
    project = proj;

    // Create and show new void form replacing old one.
    wScreen->deleteForm();
    FBForm *form = this->createForm();
    wScreen->setForm(form);

    this->pickDefaultScreen(); // will be helpful if there is void screen now
    this->updateRightMenu();
    this->updateEnableness();
    this->updateProjectString();
    this->updateSettings();
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











