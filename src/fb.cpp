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

#include "form/elements.h"


FB::~FB()
{
    this->writeSettings();
    delete ui;
}

FB::FB(QWidget *parent):
    QWidget(parent), ui(new Ui::FB)
{
    isInited = false;

    FBLangInfo lang;

    lang.name = "English";
    lang.code = "en_GB";
    lang.imgFlagPath = "";
    lang.imgNextgisPath = ":/img/nextgis_en.png";
    lang.offLink = "http://nextgis.ru/en/nextgis-formbuilder";
    languages.append(lang);

    lang.name = tr("Russian");
    lang.code = "ru_RU";
    lang.imgFlagPath = "";
    lang.imgNextgisPath = ":/img/nextgis_ru.png";
    lang.offLink = "http://nextgis.ru/nextgis-formbuilder";
    languages.append(lang);

    settLastShapeFullPath.key = "last_shp";
    settLastNgfpFullPath.key = "last_ngfp";
    settLastLanguageSelected.key = "language";
     settLastLanguageSelected.defaultValue = languages[0].code;
    settLastNgwUrl.key = "last_ngw_url";
    settLastNgwLogin.key = "last_ngw_login";
    this->readSettings();

    indexLang = 0;
    for (int i=0; i<languages.size(); i++)
    {
        if (languages[i].code == settLastLanguageSelected.value)
        {
            indexLang = i;
            break;
        }
    }

    project = NULL;

    this->setObjectName("FB"); // at least for style sheets

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

    wScreen = new FBScreen(NULL); // currently with no form 
    wScreen->changeDevice(0);
    wScreen->changeState(0);
    wScreen->updateStyle();

    wWorkingArea = new QScrollArea(this);
    wWorkingArea->setWidget(wScreen);
    wWorkingArea->setStyleSheet("QScrollArea{border: 0px; "
                                "background-color: rgb(0,0,0,0)}");
    wWorkingArea->setAlignment(Qt::AlignCenter);
    wWorkingArea->setAutoFillBackground(false);
    wWorkingArea->setWidgetResizable(true); // only for default device/state

    //----------------------------------------------------------------------
    //                              Top menu
    //----------------------------------------------------------------------

    tabMenuTop = new QTabWidget(this);
    tabMenuTop->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    tabMenuTop->setMaximumHeight(120);
    tabMenuTop->setMinimumHeight(100);
    tabMenuTop->setFont(QFont(FB_GUI_FONTTYPE,FB_GUI_FONTSIZE_NORMAL));

    wProject = new QWidget();
    tabMenuTop->addTab(wProject, tr("  Project  "));
    QHBoxLayout *layProject = new QHBoxLayout(wProject);
    layProject->setContentsMargins(0,4,0,4);
    layProject->setSpacing(2);
    layProject->addStretch();

    wView = new QWidget();
    tabMenuTop->addTab(wView, tr("  View  "));
    lhView = new QHBoxLayout(wView);
    lhView->setContentsMargins(4,4,4,4);
    lhView->addStretch();

    wTools = new QWidget();
    tabMenuTop->addTab(wTools, tr("  Tools  "));
    QHBoxLayout *layTools = new QHBoxLayout(wTools);
    layTools->setContentsMargins(4,4,4,4);
    layTools->setSpacing(2);
    layTools->addStretch();

    wSettings = new QWidget();
    tabMenuTop->addTab(wSettings, tr("  Settings  "));
    QHBoxLayout *laySettings = new QHBoxLayout(wSettings);
    laySettings->setContentsMargins(4,4,4,4);
    laySettings->addStretch();

    wAbout = new QWidget();
    tabMenuTop->addTab(wAbout, tr("  About  "));
    QHBoxLayout *layAbout = new QHBoxLayout(wAbout);
    layAbout->setContentsMargins(4,4,4,4);
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
    // Widget with device info.
    wScreenInfo = new QWidget(wView);
    wScreenInfo->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);
    wScreenInfo->setFont(QFont(FB_GUI_FONTTYPE, FB_GUI_FONTSIZE_SMALL));
    labScreenInfo1 = new QLabel(wScreenInfo);
    labScreenInfo2 = new QLabel(wScreenInfo);
    labScreenInfo3 = new QLabel(wScreenInfo);
    this->updateDeviceInfo();
    QVBoxLayout *lvScreenInfo = new QVBoxLayout(wScreenInfo);
    lvScreenInfo->setContentsMargins(2,2,2,2);
    lvScreenInfo->setSpacing(2);
    lvScreenInfo->addWidget(labScreenInfo1);
    lvScreenInfo->addWidget(labScreenInfo2);
    lvScreenInfo->addWidget(labScreenInfo3);
    lhView->insertWidget(lhView->count()-1,wScreenInfo);
    // Other.
    this->addTopMenuSplitter(wView);
    this->updateDevices(); // just for first appearance: for initial default screen
    this->updateStates();

    // Tools.
    toolbUndo = this->addTopMenuButton(wTools,":/img/undo.png",
                tr("Undo"),tr("Cancel last form \noperation"),false,true);
    toolbRedo = this->addTopMenuButton(wTools,":/img/redo.png",
                tr("Redo"),tr("Return last canceld\nform operation"),false,true);
    this->addTopMenuSplitter(wTools);
    toolbClearScreen = this->addTopMenuButton(wTools,":/img/clear_screen.png",
                tr("Clear"),tr("Clear all screen\nelements"),false,true);
    QObject::connect(toolbClearScreen, SIGNAL(clicked()),
                     this, SLOT(onClearScreenClick()));
    toolbDeleteElem = this->addTopMenuButton(wTools,":/img/delete_elem.png",
                 tr("Delete"),tr("Delete selected\nelement"),false,true);
    QObject::connect(toolbDeleteElem, SIGNAL(clicked()),
                     this, SLOT(onDeleteElemClick()));
    this->addTopMenuSplitter(wTools);
    toolbFieldManager = this->addTopMenuButton(wTools,":/img/fields.png",
      tr("Fields"),tr("Modify fields\nof the project"),false,true);
    QObject::connect(toolbFieldManager, SIGNAL(clicked()),
                     this, SLOT(onFieldsManagerClick()));
    toolbImportControls = this->addTopMenuButton(wTools,":/img/import_controls.png",
      tr("Import"),tr("Import elements\nfrom another project"),false,true);
    QObject::connect(toolbImportControls, SIGNAL(clicked()),
                     this, SLOT(onImportControlsClick()));
    toolbUpdateData = this->addTopMenuButton(wTools,":/img/update_data.png",
      tr("Update"),tr("Update layer with data\nfrom other Shapefile"),false,true);
    QObject::connect(toolbUpdateData, SIGNAL(clicked()),
                     this, SLOT(onUpdateDataClick()));
    // Settings.
    this->addTopMenuSpacer(wSettings);
    QStringList strsLangs;
    for (int i=0; i<languages.size(); i++)
    {
        strsLangs.append(languages[i].name);
    }
    comboLang = addTopMenuCombo(wSettings, tr("Language"), strsLangs);
    comboLang->setCurrentIndex(indexLang);
    QObject::connect(comboLang, SIGNAL(activated(int)),
                     this, SLOT(onLanguageSelect(int)));
    // About.
    this->addTopMenuSpacer(wAbout);
    this->addTopMenuLabel(wAbout, FBProject::getProgVersionStr(),
                          tr("Version of\nprogram"));
    QLabel *labAboutImg = new QLabel(wAbout);
    QPixmap pixAbout = QPixmap(languages[indexLang].imgNextgisPath);
    labAboutImg->setPixmap(pixAbout);
    labAboutImg->setAlignment(Qt::AlignCenter);
    this->addTopMenuSpacer(wAbout);
    QLabel *labAboutUrl = new QLabel(wAbout);
    labAboutUrl->setAlignment(Qt::AlignCenter);
    labAboutUrl->setText(QString("<a href=\"") + languages[indexLang].offLink
                   + QString("\">") + tr("Official page") + "</a>");
    labAboutUrl->setTextFormat(Qt::RichText);
    labAboutUrl->setTextInteractionFlags(Qt::TextBrowserInteraction);
    labAboutUrl->setOpenExternalLinks(true);
    labAboutUrl->setFont(QFont(FB_GUI_FONTTYPE,FB_GUI_FONTSIZE_SMALL));
    QVBoxLayout *lvAbout2 = new QVBoxLayout();
    lvAbout2->setContentsMargins(0,0,0,0);
    lvAbout2->setSpacing(5);
    lvAbout2->addWidget(labAboutImg);
    lvAbout2->addWidget(labAboutUrl);
    layAbout->insertLayout(layAbout->count()-1,lvAbout2);
    toolbAboutGraphics = this->addTopMenuButton(wAbout,":img/cc.png",
              tr("Graphics"),tr("Authors of images\nin program"), false, true, true);
    QObject::connect(toolbAboutGraphics, SIGNAL(clicked()),
                     this, SLOT(onAboutGraphicsClick()));

    //----------------------------------------------------------------------
    //                              Left menu
    //----------------------------------------------------------------------

    wMenuLeft = new QWidget(this);
    wMenuLeft->setObjectName("wMenuLeft");
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
    wMenuRight->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);

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
    lhMiddle->addWidget(wWorkingArea);
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
    this->setStyleSheet("QWidget#"+this->objectName()+" { background: white;"
                        " color: black; }"); // explicitly set dark font color, otherwise
                                             // users who have dark system background
                                             // color and light font color will have
                                             // troubles with displaying GUI
    tabMenuTop->setStyleSheet("QTabWidget {background: white;}"
                              "QTabWidget::pane {"
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

    wScreenInfo->setStyleSheet("QWidget"
                               "{background: "+QString(FB_COLOR_LIGHTGREY)+";"
                               "border: none; border-radius: 2px;"
                               "color: "+QString(FB_COLOR_DARKGREY)+"}");

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
                                  "background: white;"
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
                                  "padding-top: 5px;"
                                  "padding-bottom: 5px;"
                              "}"
                              "QTreeWidget::item:has-children"
                              "{"
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

    labBottom->setStyleSheet("QLabel {color: "+QString(FB_COLOR_DARKGREY)+"; "
                             "background: white;}");
}


