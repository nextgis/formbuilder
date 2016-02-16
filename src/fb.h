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
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QTabWidget>
#include <QTreeWidget>
#include <QTableWidget>
#include <QScrollArea>
#include <QToolButton>

#include <QFileInfo>

#include "fb_common.h"
#include "form/form_core.h"
#include "project/project.h"
#include "screen.h"

// Constants and limits.
#define FB_MENURIGHT_TABLES_MAX 5
#define FB_BOTTOMSTRING_LEN_MAX 40

// Different sizes for GUI.
#define FB_GUI_FONTSIZE_NORMAL 11
#define FB_GUI_FONTSIZE_SMALL 9


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
     QString getSelectedGeom () { return comboGeom->currentText(); }
    private:
     QComboBox *comboGeom;
};

/**
 * Progress dialog.
 */
class FBDialogProgress: public QDialog
{
    public:
     FBDialogProgress (QWidget *parent);
     ~FBDialogProgress();
};

/**
 * About dialog.
 */
class FBDialogAbout: public QDialog
{

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

    private slots:

     // main gui slots
     void onAddElemPress (QTreeWidgetItem* item, int column);
     void onNewVoidClick ();
     void onNewShapeClick ();
     void onNewNgwClick ();
     void onOpenClick ();
     void onSaveClick ();
     void onSaveAsClick ();
     void onScreenPick ();
     void onScreenStatePick ();
     void onScreenDeviceSelect (int index);
     void onUndoClick ();
     void onRedoClick ();
     void onClearScreenClick ();
     void onDeleteElemClick ();
     void onSettingLanguageSelect ();
     void onAboutGraphicsClick ();

     // other gui slots
     void onElemSelect ();
     void onLeftArrowClick ();
     void onRightArrowClick ();
     int showBox (QString msg, QString caption);
     void showInfo (QString msg);
     int showWarning (QString msg);
     void showError (QString msg);
     int showErrorFull (QString msgMain, FBErr err);
     bool askToLeaveUnsafeProject ();

    private: // methods

     // gui
     QToolButton *addTopMenuButton (QWidget *parentTab, QString imgPath,
                           QString name, QString description, bool small=false);
     void addTopMenuSplitter (QWidget *parentTab);
     QComboBox *addTopMenuCombo(QWidget *parentTab, QString caption,
                     QStringList values);
     void flipLeftMenu (bool isFull);
     void flipRightMenu (bool isFull);
     QTableWidget* addRightMenuTable (int rowCount);
     void updateEnableness ();
     void updateLeftTrees ();
     void updateRightMenu ();
     void updateMenuView ();
     void setBottomString (QString strToShorten, QString strToPrepend = "");
     void updateProjectString ();
     void finishProjDialog ();

     // settings
     void updateSettings ();
     QString getSettingLastPath ();

     //errors
     QString getErrString (FBErr err);

    private: // fields

     // Current project of the app.
     // For future here can be an array of projects - but that requires changes
     // in FB behaviour and appearance.
     FBProject *project;

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
     QToolButton *toolbNewVoid;
     QToolButton *toolbNewShape;
     QToolButton *toolbNewNgw;
     QToolButton *toolbOpen;
     QToolButton *toolbSave;
     QToolButton *toolbSaveAs;
     QHBoxLayout *lhView;
     QList<QToolButton*> toolbsScreen;
     QList<QToolButton*> toolbsScreenState;
     QComboBox *comboScreenDevice;
     QWidget *wScreenInfo;
     QToolButton *toolbUndo;
     QToolButton *toolbRedo;
     QToolButton *toolbClearScreen;
     QToolButton *toolbDeleteElem;
     QToolButton *toolbImportControls;
     QToolButton *toolbUpdateData;
     QToolButton *toolbFieldManager;
     
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
     FBScreen *wScreen;
          
     // other gui
     QLabel *labBottom;
     FBDialogProgress *dlgProgress;
};


#endif //FB_H
