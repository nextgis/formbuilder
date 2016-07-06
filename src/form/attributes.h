/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  various attributes
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

#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QListWidget>
#include <QToolButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QTableWidget>
#include <QDateTime>
#include <QMessageBox>
#include <QHeaderView>

#include "form_core.h"
#include "ngw.h"

#define FB_ATTRLIMIT_LISTVALUES_MAXCOUNT 1024
#define FB_ATTRLIMIT_LISTVALUE_MAXLENGTH 255
#define FB_ATTRLIMIT_STRDISPLAY_MAXSIZE 23


// Abstract class for attributes with complex values which must be set via specific
// dialogues.
class FBAttrDialog: public FBAttr
{
    Q_OBJECT
    public:
     FBAttrDialog (FBElem *parentElem, QString keyName, QString displayName,
                   QString descr, FBAttrrole role, QWidget *parentForDialog);
     virtual ~FBAttrDialog () { }
     virtual QPushButton *getWidget ();
    protected slots:
     virtual void onEditStart () = 0;
    protected:
     QWidget *parentForDialog;
};

class FBAttrField: public FBAttr
{
    Q_OBJECT
    public:
     FBAttrField (FBElem *parentElem, QString keyName, QString displayName,
                  QString descr, FBAttrrole role);
     ~FBAttrField () { }
     static void updateValues (QStringList newKeyNames);
     Json::Value toJson ();
     bool fromJson (Json::Value jsonVal);
     QWidget *getWidget ();
     QVariant getValue () { return keyNameSelected; }
     void resetValue ();
    protected:
     static QStringList keyNames; // fields are common for all Field attributes
    protected slots:
     void onEditEnd (QString keyNameSelected);
    private:
     QString keyNameSelected; // not an index because we store string in json. It is
                              // safe because keynames are unique
};

class FBAttrString: public FBAttr
{
    Q_OBJECT
    public:
     FBAttrString (FBElem *parentElem, QString keyName, QString displayName,
                 QString descr, FBAttrrole role, QString initValue);
     ~FBAttrString () { }
     Json::Value toJson ();
     bool fromJson (Json::Value jsonVal);
     QWidget *getWidget ();
     QVariant getValue () { return value; }
    protected slots:
     void onEditEnd (QString lineEditText);
    private:
     QString value;
};

class FBAttrNumber: public FBAttr
{
    Q_OBJECT
    public:
     FBAttrNumber (FBElem *parentElem, QString keyName, QString displayName,
                 QString descr, FBAttrrole role, int initValue, int min, int max);
     ~FBAttrNumber () { }
     Json::Value toJson ();
     bool fromJson (Json::Value jsonVal);
     QWidget *getWidget ();
     QVariant getValue () { return value; }
    protected slots:
     void onEditEnd (int spinBoxValue);
    private:
     int value;
     int min;
     int max;
};

class FBAttrBoolean: public FBAttr
{
    Q_OBJECT
    public:
     FBAttrBoolean (FBElem *parentElem, QString keyName, QString displayName,
                 QString descr, FBAttrrole role, bool initValue);
     ~FBAttrBoolean () { }
     Json::Value toJson ();
     bool fromJson (Json::Value jsonVal);
     QWidget *getWidget ();
     QVariant getValue () { return value; }
    protected slots:
     void onEditEnd (int checkBoxValue);
    private:
     bool value;
};


