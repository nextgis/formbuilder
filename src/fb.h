/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  main app+gui definitions
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
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QTabWidget>
#include <QTreeWidget>
#include <QTableWidget>
#include <QScrollArea>
#include <QToolButton>
#include <QProgressBar>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFileInfo>
#include <QThread>
#include <QSettings>

#include "project/project_core.h"
#include "form/form_core.h"
#include "screen/screen_core.h"

#include "project/projects.h"
#include "screen/screens.h"

// GUI constants.
#define FB_GUI_FONTTYPE "Candara"
#define FB_GUI_FONTTYPE_2 "Segoe UI"
#define FB_GUI_FONTSIZE_NORMAL 11
#define FB_GUI_FONTSIZE_SMALL 9
#define FB_GUI_SIZE_MENUTOP_MIN 100
#define FB_GUI_SIZE_MENUTOP_MAX 120
#define FB_GUI_SIZE_TOPBUT_ICON 60
#define FB_GUI_SIZE_TOPBUT_BIG 65
#define FB_GUI_SIZE_TOPBUT_SMALL 35
#define FB_GUI_SIZE_TOPBUT_CAPTION 43
#define FB_GUI_SIZE_TOPSPACE 10
#define FB_GUI_SIZE_TOPSPLITT 1
#define FB_GUI_FONTSIZE_TOPLABEL 18
#define FB_GUI_SIZE_TREELEFTSMALL 35
#define FB_GUI_SIZE_TREEICON 20
#define FB_GUI_SIZE_TREEITEM_HEIGHT 20
#define FB_GUI_SIZE_TREEITEM_PADD 5
#define FB_GUI_SIZE_MENULEFT_FULL 240
#define FB_GUI_SIZE_MENULEFT_SMALL 70
#define FB_GUI_SIZE_MENURIGHT_FULL 300
#define FB_GUI_SIZE_MENURIGHT_SMALL 40
#define FB_COLOR_LIGHTGREY "rgb(238,238,238)"
#define FB_COLOR_LIGHTMEDIUMGREY "rgb(210,210,210)"
#define FB_COLOR_MEDIUMGREY "rgb(170,170,170)"
#define FB_COLOR_DARKGREY "rgb(100,100,100)"
#define FB_COLOR_VERYDARKGREY "rgb(46,46,46)"
#define FB_COLOR_LIGHTBLUE "rgb(139,183,224)"
#define FB_COLOR_DARKBLUE "rgb(23,111,193)"

// Limits for GUI.
#define FB_MENURIGHT_TABLES_MAX 5
#define FB_BOTTOMSTRING_LEN_MAX 60

#define FB_SCREEN_SIZEFACTOR 130.0 // used for actual screen scale


namespace Ui
{
    class FB;
}


/**
 * New project's dialog.
 * Used to select geometry type of newly created project.
 */
class FBDialogProjectNew: public QDialog
{
    Q_OBJECT
    public:
     FBDialogProjectNew (QWidget *parent);
     ~FBDialogProjectNew ();
     QString getSelectedGeom();
    private:
     QComboBox *comboGeom;
};


/**
 * NextGIS Web new project's dialog.
 * Used to select resource on NGW server (finally received as GeoJSON dataset).
 * See NextGIS Web API how to get json data from NGW server in the methods
 * of this dialogue.
 */
class FBDialogProjectNgw: public QDialog
{
    Q_OBJECT
    public:
     FBDialogProjectNgw (QWidget *parent, QString lastNgwUrl, QString lastNgwLogin);
     ~FBDialogProjectNgw ();
     QString getSelectedNgwResource (QString &strUrl, QString &strUrlName,
         QString &strLogin, QString &strPass, int &strId, Json::Value &jsonLayerMeta);
    signals:
     void updateNgwSettings (QString lastUrl, QString lastLogin);
    private slots:
     void onConnectClicked ();
     void onSelectClicked ();
    void onCheckboxClick (bool pressed);
     void httpOnItemExpended (QTreeWidgetItem *treeItem);
     void httpOnItemCollapsed (QTreeWidgetItem *treeItem);
     void httpOnItemClicked (QTreeWidgetItem *treeItem, int treeItemColumn);
     void httpReadyAuthRead ();
     void httpReadyRead ();
     void httpReadyResourceRead ();
     void httpReadySelectedRead ();
     void httpAuthFinished ();
     void httpFinished ();
     void httpResourceFinished ();
     void httpSelectedFinished ();
    private:
     QList<QTreeWidgetItem*> parseJsonReply (QNetworkReply *reply);
    private:
     // selected params
     QString strUrl;
QString strUrlName;
     QString strLogin;
     QString strPass;
     QString strId;
     Json::Value jsonLayerMeta;
     // gui
     QLineEdit *wEditUrl;
     QLineEdit *wEditLogin;
     QLineEdit *wEditPass;
     QPushButton *wButConnect;
     //QPushButton *wButCancel;
     QPushButton *wButSelect;
     QTreeWidget *wTree;
     QProgressBar *wProgBar;
     QLabel *wLabelStatus;
QCheckBox *chbGuest;
     QNetworkAccessManager httpManager;
     QNetworkReply *httpAuthReply;
     QNetworkReply *httpReply;
     QNetworkReply *httpResourceReply;
     QNetworkReply *httpSelectedReply;
     std::string receivedJson;
     std::map<int,int> itemTypes; // <item_id, item_type>
     QTreeWidgetItem *itemToExpand;
};