/****************************************************************************/
/*                                                                          */
/*                         Main GUI private slots                           */
/*                                                                          */
/****************************************************************************/

// ADD ELEM FROM RIGHT MENU
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

    wScreen->requestScrollToBottom();

    FBElem *elem = fct->create();
    form->addElem(elem,NULL);

    this->onElemSelect();
}


// SELECT ELEM ON THE SCREEN
void FB::onElemSelect ()
{
    this->updateRightMenu();
}


// NEW VOID PROJECT BUTTON CLICK
void FB::onNewVoidClick ()
{
    toolbNewVoid->setDown(true);
    if (this->onAskToLeaveUnsafeProject())
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

        FBProjectVoid *projVoid = new FBProjectVoid(
                    FBProject::findGeomTypeByNgw(geomType));
        QObject::connect(projVoid, SIGNAL(changeProgress(int)),
                dlgProgress, SLOT(onChangeProgress(int)));
        this->newProjectCommonActions(projVoid, ""); // just void string stub
    }
}


// NEW SHAPEFILE PROJECT BUTTON CLICK
void FB::onNewShapeClick ()
{
    toolbNewShape->setDown(true);
    if (this->onAskToLeaveUnsafeProject())
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
    dialog.setDirectory(QDir()); // current directory
    QString lastPath = settLastShapeFullPath.value;
    if (lastPath != "")
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
        QObject::connect(projShp, SIGNAL(changeProgress(int)),
                dlgProgress, SLOT(onChangeProgress(int)));
        bool ok = this->newProjectCommonActions(projShp, pathShapefile);
        if (ok) settLastShapeFullPath.value = pathShapefile;
    }
}


// NEW NEXTGISWEB PROJECT BUTTON CLICK
void FB::onNewNgwClick ()
{
    toolbNewNgw->setDown(true);
    if (this->onAskToLeaveUnsafeProject())
    {
        toolbNewNgw->setDown(false);
        return;
    }

    QString lastUrl = settLastNgwUrl.value;
    QString lastLogin = settLastNgwLogin.value;
    FBDialogProjectNgw dialog(this,lastUrl,lastLogin);
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
        QObject::connect(projNgw, SIGNAL(changeProgress(int)),
                dlgProgress, SLOT(onChangeProgress(int)));
        this->newProjectCommonActions(projNgw, pathNgwUrl);
        settLastNgwUrl.value = strUrl; // anyway save last parameters
        settLastNgwLogin.value = strLogin;
    }
}


