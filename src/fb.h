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
#include <QDialog>
#include <QMessageBox>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QTabWidget>
#include <QTreeWidget>
#include <QTableWidget>
#include <QScrollArea>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "fb_general.h"
#include "project.h"
#include "screen.h"
//#include "factory.h"


// Constants and limits.
#define FB_MENURIGHT_TABLES_COUNT 3

// Different sizes for GUI.
#define FB_GUI_FONTSIZE_NORMAL 11
#define FB_GUI_FONTSIZE_SMALL 9


namespace Ui
{
    class FB;
}


/**
 *
 */
class FBDialogProjectNew: public QDialog
{
    Q_OBJECT

    public:
     FBDialogProjectNew (QWidget *parent);
     ~FBDialogProjectNew () {}

    //public slots:
     //virtual FBProject *exec ();

    private:
     QComboBox *comboGeom;
};

/**
 *
 */
class FBDialogProgress: public QDialog
{

};

/**
 *
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
     void onElemPress ();
     void onNewVoidClick ();
     void onNewShapeClick ();
     void onNewNgwClick ();
     void onOpenClick ();
     void onSaveClick ();
     void onSaveAsClick ();
     void onScreenStylePick ();
     void onScreenTypePick ();
     void onScreenRatioSelect (int index);
     void onScreenResolSelect (int index);
     void onUndoClick ();
     void onRedoClick ();
     void onClearScreenClick ();
     void onDeleteElemClick ();
     void onSettingLanguageSelect ();
     void onAboutGraphicsClick ();

     // other gui slots
     void onElemHighlight ();
     void onLeftArrowClick ();
     void onRightArrowClick ();
     void showInfo (QString msg);
     int showWarning (QString msg);
     void showError (QString msg);
     int showBox (QString msg, QString caption);
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
     void setRightMenuCaption (bool isElemSelected);
     void updateEnableness ();

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
     QList<QToolButton*> toolbsScreenStyle;
     QList<QToolButton*> toolbsScreenType;
     QComboBox *comboScreenRatio;
     QComboBox *comboScreenResol;
     QToolButton *toolbUndo;
     QToolButton *toolbRedo;
     QToolButton *toolbClearScreen;
     QToolButton *toolbDeleteElem;
     
     // left menu
     QWidget *wMenuLeft;
     QPushButton *butArrowLeft;
     QTreeWidget *treeLeftFull;
     QTreeWidget *treeLeftShort;
     
     // right menu
     QWidget *wMenuRight;
     QPushButton *butArrowRight;
     QVBoxLayout *vlRight; // for attr tables adding
     QList<QTableWidget*> tablesRight;
     QLabel *labRight;
     
     // working area
     FBWorkingArea *wWorkingArea;
          
     // other gui
     QLabel *labBottom;
};


/*
class FBGui
{
    public:
     FBGui(FB *fbPtr);
     virtual void arrangeWidgets () = 0;
};

class FBGuiButton: public FBGui, public QToolButton
{
    public:
     FBGuiButton(FB *fbPtr, QWidget parentTab, QString imgPath,
                 QString caption, QString hint);

};

class FBGuiSwitcherGroup: public FBGui
{
    public:

    protected:
     QList<FBGuiButton*> switchers;

};

class FBGuiCombo: public FBGui
{

};

class FBGuiTreeItem: public FBGui, public QTreeWidgetItem
{

};
*/


#endif //FB_H
