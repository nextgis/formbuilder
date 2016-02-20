/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  attributes declaration
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
#include <QCombobox>
#include <QLabel>
#include <QListWidget>
#include <QToolButton>
#include <QGroupBox>

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
     Json::Value toJson ();
     FBErr fromJson (Json::Value jsonVal);
     QWidget *getWidget ();
     void updateValues (QStringList newKeyNames);
    protected slots:
     void onEditEnd (QString keyNameSelected);
    private:
     QStringList keyNames;
     QString keyNameSelected; // not an index because we store string in json. It is
                              // safe because keyNames will be unique
};

class FBAttrText: public FBAttr
{
    Q_OBJECT
    public:
     FBAttrText (FBElem *parentElem, QString keyName, QString displayName,
                 FBAttrrole role, QString initValue);
     ~FBAttrText () { }
     Json::Value toJson ();
     FBErr fromJson (Json::Value jsonVal);
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
     FBErr fromJson (Json::Value jsonVal);
     QWidget *getWidget ();
    protected slots:
     void onEditEnd (int spinBoxValue);
    private:
     int value;
     int min;
     int max;
};

class FBAttrListvalues: public FBAttrDialog
{
    Q_OBJECT
    public:
     FBAttrListvalues (FBElem *parentElem, QString keyName, QString displayName,
                FBAttrrole role, QWidget *parentForDialog);
     virtual ~FBAttrListvalues () { }
     virtual Json::Value toJson ();
     virtual FBErr fromJson (Json::Value jsonVal);
     QString getDefDispValue ();
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
     FBDialogListvalues (QWidget *parent);//QString elemName);
     ~FBDialogListvalues() { }
     void putValues (QList<QPair<QString,QString> > values, int valueDefault);
     void getValues (QList<QPair<QString,QString> > &values, int &valueDefault);
     static QString shortenStr (QString str);
    private slots:
     void onOkClicked ();
     void onCancelClicked ();
     void onLeftClicked (QListWidgetItem *item);
     void onLeftAddClicked ();
     void onLeftRemoveClicked ();
     void onLeftChangeClicked ();
    private:
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


#endif //ATTRIBUTES_H