// OPEN PROJECT BUTTON CLICK
void FB::onOpenClick ()
{
    toolbOpen->setDown(true);
    if (this->onAskToLeaveUnsafeProject())
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
    dialog.setWindowTitle(tr("Open project. Select project file ..."));
    dialog.setDirectory(QDir()); // current directory
    QString lastPath = settLastNgfpFullPath.value;
    if (lastPath != "")
    {
        QFileInfo fileInfo(lastPath);
        dialog.setDirectory(fileInfo.absoluteDir());
    }
    QObject::connect(&dialog, SIGNAL(finished(int)),
                     this, SLOT(onProjDialogFinished(int)));

    if (dialog.exec())
    {
        // Check file extension anyway because user can type it manually.
        QString strFullPath = dialog.selectedFiles().first();
        if (!strFullPath.endsWith("." + QString(FB_PROJECT_EXTENSION),
                                  Qt::CaseInsensitive))
        {
            this->onShowError(tr("Wrong file extension. Must be .")
                          + QString(FB_PROJECT_EXTENSION));
            return;
        }

        // Create new project, by opening it.
        FBProject *projOpen = new FBProject();
        QObject::connect(projOpen, SIGNAL(changeProgress(int)),
                dlgProgress, SLOT(onChangeProgress(int)));
        FBErr err = projOpen->read(strFullPath);
        if (err != FBErrNone)
        {
            delete projOpen;
            this->onShowErrorFull(tr("Unable to open project!"), err);
            return;
        }

        // Create and show new form, replacing old one. Fill it with elems from
        // project.
        FBForm *form = this->createForm();
        if (!form->fromJson(FBProject::readForm(strFullPath)))
        {
            delete projOpen;
            this->onShowErrorFull(tr("Unable to read form in a project!"),
                                  FBErrReadNgfpFail);
            return;
        }

        // If all was correct: replace old project and form with new ones.
        if (project != NULL)
            delete project;
        project = projOpen;
        wScreen->deleteForm();
        wScreen->setForm(form);

        // Update list of fields for all Input elements.
        FBElemInput::updateFields(project->getFields().keys());

        this->pickDefaultScreen(); // will be helpful if there is void screen now
        this->updateRightMenu();
        this->updateEnableness();
        this->updateProjectString();

        settLastNgfpFullPath.value = strFullPath;
    }
}


// SAVE PROJECT BUTTON CLICK
void FB::onSaveClick ()
{
    toolbSave->setDown(true);

    FBForm *form = wScreen->getFormPtr();
    if (project == NULL || form == NULL || !project->wasFirstSaved())
    {
        toolbSave->setDown(false);
        return;
    }

    this->saveProjectCommonActions(project->getCurrentFilePath());
}


// SAVE AS PROJECT BUTTON CLICK
void FB::onSaveAsClick ()
{
    toolbSaveAs->setDown(true);

    FBForm *form = wScreen->getFormPtr();
    if (project == NULL || form == NULL)
    {
        toolbSaveAs->setDown(false);
        return;
    }

    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setViewMode(QFileDialog::List);
    dialog.setDefaultSuffix(FB_PROJECT_EXTENSION);
    dialog.setNameFilter("*." + QString(FB_PROJECT_EXTENSION));
    dialog.setWindowTitle(tr("Save project as ..."));
    dialog.setDirectory(QDir()); // current directory
    QString lastPath = settLastNgfpFullPath.value;
    if (lastPath != "")
    {
        QFileInfo fileInfo(lastPath);
        dialog.setDirectory(fileInfo.absoluteDir());
    }
    QObject::connect(&dialog,SIGNAL(finished(int)),
                  this,SLOT(onProjDialogFinished(int)));

    if (dialog.exec())
    {
        QString ngfpFullPath = dialog.selectedFiles().first();
        if (!ngfpFullPath.endsWith("."+QString(FB_PROJECT_EXTENSION),
                                  Qt::CaseInsensitive))
            ngfpFullPath = ngfpFullPath+"."+QString(FB_PROJECT_EXTENSION);

        this->saveProjectCommonActions(ngfpFullPath);
    }
}


// SCREENS PICK
void FB::onScreenAndroidPick ()
{
    FBScreenAndroid *screen = new FBScreenAndroid(NULL);
    this->recreateScreen(screen,false);
    this->afterPickScreen(toolbScreenAndroid);
}
void FB::onScreenIosPick ()
{
    FBScreenIos *screen = new FBScreenIos(NULL);
    this->recreateScreen(screen,false);
    this->afterPickScreen(toolbScreenIos);
}
void FB::onScreenWebPick ()
{
    FBScreen *screen = new FBScreen(NULL);
    this->recreateScreen(screen,false);
    this->afterPickScreen(toolbScreenWeb);
}
void FB::onScreenQgisPick ()
{
    FBScreen *screen = new FBScreen(NULL);
    this->recreateScreen(screen,false);
    this->afterPickScreen(toolbScreenQgis);
}

// SCREEN DEVICE SELECT
void FB::onScreenDeviceSelect (int index)
{
    if (index < 0)
        index = 0;
    if (index == 0)
        wWorkingArea->setWidgetResizable(true); // help widget to expand self
    else
        wWorkingArea->setWidgetResizable(false);
    wScreen->changeDevice(index);
    this->updateStates();
    this->updateDeviceInfo();
}

// SCREEN STATE PICK
void FB::onScreenStatePick ()
{
    // We must always have the default screen type - so here we set
    // the first occur button in the array.
    QObject *obj = this->sender();
    if (obj == NULL)
    {
        return;
    }
    int index = 0;
    for (int i=0; i<toolbsScreenState.size(); i++)
    {
        toolbsScreenState[i]->setDown(false);
        if (toolbsScreenState[i] == static_cast<QToolButton*>(obj))
        {
            index = i;
            toolbsScreenState[i]->setDown(true);
        }
    }
    wScreen->changeState(index);
}


// UNDO TOOL CLICK
void FB::onUndoClick ()
{

}
// REDO TOOL CLICK
void FB::onRedoClick ()
{

}


// CLEAR SCREEN TOOL CLICK
void FB::onClearScreenClick ()
{
    FBForm *form = wScreen->getFormPtr();
    if (form == NULL || form->isVoid())
        return;
    toolbClearScreen->setDown(true);
    if (this->onShowWarning(tr("Clear screen from all elements?"))
            != QMessageBox::Ok)
    {
        toolbClearScreen->setDown(false);
        return;
    }
    form->clear();
    this->updateRightMenu();
    toolbClearScreen->setDown(false);
}
// DELETE ELEM TOOL CLICK
void FB::onDeleteElemClick ()
{
    FBForm *form = wScreen->getFormPtr();
    if (form == NULL || form->isVoid())
        return;
    toolbDeleteElem->setDown(true);
    form->deleteSelected();
    this->updateRightMenu();
    toolbDeleteElem->setDown(false);
}


