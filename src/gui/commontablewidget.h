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

#include <QTableWidget>

namespace Fb
{
namespace Gui
{


const int CMNTBL_MAX_ROWS = 65535;
const int CMNTBL_MAX_STRSIZE = 255;
const QColor CMNTBL_TEXT_CLR = {0, 0, 0};
const QColor CMNTBL_NOTSEL_CLR = {255, 255, 255};
const QColor CMNTBL_DEF_CLR = {139, 183, 224};
const QColor CMNTBL_INPROW_CLR = {255, 255, 150};
const QString CMNTBL_DEF_STR = {QObject::tr("<default item>")};

/*!
 * @brief A common table widget for entering a list(s) of items with some restrictions. The table
 * has an obligatory row at the end (aka "input" row) which serves for entering new items.
 *
 * Note. To correctly use this table you should define the appropriate columns count in a
 * constructor and add rows with the help of specific methods. Do not add columns/rows in a
 * standard way.
 */
class CommonTableWidget: public QTableWidget
{
    Q_OBJECT

    public:

     explicit CommonTableWidget (QWidget *parent, const QStringList &column_names,
                                 int min_rows = 0, int max_rows = CMNTBL_MAX_ROWS,
                                 bool must_have_default_row = false, bool is_editable = true);
     virtual ~CommonTableWidget ();

     inline int getMinRows () const { return min_rows; }
     inline int getDefaultRow () const { return default_row; }
     int getRealRowCount () const;
     QStringList getHorizontalHeaderItems () const;

     void putItems(const QList<QStringList> &new_items, int new_default_row);
     void getItems (QList<QStringList> &ret_items, int &ret_default_row);

     void clearItems ();

     void addRowFromInputRow ();
     void removeSelectedRow ();
     void makeSelectedRowDefault ();

     void abortOperation () { abort_flag = true; } // abort adding or removing items

     void setMaxStrSize (int new_max_str_size);

    signals:

     void rowSelected ();
     void rowAdded ();
     void rowRemoved ();
     void rowChanged ();

     void cleared ();

     void beforeAddRow ();
     void beforeRemoveRow ();

    protected slots:

     virtual void keyPressEvent (QKeyEvent *event) override;
     void onCellChanged (int row, int col);
     void onItemSelectionChanged ();

    protected:

     void u_createInputRow ();
     void u_setDefaultRow (int new_default_row);
     void u_unsetDefaultRow ();
     void u_appendVoidRow ();
     void u_removeLastRow ();
     void u_setItem (int row, int col, QString text);
     void u_colorRow (int row, QColor color);
     bool u_isRowInRange (int row) const;
     bool u_isColInRange (int col) const;
     bool u_isInputRow (int row) const;
     bool u_isStringVoid (QString str) const;
     bool u_chopString (QString &str);
     bool u_isRowVoid (int row) const;
     bool u_isOneInRowVoid (int row) const;
     void u_completeRow (int row);
     void u_switchToInputRow ();

     void u_setDefaultEditTriggers ();
     bool u_commitAndClosePersistentEditor (QTableWidgetItem *item);

     int real_column_count;

     int min_rows;
     int max_rows;
     bool must_have_default_row;
     int max_str_size;

     bool is_input_row_created;
     int default_row;

     bool abort_flag;

     QAbstractItemView::EditTriggers edit_triggers;
};


}
}