/**
 * Fields manager dialog.
 * Used to work with fields of the current project.
 */
class FBDialogFieldsManager: public QDialog
{
    Q_OBJECT
    public:
     FBDialogFieldsManager (QWidget *parent);
     ~FBDialogFieldsManager ();
     void loadFields (QMap<QString,FBField> fields);
     QMap<QString,FBField> getFields ();
     QSet<QString> getDeletedFields () { return fieldsDeleted; }
    private slots:
     void onAddClick ();
     void onRemoveClick ();
     void onItemClick (QTreeWidgetItem *item, int itemCol);
     int onShowAlertBox (QString msg, QMessageBox::Icon icon);
     void onShowMsgBox (QString msg, QMessageBox::Icon icon);
    private:
     QTreeWidget *tree;
     QLineEdit *editName;
     QComboBox *comboType;
     QToolButton *butAdd;
     QToolButton *butRemove;
     QPushButton *butOk;
     QPushButton *butCancel;
    private:
     FBProject *project;
     QSet<QString> fieldsDeleted; // keynames of fields are stored
};


/**
 * Progress dialog.
 * Shows progress bar.
 */
class FBDialogProgress: public QDialog
{
    Q_OBJECT
    public:
     FBDialogProgress (QWidget *parent);
     ~FBDialogProgress() { }
    public slots:
     void onChangeProgress (int value) { bar->setValue(value); }
    private:
     QProgressBar *bar;
};


/**
 * About dialog.
 * Shows about information.
 */
class FBDialogAbout: public QDialog
{
    Q_OBJECT
    public:
     FBDialogAbout(QWidget *parent);
     ~FBDialogAbout();
};


/**
 * Thread class for executing one action - "Save As".
 */
class FBThreadSaveAs: public QThread
{
    //friend class FBProject;
    Q_OBJECT
    public:
     FBThreadSaveAs (QObject *parent, QString strFullPath, FBProject *project,
                     Json::Value jsonForm);
    signals:
     void resultReady (FBErr err);
    private:
     void run ();// Q_DECL_OVERRIDE;
    private:
     QString strFullPath;
     FBProject *project;
     Json::Value jsonForm;
};


/**
 * App's main window class.
 * Aggregates all GUI of the app, except specific dialogues. Contains the working area
 * to which the screen with its form is rendered.
 */
class FB: public QWidget
{
    Q_OBJECT

    public:
    
     explicit FB (QWidget *parent = 0);
     ~FB();
     void initGui ();
     void setFbStyle ();

     void registerElements ();
     void deregisterElements ();

    private slots: // main gui slots ~ button slots

     void onAddElemPress (QTreeWidgetItem* item, int column);
     void onElemSelect ();
     void onNewVoidClick ();
     void onNewShapeClick ();
     void onNewNgwClick ();
     void onOpenClick ();
     void onSaveClick ();
     void onSaveAsClick ();
     void onScreenAndroidPick ();
     void onScreenIosPick ();
     void onScreenWebPick ();
     void onScreenQgisPick ();
     void onScreenDeviceSelect (int index);
     void onScreenStatePick ();
     void onUndoClick ();
     void onRedoClick ();
     void onClearScreenClick ();
     void onDeleteElemClick ();
     void onFieldsManagerClick ();
     void onImportControlsClick ();
     void onUpdateDataClick ();
     void onLeftArrowClick ();
     void onRightArrowClick ();
     void onLanguageSelect (int index);
     void onAboutGraphicsClick ();

     void keyPressEvent (QKeyEvent *);

    private slots: // other slots

     int onShowBox (QString msg, QString caption);
     void onShowInfo (QString msg);
     int onShowWarning (QString msg);
     void onShowError (QString msg);
     int onShowErrorFull (QString msgMain, FBErr err);
     bool onAskToLeaveUnsafeProject ();
     void onProjDialogFinished (int code);
     void onSaveAnyEnded (FBErr err);

