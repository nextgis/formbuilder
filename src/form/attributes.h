/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  various attributes declaration
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

#include "form_core.h"

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

#define FB_ATTRLIMIT_LISTVALUES_MAXCOUNT 65535
#define FB_ATTRLIMIT_STRDISPLAY_MAXSIZE 23


// Abstract class for attributes with complex values which must be set via specific
// dialogues.
class FBAttrDialog: public FBAttr
{
    Q_OBJECT
    public:
     FBAttrDialog (FBElem *parentElem, QString keyName, QString displayName,
            FBAttrrole role, QWidget *parentForDialog);
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
                 FBAttrrole role);
     ~FBAttrField () { }
     static void updateValues (QStringList newKeyNames);
     Json::Value toJson ();
     bool fromJson (Json::Value jsonVal);
     QWidget *getWidget ();
     QString getValue () { return keyNameSelected; }
     void resetValue ();
    protected:
     static QStringList keyNames; // fields are common for all Field attributes
    protected slots:
     void onEditEnd (QString keyNameSelected);
    private:
     QString keyNameSelected; // not an index because we store string in json. It is
                              // safe because keynames are unique
};

class FBAttrText: public FBAttr
{
    Q_OBJECT
    public:
     FBAttrText (FBElem *parentElem, QString keyName, QString displayName,
                 FBAttrrole role, QString initValue);
     ~FBAttrText () { }
     Json::Value toJson ();
     bool fromJson (Json::Value jsonVal);
     QWidget *getWidget ();
     QString getValue () { return value; }
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
                 FBAttrrole role, int initValue, int min, int max);
     ~FBAttrNumber () { }
     Json::Value toJson ();
     bool fromJson (Json::Value jsonVal);
     QWidget *getWidget ();
     int getValue () { return value; }
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
                 FBAttrrole role, bool initValue);
     ~FBAttrBoolean () { }
     Json::Value toJson ();
     bool fromJson (Json::Value jsonVal);
     QWidget *getWidget ();
     bool getValue () { return value; }
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
                FBAttrrole role, QWidget *parentForDialog);
     virtual ~FBAttrListvalues () { }
     virtual Json::Value toJson ();
     virtual bool fromJson (Json::Value jsonVal);
     QString getDefDispValue ();
     QStringList getDispValues ();
     int getDefIndex () { return valueDefault; }
    protected slots:
     virtual void onEditStart ();
    protected:
     QList<QPair<QString,QString> > values; // first = key name, second = display name
     int valueDefault;
};
class FBDialogListvalues: public QDialog
{
    Q_OBJECT
    public:
     FBDialogListvalues (QWidget *parent, bool addUndefinedValue=true);
     ~FBDialogListvalues() { }
     void putValues (QList<QPair<QString,QString> > values, int valueDefault);
     void getValues (QList<QPair<QString,QString> > &values, int &valueDefault);
    private slots:
     void onOkClicked ();
     void onCancelClicked ();
     void onLeftClicked (QListWidgetItem *item);
     void onLeftAddClicked ();
     void onLeftRemoveClicked ();
     void onLeftChangeClicked ();
    private:
     static QString shortenStr (QString str);
    private:
     bool hasUndefinedValue;
     QString elemName;
     QListWidget *listL;
     QToolButton *butAddL; // +
     QToolButton *butRemoveL; // -
     QToolButton *butChangeL; // #
     QLineEdit *editInnerL;
     QLineEdit *editOuterL;
     QComboBox *comboL;
     QPushButton *butOk;
};

class FBAttrListvaluesStrict: public FBAttrListvalues
{
    Q_OBJECT
    public:
     FBAttrListvaluesStrict (FBElem *parentElem, QString keyName, QString displayName,
               FBAttrrole role, QWidget *parentForDialog);
     virtual ~FBAttrListvaluesStrict () { }
    protected slots:
     virtual void onEditStart ();
};

class FBAttrListvaluesDouble: public FBAttrListvalues
{
    Q_OBJECT
    public:
     FBAttrListvaluesDouble (FBElem *parentElem, QString keyName, QString displayName,
                FBAttrrole role, QWidget *parentForDialog);
     virtual ~FBAttrListvaluesDouble () { }
     virtual Json::Value toJson ();
     virtual bool fromJson (Json::Value jsonVal);
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
                 FBAttrrole role, QStringList valuesRange, int initValue);
     ~FBAttrSelect () { }
     Json::Value toJson ();
     bool fromJson (Json::Value jsonVal);
     QWidget *getWidget ();
     int getValue () { return value; }
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
                FBAttrrole role, QWidget *parentForDialog);
     virtual ~FBAttrDatetime () { }
     virtual Json::Value toJson ();
     virtual bool fromJson (Json::Value jsonVal);
     QString getValueString ();
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




