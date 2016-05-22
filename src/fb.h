/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  main app+gui class definitions
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

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>

#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QFileInfo>

#include <QThread>

#include <QSettings>

#include "form/form_core.h"
#include "project/project.h"
#include "screen/screen.h"

// Visual.
#define FB_GUI_FONTTYPE "Candara"
#define FB_GUI_FONTSIZE_NORMAL 11
#define FB_GUI_FONTSIZE_SMALL 9

// Constants and limits.
#define FB_MENURIGHT_TABLES_MAX 5
#define FB_BOTTOMSTRING_LEN_MAX 40

// NGW.
#define FB_NGW_ITEMTYPE_UNDEFINED 0
#define FB_NGW_ITEMTYPE_RESOURCEGROUP 1
#define FB_NGW_ITEMTYPE_VECTORLAYER 2
#define FB_NGW_ITEMTYPE_POSTGISLAYER 3

namespace Ui
{
    class FB;
}


/**
 * New project's dialog.
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
 * Used to select GeoJSON dataset on NGW server.
 * See NextGIS Web syntax how to get json data from NGW server in the methods
 * of this dialogue.
 */
class FBDialogProjectNgw: public QDialog
{
    Q_OBJECT

    public:

     FBDialogProjectNgw (QWidget *parent, QString lastNgwUrl, QString lastNgwLogin);
     ~FBDialogProjectNgw ();

     QString getSelectedNgwResource (QString &strUrl, QString &strLogin,
                 QString &strPass, int &strId, Json::Value &jsonLayerMeta);

    signals:

     void updateNgwSettings (QString lastUrl, QString lastLogin);

    private slots:

     void onConnectClicked ();
     void onSelectClicked ();

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
 * App's window class. Aggregates all GUI of the app, except specific dialogues.
 *
 * Contains the working area to which the screen with its form is rendered.
 */
class FB: public QWidget
{
    Q_OBJECT

    public:
    
     explicit FB (QWidget *parent = 0);
     ~FB();
     void initGui ();
     void setFbStyle ();

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