// FIELDS MANAGER
void FB::onFieldsManagerClick ()
{
    if (project == NULL)
        return;

    toolbFieldManager->setDown(true);

    FBDialogFieldsManager dialog(this);
    dialog.loadFields(project->getFields());
    QObject::connect(&dialog,SIGNAL(finished(int)),
                  this,SLOT(onProjDialogFinished(int)));

    if (dialog.exec())
    {
        QMap<QString,FBField> newFields = dialog.getFields();
        QSet<QString> deletedFields = dialog.getDeletedFields();

        // Reset project fields with newly created list.
        // Also we must update the list of deleted fields after each session of
        // the dialog, because we must remember which existing fields (of the
        // underlying layer) to delete during savig project time in case when the
        // new fields with such names have been created after the deletion of these
        // fields.
        // Note: we can do all this because the keynames of fields are unique.
        project->modifyFields(newFields,deletedFields);

        // Update list of fields for all Input elements.
        // After updating we must reset the fields for Input elements, which were
        // added via dialog after the deletion of fields with the same keynames as
        // deleted ones, because for now they are still being selected in the elems.
        FBElemInput::updateFields(project->getFields().keys());
        FBForm* form = wScreen->getFormPtr();
        if (form != NULL)
        {
            QList<FBElem*> elems = form->getAllElems();
            for (int i=0; i<elems.size(); i++)
            {
                // This is only for Input elems.
                FBElemInput *e = qobject_cast<FBElemInput*>(elems[i]);
                if (e != NULL)
                {
                    // Get selected (for the other project) field of elem.
                    // Some elements may have several Field attributes, so we try to
                    // reset them all, if needed.
                    QStringList selectedFields = e->getSelectedFields();
                    for (int k=0; k<selectedFields.size(); k++)
                    {
                        if (deletedFields.contains(selectedFields[k]))
                        {
                            e->resetSelectedField(selectedFields[k]);
                        }
                    }
                }
            }
        }

        // Update right menu so the comboboxes with lists of fields of currently
        // selected elem will be updated.
        this->updateRightMenu();
    }
}


// IMPORT CONTROLS
void FB::onImportControlsClick()
{
    FBForm* form = wScreen->getFormPtr();
    if (project == NULL || form == NULL)
        return;

    toolbImportControls->setDown(true);

    if (!form->isVoid() && this->onShowWarning(tr("If you import elements from"
        " another project all current elements will be removed. Continue?"))
            != QMessageBox::Ok)
    {
        toolbImportControls->setDown(false);
        return;
    }

    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::List);
    dialog.setDefaultSuffix(FB_PROJECT_EXTENSION);
    dialog.setNameFilter("*."+QString(FB_PROJECT_EXTENSION));
    dialog.setWindowTitle(tr("Import form elements. Select project file ..."));
    dialog.setDirectory(QDir()); // current directory
    QString lastPath = settLastNgfpFullPath.value;
    if (lastPath != "")
    {
        QFileInfo fileInfo(lastPath);
        dialog.setDirectory(fileInfo.absoluteDir());
    }
    QObject::connect(&dialog,SIGNAL(finished(int)),
                  this,SLOT(onProjDialogFinished(int)));

    if (dialog.exec())
    {
        // Check file extension anyway because user can type it manually.
        QString strFullPath = dialog.selectedFiles().first();
        if (!strFullPath.endsWith("." + QString(FB_PROJECT_EXTENSION),
                                  Qt::CaseInsensitive))
        {
            this->onShowError(tr("Wrong file extension. Must be .")
                          + QString(FB_PROJECT_EXTENSION));
            return;
        }

        // TODO: check, if the selected file is the current project file.

        // Process selected project.
        FBProject *projOther = new FBProject();
        FBErr err = projOther->read(strFullPath);
        if (err != FBErrNone)
        {
            delete projOther;
            this->onShowErrorFull(tr("Unable to process selected project!"),err);
            return;
        }

        // Load and show its form elements, clearing the current ones.
        if (!form->fromJson(FBProject::readForm(strFullPath)))
        {
            delete projOther;
            this->onShowErrorFull(tr("Unable to read form in a project!"),
                                  FBErrReadNgfpFail);
            return;
        }

        // Loaded Input elements now have other field bindings so we must reset
        // them but excluding thouse fields which are valid for the current project.
        // Firstly we form a list with the FULLY EQUAL fields from both prjects.
        QMap<QString,FBField> fieldsOther = projOther->getFields();
        delete projOther;
        QMap<QString,FBField> fieldsThis = project->getFields();
        QSet<QString> fieldsSame;
        QMap<QString,FBField>::const_iterator it = fieldsThis.constBegin();
        while (it != fieldsThis.constEnd()) // we are not interested in the fields
        {                                    // of other project, so look through the
            QString keyname = it.key();      // current project's fields
            FBField fieldThis = it.value();
            FBField fieldOther = fieldsOther.value(keyname);
            if (fieldThis == fieldOther) // will be compared with default-
            {                                   // constructed field if there is no
                fieldsSame.insert(keyname);     // such field in other project
            }
            ++it;
            // NOTE: with this approach for example if fieldsThis has the field with
            // "Id" keyname and at the same time fieldsOther has the field with "ID"
            // keyname - these fields will be regarded unequal because QMap::value() of
            // fieldsOther will not find "Id" key.
            // Currently we allow such behaviour, but this is a not critical error: all
            // fields' keynames must be compared case-insensitively, so "ID" and
            // "Id" must be regarded as equal. See Field's dialog class why we do so.
            // TODO: to solve this problem: remove the QMap::value() search and
            // add manual looking for equal keynames in a loop with case-insensitive
            // check.
        }

        // Finally reset all field bindings except the equal fields.
        QSet<QString> fieldsReseted;
        QList<FBElem*> elems = form->getAllElems();
        for (int i=0; i<elems.size(); i++)
        {
            // This is only for Input elems.
            FBElemInput *e = qobject_cast<FBElemInput*>(elems[i]);
            if (e != NULL)
            {
                // Get selected (for the other project) field of elem.
                // Some elements may have several Field attributes, so we try to
                // reset them all, if needed.
                QStringList selectedFields = e->getSelectedFields();
                for (int k=0; k<selectedFields.size(); k++)
                {
                    if (!fieldsSame.contains(selectedFields[k]))
                    {
                        e->resetSelectedField(selectedFields[k]);
                        fieldsReseted.insert(selectedFields[k]);
                    }
                }
            }
        }

        // Report to user which fields have been reseted.
        if (!fieldsReseted.isEmpty())
        {
            QString strFields = "";
            QSet<QString>::const_iterator it = fieldsReseted.constBegin();
            while (it != fieldsReseted.constEnd())
            {
                strFields = strFields + "[" + *it + "], ";
                ++it;
            }
            strFields.chop(2); // remove last ", " ending
            this->onShowInfo(tr("The bindings to the following fields of the imported"
                                " elements have been reset, because such fields do not"
                                " exist in the current project: ") + strFields);
        }

        // Update right menu so the comboboxes with lists of fields of currently
        // selected elem will be updated.
        this->updateRightMenu();
    }
}