class FBAttrListvalues: public FBAttrDialog
{
    Q_OBJECT
    public:
     FBAttrListvalues (FBElem *parentElem, QString keyName, QString displayName,
                QString descr, FBAttrrole role, QWidget *parentForDialog);
     virtual ~FBAttrListvalues () { }
     static void updateNgwParams (QString curNgwUrl, QString curNgwLogin,
                                  QString curNgwPass);
     virtual Json::Value toJson ();
     virtual bool fromJson (Json::Value jsonVal);
     QVariant getValue ();
     QString getDefDispValue ();
     QStringList getDispValues ();
     int getDefIndex () { return valueDefault; }
     int getNgwLookupId () { return ngwLookupId; }
     void setNgwLookupId (int id) { ngwLookupId = id; }
    protected slots:
     virtual void onEditStart ();
    protected:
     QList<QPair<QString,QString> > values; // first = key name, second = display name
     int valueDefault;
     static QString curNgwUrl;
     static QString curNgwLogin;
     static QString curNgwPass;
     int ngwLookupId; // corresponds to those in FBDialogListvalues. See dialog class
};
class FBTableDialoglistvalues: public QTableWidget
{
    Q_OBJECT
    signals:
     void keyLastEnterPressed ();
     void keyNotLastDeletePressed ();
     void keyNotLastEnterPressed ();
    public:
     FBTableDialoglistvalues (QWidget *parent);
     ~FBTableDialoglistvalues () { }
     void setDefaultEditTriggers () { this->setEditTriggers(editTrigs); }
    private slots:
     void keyPressEvent (QKeyEvent *event);
    private:
     bool commitAndClosePersistentEditor (QTableWidgetItem* item);
    private:
     QAbstractItemView::EditTriggers editTrigs;
};
class FBDialogListvalues: public QDialog
{
    Q_OBJECT
    public:
     FBDialogListvalues (QWidget *parent, bool addUndefinedValue=true);
     ~FBDialogListvalues () { }
     void putValues (QList<QPair<QString,QString> > values, int valueDefault);
     void getValues (QList<QPair<QString,QString> > &values, int &valueDefault);
     void putNgwParams (QString url, QString login, QString pass, int id);
     void getNgwParams (int &id);
    private slots:
     void onTableSelectionChanged ();
     void onCellChanged(int,int);
     void onClearAllClicked ();
     int onLoadNgwClicked ();
     void onLoadNgwSyncClicked ();
     void onLoadCsvClicked ();
     void onOkClicked () { this->accept(); }
     void onCancelClicked () { this->reject(); }
     int onShowAlertBox (QString msg, QMessageBox::Icon icon);
     void onShowMsgBox (QString msg, QMessageBox::Icon icon);
     void keyPressEvent (QKeyEvent *event);
    private slots:
     void onAdd ();
     void onDelete ();
     void onDefault ();
    private:
     void appendRow ();
     void addEnterRow ();
     //void removeEnterRow ();
     void clearAllRows ();
     void completeRow (int row);
     void unmarkDefaultRow ();
     void markDefaultRow (int row);
//     void updateDefaultButton (QTableWidgetItem *selectedItem);
//     void updateItemButtons (bool enable);
     void switchToEnterRow ();
     void updateNgwLabel ();
     void updateTableEnableness ();
     bool isRowVoid (int row);
     bool isOneInRowVoid (int row);
     bool isItemVoid (QTableWidgetItem *item);
     bool isStringVoid (QString str);
     //static QString shortenStr (QString str);
     void formatString (QString &str);
     void formatList (QList<QPair<QString,QString> > &values, int &valueDefault);
    private:
     bool hasUndefinedValue;
     int rowDefault;
     int ngwLookupId; // -1 means no ngw connection and no synchronisation
     QString ngwUrl; // "" means that ngw-lookup dialog can not be called
     QString ngwLogin;
     QString ngwPass;
     QString lastCsvPath;
    private:
     FBTableDialoglistvalues *table;
     QToolButton *butClearAll;
     QToolButton *butLoadNgw;
     QToolButton *butLoadNgwSync;
     QToolButton *butLoadCsv;
     QLabel *labNgw;
     QPushButton *butOk;
};
class FBDialogLookupNgw: public FBDialogNgw
{
    Q_OBJECT
    public:
     FBDialogLookupNgw (QWidget *parent, QString curNgwUrl, QString curNgwLogin,
                             QString curNgwPass);
     virtual ~FBDialogLookupNgw () { }
     int getSelectedLookupTable (QList<QPair<QString,QString> > &list);
    protected slots:
     virtual void httpSelectedFinished ();
    protected:
     QList<QPair<QString,QString> > selectedLookup;
};
class FBDialogCsv: public QDialog
{
    Q_OBJECT
    public:
     FBDialogCsv (QWidget *parent, QString pathCsv);
     ~FBDialogCsv () { }
     bool loadValues (QString &errString);
     bool getSelectedValues (QList<QPair<QString,QString> > &values,
                             QString &errString);
    private slots:
     void onInnerClicked ();
     void onDisplayedClicked ();
     void onOkClicked () { this->accept(); }
     void onCancelClicked () { this->reject(); }
    private:
     void anyButtonClick (bool isInner);
    private:
     QByteArray baPathCsv;
     int iInner;
     int iDisplayed;
    private:
     QListWidget *listMain;
     QPushButton *butInner;
     QPushButton *butDisplayed;
     QLabel *labInner;
     QLabel *labDisplayed;
     QPushButton *butOk;
};


