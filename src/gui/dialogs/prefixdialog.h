/**************************************************************************************************
 *                                                                                                *
 *    Project:  NextGIS Formbuilder                                                               *
 *    Authors:  Mikhail Gusev, gusevmihs@gmail.com                                                *
 *              Copyright (C) 2014-2019 NextGIS                                                   *
 *                                                                                                *
 *    This program is free software: you can redistribute it and/or modify it under the terms     *
 *    of the GNU General Public License as published by the Free Software Foundation, either      *
 *    version 2 of the License, or (at your option) any later version.                            *
 *                                                                                                *
 *    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;   *
 *    without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   *
 *    See the GNU General Public License for more details.                                        *
 *                                                                                                *
 *    You should have received a copy of the GNU General Public License along with this program.  *
 *    If not, see http://www.gnu.org/licenses/.                                                   *
 *                                                                                                *
 **************************************************************************************************/

#pragma once

#include "gui/custom_dialog.h"

#include <QTableWidget>
#include <QToolButton>
#include <QPushButton>
#include <QLineEdit>

namespace Fb
{
namespace Gui
{


class PrefixColumnDialog: public QDialog
{
    Q_OBJECT
    public:
     PrefixColumnDialog (QWidget *parent, QStringList existedNames);
     ~PrefixColumnDialog ();
     QString getName ();
    protected slots:
     void onOkClick ();
    protected:
     bool isNameCorrect (QString string);
     QLineEdit *edit;
     QPushButton *butOk;
     QStringList existedNames;
     QString badSymbols;
     const int FB_LISTS_MAX_FIELDNAMELENGTH = 32;
};


class PrefixDialog: public QDialog
{
    Q_OBJECT
    public:
     PrefixDialog (QWidget *parent);
     ~PrefixDialog ();
     void loadLists (QList<QStringList> lists, QString keyList);
     QList<QStringList> getLists ();
     QString getKeyList ();
    protected slots:
     void onAddColumnClick ();
     void onRemoveColumnClick ();
     void onAddRowClick ();
     void onRemoveRowClick ();
     void onMakeKeyClick ();
     void onLoadCsvClick ();
    protected:
     void colorColumn (int index, QColor color);
    protected:
     QTableWidget *table;
     QToolButton *butAddCol;
     QToolButton *butRemoveCol;
     QToolButton *butAddRow;
     QToolButton *butRemoveRow;
     QToolButton *butMakeKey;
     QToolButton *butLoadCsv;
     QPushButton *butOk;
     int indexKeyList;

    const int FB_LISTS_MAX_COLUMNS = 64;
    const int FB_LISTS_MAX_ROWS = 1024;
    const QColor FB_LISTS_COLOR_DEFAULT = {255,255,255};
    const QColor FB_LISTS_COLOR_KEY_COLUMN = {139,183,224};
};


}
}