// UPDATE DATA
void FB::onUpdateDataClick ()
{
    if (project == NULL)
        return;

    toolbUpdateData->setDown(true);

    // We do not allow to update data until the project is saved first time,
    // because we do not have the ngfp file in which we will replace data yet
    // (though this is checked in writeData() method).
    if (!project->wasFirstSaved())
    {
        toolbUpdateData->setDown(false);
        this->onShowError(tr("Project is not saved yet. Please save project"
                             " before updating it's data"));
        return;
    }

    // Also do not allow to update data if there are some field modifications
    // currently hanging unsaved, so to strictly devide the changing of data
    // structure and data itself (though field modifications are ignored in
    // writeData() method).
    if (project->needToSaveFieldsModifics())
    {
        toolbUpdateData->setDown(false);
        this->onShowError(tr("Project has unsaved fields' modifications. Please"
                             " save them before updating data"));
        return;
    }

    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::List);
    dialog.setDefaultSuffix("shp");
    dialog.setNameFilter("*.shp");
    dialog.setWindowTitle(tr("Update data in project. Select a Shapefile ..."));
    dialog.setDirectory(QDir()); // current directory
    QString lastPath = settLastShapeFullPath.value;
    if (lastPath != "")
    {
        QFileInfo fileInfo(lastPath);
        dialog.setDirectory(fileInfo.absoluteDir());
    }
    QObject::connect(&dialog,SIGNAL(finished(int)),
                  this,SLOT(onProjDialogFinished(int)));

    if (dialog.exec())
    {
        QString pathShapefile;
        QStringList sPaths = dialog.selectedFiles();
        pathShapefile = sPaths[0];

        // TODO: do it in separate thread like in Save As method.
        FBErr err = project->writeData(pathShapefile);
        if (err != FBErrNone)
        {
            this->onShowErrorFull(tr("Unable to update layer's data!"), err);
            return;
        }

        // FOR FUTURE: show the list of updated fields here.
        this->onShowInfo(tr("Layer's data updated successfully"));
    }
}


// ARROWS CLICK
void FB::onLeftArrowClick ()
{
    this->flipLeftMenu(!treeLeftFull->isVisible());
}
void FB::onRightArrowClick ()
{
    this->flipRightMenu(!labRight->isVisible());
}


// LANGUAGE SELECT
void FB::onLanguageSelect (int index)
{
    if (index == indexLang)
        return;
    indexLang = index;
    settLastLanguageSelected.value = languages[indexLang].code;
    this->onShowInfo(tr("To change language please restart the application"));
}


// ABOUT CLICK
void FB::onAboutGraphicsClick ()
{
    FBDialogAbout dialog(this);
    dialog.exec();
}


// KEY PRESS
void FB::keyPressEvent (QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete)
    {
        FBForm *form = wScreen->getFormPtr();
        if (form != NULL)
        {
            form->deleteSelected();
            this->updateRightMenu();
        }
        return;
    }
    // Form docs: if you reimplement this handler, it is very important that you
    // call the base class implementation if you do not act upon the key.
    QWidget::keyPressEvent(event);
}


/*****************************************************************************/
/*                                                                           */
/*                          Other private slots                              */
/*                                                                           */
/*****************************************************************************/

void FB::onShowInfo(QString msg)
{
    onShowBox(msg,tr("Information"));
}

int FB::onShowWarning (QString msg)
{
    return onShowBox(msg,tr("Warning"));
}

void FB::onShowError (QString msg)
{
    onShowBox(msg,tr("Error"));
}

int FB::onShowBox (QString msg, QString caption)
{
    QMessageBox msgBox(this);
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

int FB::onShowErrorFull (QString msgMain, FBErr err)
{
    QMessageBox msgBox(this);
    msgBox.setStyleSheet("");
    msgBox.setWindowTitle(tr("Error"));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText(msgMain);
    QString str = this->getErrStr(err);
    if (str != "")
    {
        msgBox.setInformativeText(str);
        msgBox.setDetailedText(FBProject::CUR_ERR_INFO);
    }
    FBProject::CUR_ERR_INFO = "";
    return msgBox.exec();
}


// Some common actions.
// Call only in according methods.
bool FB::onAskToLeaveUnsafeProject ()
{
    return (project != NULL
            && project->isSaveRequired()
            && this->onShowWarning(tr("Project hasn't been saved. Continue?"))
                != QMessageBox::Ok);
}


// Some common GUI actions after closing project dialogs.
// Connect only to finished() signal of according dialogues.
void FB::onProjDialogFinished (int code)
{
    toolbNewVoid->setDown(false);
    toolbNewShape->setDown(false);
    toolbNewNgw->setDown(false);
    toolbOpen->setDown(false);
    toolbSave->setDown(false);
    toolbSaveAs->setDown(false);
    toolbFieldManager->setDown(false);
    toolbImportControls->setDown(false);
    toolbUpdateData->setDown(false);
}


// This slot is called after the long action of saving project is ended in the
// separate thread.
// Connect only to according methods.
void FB::onSaveAnyEnded (FBErr err)
{
    // Close progress dialog and do according actions.
    dlgProgress->accept();
    if (err != FBErrNone)
    {
        this->onShowErrorFull(tr("Error saving project"),err);
    }
    else
    {
        this->onShowInfo(tr("Project saved successfully"));
        this->updateEnableness();
        this->updateProjectString();
        settLastNgfpFullPath.value = project->getCurrentFilePath();
    }
    toolbSaveAs->setDown(false);
    toolbSave->setDown(false);
}


/****************************************************************************/
/*                                                                          */
/*                            Settings                                      */
/*                                                                          */
/****************************************************************************/

// Note: example path on Windows: C:\Users\Mikhail\AppData\Roaming\NextGIS
void FB::writeSettings ()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       "NextGIS", "FormBuilder");
    settings.setValue(settLastShapeFullPath.key,settLastShapeFullPath.value);
    settings.setValue(settLastNgfpFullPath.key,settLastNgfpFullPath.value);
    settings.setValue(settLastLanguageSelected.key,settLastLanguageSelected.value);
    settings.setValue(settLastNgwUrl.key,settLastNgwUrl.value);
    settings.setValue(settLastNgwLogin.key,settLastNgwLogin.value);
}
void FB::readSettings ()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                        "NextGIS", "FormBuilder");
    settLastShapeFullPath.value = settings.value(settLastShapeFullPath.key,
                                   settLastShapeFullPath.defaultValue).toString();
    settLastNgfpFullPath.value = settings.value(settLastNgfpFullPath.key,
                                   settLastNgfpFullPath.defaultValue).toString();
    settLastLanguageSelected.value = settings.value(settLastLanguageSelected.key,
                                   settLastLanguageSelected.defaultValue).toString();
    settLastNgwUrl.value = settings.value(settLastNgwUrl.key,
                                   settLastNgwUrl.defaultValue).toString();
    settLastNgwLogin.value = settings.value(settLastNgwLogin.key,
                                   settLastNgwLogin.defaultValue).toString();
}


