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
#include <QProcess>

#include "project/project_core.h"
#include "form/form_core.h"
#include "screen/screen_core.h"

#include "ngw.h"

#include "my/user.h"

#include "project/projects.h"
#include "screen/screens.h"

#include "fbclickablelabel.h"

#define FB_PATH_TRANSLATIONS "../share/nextgis/fb/translations"
#define FB_PATH_MAINTAINER_WIN32 "\\..\\nextgisupdater.exe"

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

#define FB_MENURIGHT_TABLES_MAX 5
#define FB_BOTTOMSTRING_LEN_MAX 60

#define FB_SCREEN_SIZEFACTOR 130.0 // used for actual screen scale

#define FB_UTF8CHAR_LOCK "\xF0\x9F\x94\x92"

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
class FBDialogProjectNgw: public FBDialogNgw
{
    Q_OBJECT
    public:
     FBDialogProjectNgw (QWidget *parent, QString lastNgwUrl, QString lastNgwLogin);
     virtual ~FBDialogProjectNgw () { }
     QString getSelectedNgwResource (QString &strUrl, QString &strUrlName,
         QString &strLogin, QString &strPass, int &strId, Json::Value &jsonLayerMeta);
    protected slots:
     virtual void httpSelectedFinished ();
};

/**
 * NextGIS Web dialog for creating layer.
 */
class FBDialogLayerNgw: public FBDialogNgw
{
    Q_OBJECT
    public:
     FBDialogLayerNgw (QWidget *parent, QString lastNgwUrl, QString lastNgwLogin,
                       QMap<QString,FBField> fields, FbGeomType *geomType);
     virtual ~FBDialogLayerNgw () { }
     QString getSelectedNgwResource (QString &strUrl, QString &strUrlName,
         QString &strLogin, QString &strPass, int &strId, Json::Value &jsonLayerMeta);
    protected slots:
     void httpFinished ();
     void httpOnItemClicked(QTreeWidgetItem *treeItem, int treeItemColumn);
     void onSelectClicked ();
     virtual void httpSelectedFinished ();
     void showMsg (QString msg);
    protected:
     QLineEdit *leditName;
     QMap<QString,FBField> fields;
     FbGeomType *geomType;
     int groupId;
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
 * Lists dialog.
 *
 */
class FBDialogLists: public QDialog
{
    Q_OBJECT
    public:
     FBDialogLists (QWidget *parent);
     ~FBDialogLists() { }
     void loadLists (QList<QStringList> lists, QString keyList);
     QList<QStringList> getLists ();
     QString getKeyList ();
    protected slots:
     void onAddColumnClick ();
     void onRemoveColumnClick ();
     void onAddRowClick ();
     void onRemoveRowClick ();
     void onMakeKeyClick ();
    protected:
     void colorColumn (int index, QColor color);
    protected:
     QTableWidget *table;
     QToolButton *butAddCol;
     QToolButton *butRemoveCol;
     QToolButton *butAddRow;
     QToolButton *butRemoveRow;
     QToolButton *butMakeKey;
     QPushButton *butOk;
     int indexKeyList;
};
class FBDialogAddColumn: public QDialog
{
    Q_OBJECT
    public:
     FBDialogAddColumn (QWidget *parent, QStringList existedNames);
     ~FBDialogAddColumn() { }
     QString getName ();
    protected slots:
     void onOkClick ();
    protected:
     bool isNameCorrect (QString string);
     QLineEdit *edit;
     QPushButton *butOk;
     QStringList existedNames;
     QString badSymbols;
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

     void startInitialAuthentication ();

protected:

     void resizeEvent (QResizeEvent *event);

private slots:

     void onAddElemPress (QTreeWidgetItem* item, int column);
     void onElemSelect ();
     void onNewVoidClick ();
     void onNewShapeClick ();
     void onNewNgwClick ();
     void onOpenClick ();
     void onSaveClick ();
     void onSaveAsClick ();
     void onUploadClick ();
     void onListsClick ();
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
     void onUpdatesClick ();
     void onAuthClick ();
     void onLabAuthAnimClick ();
     void authorizeFinished ();
     void endCheckingUpdates (int exitCode, QProcess::ExitStatus exitStatus);
     void endMaintainerWork (int exitCode, QProcess::ExitStatus exitStatus);
     void keyPressEvent (QKeyEvent *);
     int onShowBox (QString msg, QString caption);
     void onShowInfo (QString msg);
     int onShowWarning (QString msg);
     void onShowError (QString msg);
     int onShowQuestion (QString msg);
     int onShowErrorFull (QString msgMain, FBErr err);
     bool onAskToLeaveUnsafeProject ();
     void onProjDialogFinished (int code);
     void onSaveAnyEnded (FBErr err);

private: // methods