class FBAttrListvaluesStrict: public FBAttrListvalues
{
    Q_OBJECT
    public:
     FBAttrListvaluesStrict (FBElem *parentElem, QString keyName, QString displayName,
               QString descr, FBAttrrole role, QWidget *parentForDialog);
     virtual ~FBAttrListvaluesStrict () { }
    protected slots:
     virtual void onEditStart ();
};

class FBAttrListvaluesDouble: public FBAttrListvalues
{
    Q_OBJECT
    public:
     FBAttrListvaluesDouble (FBElem *parentElem, QString keyName, QString displayName,
                QString descr, FBAttrrole role, QWidget *parentForDialog);
     virtual ~FBAttrListvaluesDouble () { }
     virtual Json::Value toJson ();
     virtual bool fromJson (Json::Value jsonVal);
     QVariant getValue ();
     void getDefDispValues (QString &str1, QString &str2);
    protected slots:
     virtual void onEditStart ();
    protected:
     QList<QList<QPair<QString,QString> > > values2;
     QList<int> valuesDefault2;
};
class FBDialogDlistvalues: public QDialog
{
    Q_OBJECT
    public:
     FBDialogDlistvalues (QWidget *parent);
     ~FBDialogDlistvalues () { }
    public:
     void putValues (QList<QPair<QString,QString> > vs, int vDef,
                     QList<QList<QPair<QString,QString> > > vs2,
                     QList<int> vsDef2);
     void getValues (QList<QPair<QString,QString> > &vs, int &vDef,
                     QList<QList<QPair<QString,QString> > > &vs2,
                     QList<int> &vsDef2);
    private slots:
     void onOkClicked ();
     void onTable1SelectionChanged ();
     void onTable2SelectionChanged ();
     void onCell1Changed (int row, int col);
     void onCell2Changed (int row, int col);
    private:
     void addTable2 ();
     void removeTable2 (int row);
     void showTable2 (int row);
     void showMsgBox (QString msg);
     QString getTable2String (QString srcStr);
     static QString shortenStringForOutput (QString inStr);
    private:
     QVBoxLayout *vlTable2;
     QHBoxLayout *hlCombo2;
     QTableWidget *table1;
     QComboBox *combo1;
     QLabel *label2;
     QList<QTableWidget*> tables2;
     QList<QComboBox*> combos2;
     QPushButton *butOk;
     // Additional pointers:
     QTableWidget *curTable2Ptr;
     QComboBox *curCombo2Ptr;
};

class FBAttrSelect: public FBAttr
{
    Q_OBJECT
    public:
     FBAttrSelect (FBElem *parentElem, QString keyName, QString displayName,
         QString descr, FBAttrrole role, QStringList valuesRange, int initValue);
     ~FBAttrSelect () { }
     Json::Value toJson ();
     bool fromJson (Json::Value jsonVal);
     QWidget *getWidget ();
     QVariant getValue () { return value; }
    protected slots:
     void onEditEnd (int indexSelected);
    private:
     QStringList valuesRange;
     int value;
};

class FBAttrDatetime: public FBAttrDialog
{
    Q_OBJECT
    public:
     FBAttrDatetime (FBElem *parentElem, QString keyName, QString displayName,
                QString descr, FBAttrrole role, QWidget *parentForDialog);
     virtual ~FBAttrDatetime () { }
     virtual Json::Value toJson ();
     virtual bool fromJson (Json::Value jsonVal);
     QVariant getValue ();
     void changeFormat (FBDatetimeFormat *newFormat);
    protected slots:
     virtual void onEditStart ();
    protected:
     QDateTime value;
     bool ignoreValue; // can be used to indicate a "current date/time" value
     FBDatetimeFormat *format; // for now not obligatory here. See "Date&Time" elem
};
class FBDialogDatetime: public QDialog
{
    Q_OBJECT
    public:
     FBDialogDatetime (QWidget *parent);
     ~FBDialogDatetime () {}
     void putValues (QDateTime value, bool ignoreValue, FBDatetimeFormat *format);
     void getValues (QDateTime &value, bool &ignoreValue);
    private slots:
     void onCheckboxClicked (bool isChecked);
    private:
     QDateTimeEdit *wDateTime;
     QCheckBox *chbCurrent;
};

#endif //ATTRIBUTES_H