/****************************************************************************/
/*                                                                          */
/*                            Convertions                                   */
/*                                                                          */
/****************************************************************************/

// Convert main error code to string.
QString FB::getErrStr (FBErr err)
{
    QString ret;
    switch (err)
    {
        case FBErrWrongVersion: ret = tr("Wrong file version"); break;
        case FBErrIncorrectJson: ret = tr("Incorrect JSON file structure"); break;
        case FBErrIncorrectFileStructure: ret = tr("Incorrect file structure"); break;
        case FBErrIncorrectGdalDataset: ret = tr("Incorrect GDAL dataset"); break;
        case FBErrBadSavePath: ret = tr("Bad save path"); break;
        case FBErrTempFileFail: ret = tr("Temporary file error"); break;
        case FBErrGDALFail: ret = tr("GDAL error"); break;
        case FBErrCPLFail: ret = tr("GDAL CPL error"); break;
        case FBErrReadNgfpFail: ret = tr("Reading project file error"); break;
        case FBErrStructureDiffers: ret = tr("Different file structures"); break;
        default: ret = ""; break; // some errors will be explicitly not processed
    }
    return ret;
}


// Convert group type of element to string.
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


/****************************************************************************/
/*                                                                          */
/*                            Top menu                                      */
/*                                                                          */
/****************************************************************************/

// Create and add a new button for any tab of the top menu.
QToolButton *FB::addTopMenuButton (QWidget *parentTab, QString imgPath, QString name,
         QString description, bool isSmall, bool withCaption, bool atTheEnd)
{
    QToolButton *but = new QToolButton(parentTab);
    QHBoxLayout *lhParent = (QHBoxLayout*)parentTab->layout();
    but->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    but->setAutoRaise(true);
    but->setIcon(QIcon(imgPath));
    but->setText(name); // necessarily do this because it will store correspondence
                        // to screen arrays for screen menu buttons
    but->setFont(QFont(FB_GUI_FONTTYPE, FB_GUI_FONTSIZE_SMALL));
    but->setToolTip(description);
    but->setCursor(Qt::PointingHandCursor);
    // TODO: Make real semitransparent style.
    but->setStyleSheet("QToolButton"
      "{border: none; color: "+QString(FB_COLOR_DARKGREY)+";}"
      "QToolButton:hover"
      "{background-color: "+FB_COLOR_DARKBLUE+"; color: white;}"
      "QToolButton:pressed{"
      "background-color: "+FB_COLOR_LIGHTBLUE+"; color: white;}"
      "QToolButton:disabled{}");
    if (isSmall)
    {
        but->setIconSize(QSize(60,60));
        but->setFixedSize(35,35);
    }
    else
    {
        but->setIconSize(QSize(60,60));
        but->setFixedSize(65,65);
    }

    QVBoxLayout *lay = new QVBoxLayout();
    lay->addWidget(but);

    if (withCaption)
    {
        but->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        but->setFont(QFont(FB_GUI_FONTTYPE,FB_GUI_FONTSIZE_SMALL));
        but->setIconSize(QSize(43,43));
    }

    if (atTheEnd)
    {
        lhParent->addLayout(lay);
    }
    else
    {
        lhParent->insertLayout(lhParent->count()-1,lay); // last is stretch item.
    }

    return but;
}


// Create and add a new combobox with caption for any tab of the top menu.
QComboBox *FB::addTopMenuCombo (QWidget *parentTab, QString caption,
                                QStringList values)
{
    QComboBox *combo = new QComboBox(parentTab);
    combo->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Fixed);
    combo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    //combo->setMaximumWidth(125);
    combo->setFont(QFont("Segoe UI",FB_GUI_FONTSIZE_NORMAL));
    for (int i=0; i<values.size(); i++)
    {
        combo->addItem(values[i]);
    }

    QLabel *lab = new QLabel(parentTab);
    lab->setText(caption);
    lab->setAlignment(Qt::AlignCenter);
    lab->setFont(QFont(FB_GUI_FONTTYPE,FB_GUI_FONTSIZE_SMALL));
    lab->setStyleSheet("QLabel {color: "+QString(FB_COLOR_DARKGREY)+"}");
    lab->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);

    QHBoxLayout *lhParent = (QHBoxLayout*)parentTab->layout();
    QVBoxLayout *vl = new QVBoxLayout();
    vl->setContentsMargins(0,0,0,0);
    vl->setSpacing(10);
    vl->addWidget(combo);
    vl->addWidget(lab);
    lhParent->insertLayout(lhParent->count()-1,vl);

    return combo;
}


// Create and add a splitter vertical line for any tab in the top menu.
void FB::addTopMenuSplitter (QWidget *parentTab)
{
    QWidget *wid = new QWidget(wView);
    wid->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    wid->setFixedWidth(1);
    wid->setStyleSheet("QWidget {background-color: "
                             +QString(FB_COLOR_LIGHTMEDIUMGREY)+"}");
    QHBoxLayout *lhParent = (QHBoxLayout*)parentTab->layout();
    lhParent->insertWidget(lhParent->count()-1,wid); // last is stretch item.
}