     void startCheckingUpdates ();
     void writeSettings ();
     void readSettings ();
     QString getErrStr (FBErr err);
     QString getGroupStr (FBElemtype type);
     QToolButton *addTopMenuButton (QWidget *parentTab, QString imgPath, QString name,
                                    QString description, bool isSmall=false,
                                    bool withCaption=false, bool atTheEnd=false);
     void addTopMenuSplitter (QWidget *parentTab);
     void addTopMenuSpacer (QWidget *parentTab);
     QComboBox *addTopMenuCombo (QWidget *parentTab, QString caption,
                     QStringList values);
     QLabel *addTopMenuLabel (QWidget *parentTab, QString text, QString caption);
     FBForm *createForm();
     void flipLeftMenu (bool isFull);
     void flipRightMenu (bool isFull);
     void openSideMenus ();
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
     void updateAppTitle ();
     void pickDefaultScreen ();
     void pickVoidScreen ();
     void recreateScreen (FBScreen *newScreen, bool destroyForm);
     bool newProjectCommonActions (FBProject *proj, QString path);
     void saveProjectCommonActions (QString ngfpFullPath);
     bool isSaveRequired ();
     void closeEvent (QCloseEvent *event);
     void showMsgForNotSupported ();
     void updateAtUserChange ();
     void authorize (QString sLastAccessToken = "", QString sLastRefreshToken = "");
     void stopAuthorization ();
     void dropUserSettings ();
     void defineIfCanShowSupportInfo ();
     void showFullMessage (QString sMainText, QString sDetailedText);

private: // fields

     struct FBSetting // Settings.
     {
         QString key;
         QString value;
         QString defaultValue;
     };

     struct FBLangInfo // Supported languages in program.
     {
         QString name;
         QString code;
         QString imgFlagPath;
         QString imgNextgisPath;
         QString offLink;
         QString subscribeLink;
         QString callbakHtmlPath;
     };

     bool isInited;

     FBSetting settLastShapeFullPath;
     FBSetting settLastNgfpFullPath;
     FBSetting settLastLanguageSelected;
     FBSetting settLastNgwUrl;
     FBSetting settLastNgwLogin;

     QList<FBLangInfo> languages; // the order is important
     int indexLang; // index in this list, default is 0 = English

     FBProject *project; // Current project of the app.

     QProcess *prUpdatesCheck;
     QProcess *prMaintainer;

     QList<QPair<FBFctelem*,QString> > fctsElems; // registrar: first = factory, second = image path
     //QList<QPair<FBFctscreen*,QString> > fctsScreen;

     Ui::FB *ui;

     QVBoxLayout *lvAll;
     QHBoxLayout *lhMiddle;
     QTabWidget *tabMenuTop;
     QWidget *wProject;
     QWidget *wData;
     QWidget *wForm;
     QWidget *wView;
     //QWidget *wTools;
     QWidget *wSettings;
     QWidget *wAbout;
     QToolButton *toolbNewVoid;
     QToolButton *toolbNewShape;
     QToolButton *toolbNewNgw;
     QToolButton *toolbOpen;
     QToolButton *toolbSave;
     QToolButton *toolbSaveAs;
     QToolButton *toolbUpload;
     QToolButton *toolbLists;
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
//     QToolButton *toolbUndo;
//     QToolButton *toolbRedo;
     QToolButton *toolbClearScreen;
     QToolButton *toolbDeleteElem;
     QToolButton *toolbImportControls;
     QToolButton *toolbUpdateData;
     QToolButton *toolbFieldManager;
     QComboBox *comboLang;
     QToolButton *toolbAboutGraphics;
     QWidget *wPopup; //FbAuthWidget *wPopup;
     QToolButton *toolbUpdates;
     QToolButton *toolbAuth;
     FbClickableLabel *labAuthAnim;
     QMovie *movieAnim;
     QLabel *labAuth;
     QWidget *wMenuLeft;
     QPushButton *butArrowLeft;
     QTreeWidget *treeLeftFull;
     QTreeWidget *treeLeftShort;
     QWidget *wMenuRight;
     QPushButton *butArrowRight;
     QVBoxLayout *lvRight; // for attr tables adding
     QList<QTableWidget*> tablesRight;
     QLabel *labRight;
     QScrollArea *wWorkingArea;
     FBScreen *wScreen;
     QLabel *labBottom;
     FBDialogProgress *dlgProgress;
     QLabel *labUserText;
     QLabel *labUser;

     // TEMPORARY: for premium/non-premium use of the program.
     // TODO: change this and many other connected things during the big refactoring.
     FBSetting settLastAccessToken;
     FBSetting settLastRefreshToken;
     QScopedPointer<Nextgis::User> pUser;
     QStringList listPremiumElems;
     bool bShowSupportExpiredMessage;
     bool bForceOnlineAuth;
     // Note: some buttons are also hardcoded as for premium use.
};

#endif //FB_H
