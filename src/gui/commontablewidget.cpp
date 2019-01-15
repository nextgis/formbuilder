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

#include "commontablewidget.h"

#include <QHeaderView>
#include <QKeyEvent>

using namespace Fb;
using namespace Gui;


/**
* @brief Constructor.
* @param column_names The amount and names of the columns which must be created for the table. Must
* not be void.
* @param min_rows A restriction of minimum rows count.
* @param max_rows A restriction of maximum rows count. Must not be less than min_rows but can be
* equal.
* @param must_have_default A restriction which says that this table must or must not have a default
* row always marked. If must_have_default set to true be sure that min_rows is not 0.
*/
CommonTableWidget::CommonTableWidget (QWidget *parent, const QStringList &column_names,
                                      int min_rows, int max_rows, bool must_have_default_row,
                                      bool is_editable):
    QTableWidget(parent),
    min_rows(min_rows),
    max_rows(max_rows),
    must_have_default_row(must_have_default_row),
    default_row(-1),
    is_input_row_created(false),
    abort_flag(false),
    max_str_size(CMNTBL_MAX_STRSIZE)
{
    if (column_names.size() == 0)
        throw; // TODO: named exception
    if (min_rows < 0 || max_rows < 0 || max_rows == 0 || min_rows > max_rows)
        throw; // TODO: named exception
    if (must_have_default_row && min_rows == 0)
        throw; // TODO: named exception

    real_column_count = column_names.size();

    setColumnCount(real_column_count);
    setRowCount(0);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    horizontalHeader()->setStretchLastSection(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setHorizontalHeaderLabels(column_names);

    setStyleSheet("QTableWidget {selection-background-color: rgb(230,230,230); "
                  "selection-color: black;}");

    connect(this, &QTableWidget::cellChanged,
            this, &CommonTableWidget::onCellChanged);
    connect(this, &QTableWidget::itemSelectionChanged,
            this, &CommonTableWidget::onItemSelectionChanged);

//    edit_triggers = editTriggers();

    blockSignals(true); // so e.g. not to trigger cellChanged() signal

    // Add rows if if there is min_rows restriction.
    if (min_rows != 0)
    {
        setRowCount(min_rows);
        for (int r = 0; r < min_rows; r++)
            for (int c = 0; c < real_column_count; c++)
                u_setItem(r, c, CMNTBL_DEF_STR);
    }

    // Set default_row if there is must_have_default restriction.
    if (must_have_default_row)
        u_setDefaultRow(0);

    // Add obligatory "input" row at the end.
    if (is_editable)
        u_createInputRow();

    blockSignals(false);

    // The following line added because there is no any word about what are the defaults for this in
    // the Qt docs:
    edit_triggers = editTriggers();
}

/**
* @brief Destructor.
*/
CommonTableWidget::~CommonTableWidget ()
{
}


/**
* @brief Return row count depends on whether "input" row is created.
*/
int CommonTableWidget::getRealRowCount () const
{
    return is_input_row_created ? rowCount() - 1 : rowCount();
}

/**
* @brief Get the list of column headers of this table.
*/
QStringList CommonTableWidget::getHorizontalHeaderItems () const
{
    QStringList list;
    for (int i = 0; i < horizontalHeader()->count(); i++)
        list.append(horizontalHeaderItem(i)->text());
    return list;
}


/**
* @brief Clear table and load new list of items with the default row.
* @param items The size of the passed QList should be equal to the amount of columns in the table.
* The size of string lists must be equal (will be checked by the first string list) and must not be
* more than max_rows restriction and less than min_rows.
*/
void CommonTableWidget::putItems (const QList<QStringList> &new_items, int new_default_row)
{
    if (new_items.size() != real_column_count)
        throw tr("Loaded list of items must have %1 columns").arg(real_column_count);
    if (new_items[0].size() < min_rows)
        throw tr("Loaded list of items must have minimum %1 rows").arg(min_rows);
    if (new_items[0].size() > max_rows)
        throw tr("Loaded list of items must have not more than %1 rows").arg(max_rows);
    for (auto str_list: new_items)
        if (str_list.size() != new_items[0].size())
            throw tr("Loaded list of items must have equal number of rows for each column");

    // We do not care about the "input" row. Just firstly remove all rows and after that recreate
    // the "input" row. As well for the default row.
    setRowCount(0);
    setRowCount(new_items[0].size());

    blockSignals(true); // so e.g. not to trigger cellChanged()

    // Write each string list to the table as a column.
    for (int c = 0; c < real_column_count; c++)
        for (int r = 0; r < rowCount(); r++)
            u_setItem(r, c, new_items[c][r]);

    // Add "input" row at the end.
    is_input_row_created = false;
    u_createInputRow();

//    blockSignals(false);

    // Set default row.
    if (!u_isRowInRange(new_default_row))
        if (must_have_default_row)
            new_default_row = 0;
        else
            new_default_row = -1;
    if (new_default_row == -1)
        u_unsetDefaultRow();
    else
        u_setDefaultRow(new_default_row);

    blockSignals(false);
}

/**
* @brief Return a list of items and the default row.
* @param items This list will be cleared and filled with items. Its size will be equal to the
* amount of columns in the table.
* @param get_from_input_row If true the items from the "input" row will be also added to the list.
*/
void CommonTableWidget::getItems (QList<QStringList> &ret_items, int &ret_default_row)
{
    ret_items.clear();

    // Determine if we should also get the items from the last "input" row. If this row is partly
    // void we should complete it.
    int row_count = rowCount();
    if (u_isRowVoid(row_count - 1) || // "input" row is void
        row_count - 1 == max_rows) // there is already a maximum amount of items
        row_count--;
    else
        u_completeRow(row_count - 1);

    // Get items. Fill fully void lines (we could delete them but we do not want to make min_rows
    // checks here).
    for (int c = 0; c < real_column_count; c++)
    {
        QStringList list;
        for (int r = 0; r < row_count; r++)
        {
            QString str = item(r, c)->text();
            if (u_isStringVoid(str))
                str = CMNTBL_DEF_STR;
            list.append(str);
        }
        ret_items.append(list);
    }

    ret_default_row = default_row;
}


/**
* @brief Clear the table regarding restrictions.
*/
void CommonTableWidget::clearItems ()
{
    if (getRealRowCount() == 0 || getRealRowCount() <= min_rows)
        return;

    // Remove all rows except its minimum count and the "input" row.
    while (getRealRowCount() > min_rows)
        u_removeLastRow();

    // Adjust the default row if we see that we have deleted it.
    if (default_row >= getRealRowCount())
        if (must_have_default_row)
            u_setDefaultRow(0); // must_have_default_row assumes that there are definetly some rows
        else
            u_unsetDefaultRow();

    u_switchToInputRow();

    emit cleared();
}


/**
* @brief Add items from "input" row once.
*/
void CommonTableWidget::addRowFromInputRow ()
{
    // Anyway close persistant editor with saving data in the cell.
    QTableWidgetItem *item_selected = currentItem();
    if (item_selected != nullptr)
        u_commitAndClosePersistentEditor(item_selected);

    if (!is_input_row_created)
        return;
    if (getRealRowCount() == max_rows)
        return;
    if (u_isRowVoid(rowCount() - 1)) // return if the "input" row is fully void
        return;

    emit beforeAddRow();
    if (abort_flag)
    {
        abort_flag = false;
        return;
    }

    blockSignals(true); // so at least not to trigger cellChanged()

    // Insert new row at the end but before the "input" row, and cut&paste the items from it to the
    // newly inserted row. Note: after this we do not need to change the default row because the
    // new row is always added to the end.
    int last_row = rowCount() - 1;
    insertRow(last_row);
    for (int c = 0; c < real_column_count; c++)
    {
        QString text = item(last_row + 1, c)->text();
        u_setItem(last_row, c, text);
        item(last_row + 1, c)->setText("");
    }

//    blockSignals(false);

    scrollToBottom();
    u_switchToInputRow();

    blockSignals(false);

    emit rowAdded();
}

/**
* @brief Remove current row of this table.
*/
void CommonTableWidget::removeSelectedRow ()
{
    if (getRealRowCount() == min_rows)
        return;

    QTableWidgetItem *cur_item = currentItem();
    if (cur_item == nullptr)
        return;

    int cur_row = cur_item->row();
    if (is_input_row_created && cur_row == rowCount() - 1) // we are in the "input" row
        return;

    emit beforeRemoveRow ();
    if (abort_flag)
    {
        abort_flag = false;
        return;
    }

    blockSignals(true); // so at least not to trigger itemSelectionChanged()

    // Remove the row.
    removeRow(cur_row);

    // Adjust the default row if we see that we have deleted it.
    if (default_row == cur_row)
        if (must_have_default_row)
            u_setDefaultRow(0);
        else
            u_unsetDefaultRow();

    // Adjust the default row if it was before the deleted row in the column.
    if (cur_row < default_row)
        u_setDefaultRow(default_row - 1);

    // Move current row upwards (not downwards as by default).
    setCurrentCell(currentRow() - 1, currentColumn());

    blockSignals(false);

    emit rowRemoved();
}

/**
* @brief Set current row of this table as a default row and color it.
*/
void CommonTableWidget::makeSelectedRowDefault ()
{
    QTableWidgetItem *cur_item = currentItem();
    if (cur_item == nullptr)
        return;

    int cur_row = cur_item->row();
    if (is_input_row_created && cur_row == rowCount() - 1) // we are in the "input" row
        return;

    if (cur_row == default_row)
    {
        if (!must_have_default_row)
            u_unsetDefaultRow();
    }
    else
    {
        u_setDefaultRow(cur_row);
    }
}


/**
* @brief Set max string size of each item.
*/
void CommonTableWidget::setMaxStrSize (int new_max_str_size)
{
    if (new_max_str_size <= 0)
        max_str_size = CMNTBL_MAX_STRSIZE;
    else
        max_str_size = new_max_str_size;
}


/// Reimplemented event. Catch some keybord buttons and add required behavior for them.
void CommonTableWidget::keyPressEvent (QKeyEvent *event)
{
    // Do something only if there is an item selected.
    QTableWidgetItem *item_selected = currentItem();
    if (item_selected != nullptr)
    {
        // ENTER key.
        if (event->key() == Qt::Key_Return)
        {
            if (is_input_row_created && currentRow() == rowCount() - 1)
                addRowFromInputRow();
            return; // skip standard event handling for this button
        }

        // DELETE key.
        else if (event->key() == Qt::Key_Delete)
        {
            removeSelectedRow();
            return; // skip standard event handling for this button
        }
    }

    // If there is no "reserved actions" for keys above - we call base keyPressEvent().
    // It is important e.g. for the (Enter pressing at the editable cell?) or for other keys like
    // arrows, Tab key (swithching to the next cell), Esc key (cancel enter in the editing line),
    // etc.
    QTableWidget::keyPressEvent(event);
}


/// Slot. Some common actions for all rows when the current cell is changed.
void CommonTableWidget::onCellChanged (int row, int col)
{
    // a) Check if the string is too long and modify it if needed.
    QString str = item(row, col)->text();
    u_chopString(str);
    item(row, col)->setText(str);

    // b) The rule: we cannot have void strings for the items.
    // NOTE: this will work also when table looses focus.
    u_completeRow(row);

    emit rowChanged();
}

/// Slot. ...
void CommonTableWidget::onItemSelectionChanged ()
{
    emit rowSelected();
}


// Create input row at the end of the table.
void CommonTableWidget::u_createInputRow ()
{
    if (is_input_row_created)
        return;

    blockSignals(true); // so e.g. not to trigger cellChanged() signal

    u_appendVoidRow();
    int last_row = rowCount() - 1;

    setVerticalHeaderItem(last_row, new QTableWidgetItem("*"));
    for (int c = 0; c < real_column_count; c++)
        setItem(last_row, c, new QTableWidgetItem(""));

    u_colorRow(last_row, CMNTBL_INPROW_CLR);

    blockSignals(false);

    is_input_row_created = true;
}

// Set new default row.
void CommonTableWidget::u_setDefaultRow (int new_default_row)
{
    if (!u_isRowInRange(new_default_row))
        return;
    if (u_isInputRow(new_default_row))
        return;

    // Check that old default row is still valid and uncolor it.
    if (u_isRowInRange(default_row) && !u_isInputRow(default_row))
        u_colorRow(default_row, CMNTBL_NOTSEL_CLR);

    default_row = new_default_row;

    // Color new row.
    u_colorRow(new_default_row, CMNTBL_DEF_CLR);
}

// Unset default row.
void CommonTableWidget::u_unsetDefaultRow ()
{
    // Check that old default row is still valid and uncolor it.
    if (u_isRowInRange(default_row) && !u_isInputRow(default_row))
        u_colorRow(default_row, CMNTBL_NOTSEL_CLR);

    default_row = -1;
}

// Increase rows count.
void CommonTableWidget::u_appendVoidRow ()
{
    setRowCount(rowCount() + 1);
}

// Remove last row ignoring "input" row.
void CommonTableWidget::u_removeLastRow ()
{
    if (getRealRowCount() == 0)
        return;

    if (is_input_row_created)
        removeRow(rowCount() - 2);
    else
        removeRow(rowCount() - 1);
}

// Set item to the cell chopping the length of the passed string.
void CommonTableWidget::u_setItem (int row, int col, QString string)
{
    u_chopString(string);

    QTableWidgetItem *it = new QTableWidgetItem(string);
    it->setBackgroundColor(CMNTBL_NOTSEL_CLR);
    it->setTextColor(CMNTBL_TEXT_CLR);
    setItem(row, col, it);
}

// Color all cells in a row.
void CommonTableWidget::u_colorRow (int row, QColor color)
{
    if (!u_isRowInRange(row))
        return;

    for (int c = 0; c < real_column_count; c++)
        item(row, c)->setBackgroundColor(color);

    clearSelection();
}

// Return true if row is in rowCount() of the table (including "input" row).
bool CommonTableWidget::u_isRowInRange (int row) const
{
    if (row < 0 || row >= rowCount())
        return false;
    return true;
}

// Return true if column is in real_column_count of the table.
bool CommonTableWidget::u_isColInRange (int col) const
{
    if (col < 0 || col >= real_column_count)
        return false;
    return true;
}

// Return true if the given row is "input" row.
bool CommonTableWidget::u_isInputRow (int row) const
{
    if (is_input_row_created && row == rowCount() - 1)
        return true;
    return false;
}

// Return true if the string is a void string (if all symols in a string are spaces we regard
// this string also as a void one).
bool CommonTableWidget::u_isStringVoid (QString str) const
{
    if (str == "")
        return true;

    for (int i = 0; i < str.size(); i++)
        if (str[i] != ' ')
            return false;

    return true;
}

// Chop the string value if it is too long.
bool CommonTableWidget::u_chopString (QString &str)
{
    if (str.size() > max_str_size)
    {
        str.chop(str.size() - max_str_size);
        return true;
    }

    return false;
}

// Return true if all items in a row are void.
bool CommonTableWidget::u_isRowVoid (int row) const
{
    if (!u_isRowInRange(row))
        throw; // TODO: named exception

    for (int c = 0; c < real_column_count; c++)
        if (!u_isStringVoid(item(row, c)->text()))
            return false;

    return true;
}

// Return true if at least one of the items in a row is void.
bool CommonTableWidget::u_isOneInRowVoid (int row) const
{
    if (!u_isRowInRange(row))
        throw; // TODO: named exception

    for (int c = 0; c < real_column_count; c++)
        if (u_isStringVoid(item(row, c)->text()))
            return true;

    return false;
}

// Complete the row with the items copying the text from the most first occured non-void item.
void CommonTableWidget::u_completeRow (int row)
{
    if (!u_isRowInRange(row))
        return;

    QString str = "";

    // Search for the first non-void item.
    for (int c = 0; c < real_column_count; c++)
    {
        if (u_isStringVoid(item(row, c)->text()))
            continue;
        str = item(row, c)->text();
        break;
    }

    // Replace all void items.
    for (int c = 0; c < real_column_count; c++)
        if (u_isStringVoid(item(row, c)->text()))
            item(row, c)->setText(str);
}

// Switch focus to the "input" row.
void CommonTableWidget::u_switchToInputRow ()
{
    if (!is_input_row_created)
        return;

    setFocus();
    setCurrentItem(item(rowCount() - 1, 0));
}


// ...
void CommonTableWidget::u_setDefaultEditTriggers ()
{
    setEditTriggers(edit_triggers);
}

// ...
bool CommonTableWidget::u_commitAndClosePersistentEditor (QTableWidgetItem *item)
{
    if (item == nullptr)
        return false;
    QModelIndex i = indexFromItem(item);
    QWidget* w = indexWidget(i);
    if (w == nullptr) // no line edit opened for the cell for now
        return false;
    commitData(w);
    closePersistentEditor(item);
    return true;
}