// Create and add a void area for any tab in the top menu.
void FB::addTopMenuSpacer (QWidget *parentTab)
{
    QHBoxLayout *lhParent = (QHBoxLayout*)parentTab->layout();
    lhParent->insertSpacing(lhParent->count()-1,10); // last is stretch item.
}


// Create and add a label for any tab in the top menu.
QLabel *FB::addTopMenuLabel(QWidget *parentTab, QString text, QString caption)
{
    QLabel *lab1 = new QLabel(parentTab);
    lab1->setText(caption);
    lab1->setAlignment(Qt::AlignCenter);
    lab1->setFont(QFont(FB_GUI_FONTTYPE,FB_GUI_FONTSIZE_SMALL));
    lab1->setStyleSheet("QLabel {color: " + QString(FB_COLOR_DARKGREY) + "}");
    QLabel *lab2 = new QLabel(parentTab);
    lab2->setText(text);
    lab2->setAlignment(Qt::AlignCenter);
    lab2->setFont(QFont(FB_GUI_FONTTYPE, 18));
    lab2->setStyleSheet("QLabel {color: " + QString(FB_COLOR_VERYDARKGREY) + "}");
    QVBoxLayout *lv = new QVBoxLayout();
    lv->setContentsMargins(0,0,0,0);
    lv->setSpacing(5);
    lv->addWidget(lab2);
    lv->addWidget(lab1);
    QHBoxLayout *lhParent = (QHBoxLayout*)parentTab->layout();
    lhParent->insertLayout(lhParent->count()-1,lv); // last is stretch item.
    return lab1;
}


/****************************************************************************/
/*                                                                          */
/*                               GUI                                        */
/*                                                                          */
/****************************************************************************/

// Common actions for form creation.
FBForm *FB::createForm ()
{
    FBForm *form = new FBForm();
    QObject::connect(form, SIGNAL(elemPressed()),
                     this, SLOT(onElemSelect()));
    return form;
}


// Minimize or maximize menus.
void FB::flipLeftMenu (bool isFull)
{
    if (isFull)
        wMenuLeft->setFixedWidth(240);
    else
        wMenuLeft->setFixedWidth(70);


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
    if (isFull)
        wMenuRight->setFixedWidth(300);
    else
        wMenuRight->setFixedWidth(40);


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


// Create one table in right menu.
QTableWidget* FB::addRightMenuTable ()
{
    QTableWidget *table = new QTableWidget(wMenuRight);
    table->setMinimumHeight(0);
    //table->setMaximumHeight(50);
    table->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    table->setFocusPolicy(Qt::NoFocus);
    table->setFont(QFont(FB_GUI_FONTTYPE,FB_GUI_FONTSIZE_NORMAL));
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


// Update enable states for all GUI elements in program.
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
        toolbImportControls->setEnabled(false);
        toolbUpdateData->setEnabled(false);
        toolbFieldManager->setEnabled(false);
    }
    else
    {
        wMenuLeft->setEnabled(true);
        wMenuRight->setEnabled(true);
        if (project->wasFirstSaved())
        {
            toolbSave->setEnabled(true);
        }
        else
        {
            toolbSave->setEnabled(false);
        }
        toolbSaveAs->setEnabled(true);
        toolbUndo->setEnabled(true);
        toolbRedo->setEnabled(true);
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
        toolbImportControls->setEnabled(true);
        toolbUpdateData->setEnabled(true);
        toolbFieldManager->setEnabled(true);
    }

    // TEMPORARY:
    //toolbScreenWeb->setEnabled(false);
    //toolbScreenQgis->setEnabled(false);
    toolbUndo->setEnabled(false);
    toolbRedo->setEnabled(false);
}


// Clear and than fill two left trees with element factories.
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

    //treeLeftFull->expandAll();
    //treeLeftFull->topLevelItem(treeLeftFull->topLevelItemCount()-1)
    //        ->setExpanded(false);
}


// Delete old and than create new tables for right menu and update its label.
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
            widget->setStyleSheet("QWidget{background: white;}");
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
        // Show string that many elems are selected.
        labRight->setStyleSheet("QLabel{border: none;color: "
                                +QString(FB_COLOR_LIGHTBLUE)+";}");
        labRight->setText(tr("Elements selected: ")
                          + QString::number(elemsSelected.size()));
    }
}


// Set bottom status label.
void FB::setBottomString (QString strToShorten, QString strToPrepend)
{
    if (strToShorten.size() > FB_BOTTOMSTRING_LEN_MAX)
    {
        strToShorten.remove(0, strToShorten.size() - FB_BOTTOMSTRING_LEN_MAX);
        strToShorten.prepend(" ... ");
    }
    labBottom->setText(" " + strToPrepend + strToShorten);
}


// Set bottom status label for project info.
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
        strToShorten = project->getCurrentFilePath();
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


void FB::updateDevices ()
{
    comboScreenDevice->clear();
    QList<FBDevice> devices = wScreen->getDevices();
    for (int i=0; i<devices.size(); i++)
    {
        comboScreenDevice->addItem(QIcon(devices[i].getImgPath()),
                                   devices[i].getName());
    }
    // Select defaults.
    comboScreenDevice->setCurrentIndex(0); // will not trigger slot
}


void FB::updateStates ()
{
    for (int i=0; i<toolbsScreenState.size(); i++)
    {
        lhView->removeWidget(toolbsScreenState[i]);
        delete toolbsScreenState[i];
    }
    toolbsScreenState.clear();
    QList<FBState> states = wScreen->getCurDevice().getStates();
    for (int i=0; i<states.size(); i++)
    {
        toolbsScreenState.append(this->addTopMenuButton(wView,states[i].imgPath,
                                 states[i].name,states[i].descr));
        QObject::connect(toolbsScreenState.last(),SIGNAL(clicked()),
                         this,SLOT(onScreenStatePick()));
    }
    // Select defaults.
    for (int i=0; i<toolbsScreenState.size(); i++)
    {
        toolbsScreenState[i]->setDown(false);
    }
    toolbsScreenState[0]->setDown(true);
}