    private: // methods

     // settings
     void writeSettings ();
     void readSettings ();

     // conversions
     QString getErrStr (FBErr err);
     QString getGroupStr (FBElemtype type);

     // gui top menu
     QToolButton *addTopMenuButton (QWidget *parentTab, QString imgPath, QString name,
                                    QString description, bool isSmall=false,
                                    bool withCaption=false, bool atTheEnd=false);
     void addTopMenuSplitter (QWidget *parentTab);
     void addTopMenuSpacer (QWidget *parentTab);
     QComboBox *addTopMenuCombo (QWidget *parentTab, QString caption,
                     QStringList values);
     QLabel *addTopMenuLabel (QWidget *parentTab, QString text, QString caption);

     // gui
     FBForm *createForm();
     void flipLeftMenu (bool isFull);
     void flipRightMenu (bool isFull);
     QTableWidget* addRightMenuTable ();
     void updateEnableness ();
     void updateLeftTrees ();
     void updateRightMenu ();
     void setBottomString (QString strToShorten, QString strToPrepend = "");
     void updateProjectString ();
     void updateDevices ();
     void updateStates ();
     void updateDeviceInfo ();
     void afterPickScreen (QToolButton *toolbDown);

     // screen
     void pickDefaultScreen ();
     void pickVoidScreen ();
     void recreateScreen (FBScreen *newScreen, bool destroyForm);

     // project
     bool newProjectCommonActions(FBProject *proj, QString path);
     void saveProjectCommonActions (QString ngfpFullPath);

    private: // fields

     bool isInited;

     // Settings.
     struct FBSetting
     {
         QString key;
         QString value;
         QString defaultValue;
     };
     FBSetting settLastShapeFullPath;
     FBSetting settLastNgfpFullPath;
     FBSetting settLastLanguageSelected;
     FBSetting settLastNgwUrl;
     FBSetting settLastNgwLogin;

     // Supported languages in program.
     struct FBLangInfo
     {
         QString name;
         QString code;
         QString imgFlagPath;
         QString imgNextgisPath;
         QString offLink;
     };
     QList<FBLangInfo> languages; // the order is important
     int indexLang; // index in this list, default is 0 = English

     // Current project of the app.
     // TODO: For future here can be an array of projects - but that requires changes
     // in FB behaviour and appearance.
     FBProject *project;

     // registrar
     QList<QPair<FBFctelem*,QString> > fctsElems; // first = factory, second = image path
     //QList<QPair<FBFctscreen*,QString> > fctsScreen;

     // main gui
     Ui::FB *ui;
     QVBoxLayout *lvAll;
     QHBoxLayout *lhMiddle;
    
     // top menu
     QTabWidget *tabMenuTop;
     QWidget *wProject;
     QWidget *wView;
     QWidget *wTools;
     QWidget *wSettings;
     QWidget *wAbout;
     QToolButton *toolbNewVoid;
     QToolButton *toolbNewShape;
     QToolButton *toolbNewNgw;
     QToolButton *toolbOpen;
     QToolButton *toolbSave;
     QToolButton *toolbSaveAs;
     QHBoxLayout *lhView;
     QToolButton *toolbScreenAndroid;
     QToolButton *toolbScreenIos;
     QToolButton *toolbScreenWeb;
     QToolButton *toolbScreenQgis;
     QComboBox *comboScreenDevice;
     QList<QToolButton*> toolbsScreenState;
     QWidget *wScreenInfo;
      QLabel *labScreenInfo1;
      QLabel *labScreenInfo2;
      QLabel *labScreenInfo3;
     QToolButton *toolbUndo;
     QToolButton *toolbRedo;
     QToolButton *toolbClearScreen;
     QToolButton *toolbDeleteElem;
     QToolButton *toolbImportControls;
     QToolButton *toolbUpdateData;
     QToolButton *toolbFieldManager;
     QComboBox *comboLang;
     QToolButton *toolbAboutGraphics;
     
     // left menu
     QWidget *wMenuLeft;
     QPushButton *butArrowLeft;
     QTreeWidget *treeLeftFull;
     QTreeWidget *treeLeftShort;
     
     // right menu
     QWidget *wMenuRight;
     QPushButton *butArrowRight;
     QVBoxLayout *lvRight; // for attr tables adding
     QList<QTableWidget*> tablesRight;
     QLabel *labRight;
     
     // working area
     QScrollArea *wWorkingArea;

     // screen
     FBScreen *wScreen;
          
     // other gui
     QLabel *labBottom;
     FBDialogProgress *dlgProgress;
};


#endif //FB_H