void FB::updateDeviceInfo ()
{
    FBDevice device = wScreen->getCurDevice();

    labScreenInfo1->setText(tr(" Resolution:  "));
    if (device.getResolution().first != -1 && device.getResolution().second != -1)
    {
        labScreenInfo1->setText(labScreenInfo1->text()
              + QString::number(device.getResolution().first) + " x "
              + QString::number(device.getResolution().second) + " ");
    }
    else
    {
        labScreenInfo1->setText(labScreenInfo1->text() + "-  ");
    }

    labScreenInfo2->setText(tr(" Diagonal:  "));
    if (device.getDiagonal() != -1.0)
    {
        labScreenInfo2->setText(labScreenInfo2->text()
               + QString::number(device.getDiagonal()) + "\'\'");
    }
    else
    {
        labScreenInfo2->setText(labScreenInfo2->text() + "-  ");
    }

    labScreenInfo3->setText(tr(" DPI:  "));
    if (device.getDpi() != -1.0)
    {
        labScreenInfo3->setText(labScreenInfo3->text()
               + QString::number(device.getDpi()));
    }
    else
    {
        labScreenInfo3->setText(labScreenInfo3->text() + "-  ");
    }
}


// Some common GUI actions: after change screen settings.
// Call only in according methods.
void FB::afterPickScreen (QToolButton *toolbDown)
{
    this->updateDevices();
    this->updateStates();
    this->updateDeviceInfo();

    // Highlight buttons:
    toolbScreenAndroid->setDown(false);
    toolbScreenIos->setDown(false);
    toolbScreenWeb->setDown(false);
    toolbScreenQgis->setDown(false);
    if (toolbDown != NULL)
        toolbDown->setDown(true);
}


/****************************************************************************/
/*                                                                          */
/*                             Screen                                       */
/*                                                                          */
/****************************************************************************/

// Recreate screen to default one.
void FB::pickDefaultScreen ()
{
    // TODO: think about what other screen to set in this situation - may be
    // read it from the current project structure. Currently we set Android
    // screen.
    FBScreenAndroid *screen = new FBScreenAndroid(NULL);
    this->recreateScreen(screen,false); // with form copy, if was some
    this->afterPickScreen(toolbScreenAndroid);
}
// Recreate screen to void. Just "reset to grey screen".
void FB::pickVoidScreen ()
{
    FBScreen *screen = new FBScreen(NULL);
    this->recreateScreen(screen,true); // do not copy form
    this->afterPickScreen(NULL);
}


// Create new screen with copying old form (or not).
// Pass only newly created screen!
void FB::recreateScreen (FBScreen *newScreen, bool destroyForm)
{
    if (newScreen == NULL)
        return;
    if (!destroyForm)
    {
        FBForm *curForm = wScreen->takeForm();
        newScreen->setForm(curForm);
    }
    newScreen->changeDevice(0);
    newScreen->changeState(0);
    newScreen->updateStyle();
    wWorkingArea->takeWidget();
    delete wScreen;
    wWorkingArea->setWidget(newScreen);
    wWorkingArea->setWidgetResizable(true); // because 0 index of device by default
    wScreen = newScreen;
}


/****************************************************************************/
/*                                                                          */
/*                        Project common actions                            */
/*                                                                          */
/****************************************************************************/

// Common steps for all new projects creation methods.
// Call only in according methods.
bool FB::newProjectCommonActions (FBProject *proj, QString path)
{
    // Replacing old project if new one was correctly created.
    FBErr err = proj->readFirst(path);
    if (err != FBErrNone)
    {
        delete proj;
        this->onShowErrorFull(tr("Unable to create new project! "), err);
        return false;
    }
    if (project != NULL)
        delete project;
    project = proj;

    // Create and show new void form replacing old one.
    wScreen->deleteForm();
    FBForm *form = this->createForm();
    wScreen->setForm(form);

    // Reset list of fields for all future Input elements.
    FBElemInput::updateFields(project->getFields().keys());

    this->pickDefaultScreen(); // will be helpful if there is void screen now
    this->updateRightMenu();
    this->updateEnableness();
    this->updateProjectString();
    return true;
}

// Common steps for all saving methods.
// Call only in according methods.
void FB::saveProjectCommonActions (QString ngfpFullPath)
{
    // Start new thread for the long saving action.
    // At the same time we show a modal dialog in order to block all GUI events. If
    // do not do this - all occasional clicks and actions by user during the work of
    // the thread can cause an "undefined behaviour" because widgets will receive
    // mouse events right after the thread ends its work. What did not help to solve
    // this problem:
    // 1. From Qt docs: QProgressDialog pr(...); pr->show();
    // pr.setWindowModality(Qt::WindowModal);
    // 2. QApplication::processEvents(QEventLoop::ExcludeUserInputEvents) as addition
    // to 1;
    // 3. Blocking the window or all widgets via setEnabled(false).

    dlgProgress->setWindowTitle(tr("Saving ..."));
    FBThreadSaveAs *thread = new FBThreadSaveAs(this, ngfpFullPath, project,
                      wScreen->getFormPtr()->toJson()); // NULL-check was made outside
    QObject::connect(thread,SIGNAL(resultReady(FBErr)),
                     this, SLOT(onSaveAnyEnded(FBErr)));
    QObject::connect(thread,SIGNAL(finished()),
                     thread, SLOT(deleteLater()));
    thread->start();
    dlgProgress->exec();

    // The further work is made in the separate thread which ends with calling
    // onSaveAsEnded() method.
}


/*****************************************************************************/
/*                                                                           */
/*                           FBDialogProgress                                */
/*                                                                           */
/*****************************************************************************/

FBDialogProgress::FBDialogProgress (QWidget *parent):
    QDialog(parent)
{
    this->setStyleSheet("");
    this->setStyleSheet("QWidget { color: black }");

    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint
                         | Qt::CustomizeWindowHint); // hide all window buttons
    this->setWindowTitle("...");
    this->setModal(true);

    bar = new QProgressBar(this);
    bar->setMaximum(99);
    bar->setValue(0);

    QVBoxLayout *l = new QVBoxLayout(this);
    l->addWidget(bar);
}


/*****************************************************************************/
/*                                                                           */
/*                           FBThreadSaveAs                                  */
/*                                                                           */
/*****************************************************************************/

FBThreadSaveAs::FBThreadSaveAs (QObject *parent, QString strFullPath,
                                FBProject *project, Json::Value jsonForm):
    QThread (parent)
{
    this->strFullPath = strFullPath;
    this->project = project;
    this->jsonForm = jsonForm;
}

void FBThreadSaveAs::run ()
{
    FBErr err = project->write(strFullPath,jsonForm);
    emit resultReady(err);
}


