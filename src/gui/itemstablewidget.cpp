/***************************************************************************************************
 *    Project:  NextGIS Formbuilder
 *    Author:   Mikhail Gusev, gusevmihs@gmail.com
 ***************************************************************************************************
 *    Copyright (C) 2014-2017 NextGIS
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
 **************************************************************************************************/

#include "itemstablewidget.h"

#include <QHeaderView>
#include <QKeyEvent>

using namespace Fb::Gui;


/// Constructor.
FbItemsTableWidget::FbItemsTableWidget (QWidget *wParent):
    QTableWidget(wParent)
{
    m_wasEnterRowAdded = false;

    this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->horizontalHeader()->setStretchLastSection(true);
//    this->verticalHeader()->setMinimumWidth(25);
    this->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);

//    this->setStyleSheet(FbStyles->getStyle("items_table_widget"));

    connect(this, &QTableWidget::itemSelectionChanged,this,&FbItemsTableWidget::onSelectionChanged);
    connect(this, &QTableWidget::cellChanged, this, &FbItemsTableWidget::onCellChanged);

    m_nMinRows = 0;
    m_nMaxRows = MAX_ITEMSTABLE_ROWS;

    // The following line added because there is no any word about what are the defaults for this in
    // the Qt docs:
    m_eEditTrigs = this->editTriggers();
}

/// Destructor.
FbItemsTableWidget::~FbItemsTableWidget ()
{
}


/*!
* @brief Add "enter" row to this table. The row should be added if there is a need to enter new
* items or delete existig ones in this table.
*/
bool FbItemsTableWidget::addEnterRow ()
{
    if (m_wasEnterRowAdded)
        return false;

    this->blockSignals(true); // so e.g. not to trigger cellChanged() signal

    this->setRowCount(this->rowCount() + 1);
    this->setVerticalHeaderItem(this->rowCount() - 1, new QTableWidgetItem("*"));
    for (int j = 0; j < this->columnCount(); j++)
    {
        QTableWidgetItem *it = new QTableWidgetItem("");
        it->setBackgroundColor(COLOR_ITEMSTABLE_LAST);
        this->setItem(this->rowCount() - 1, j, it);
    }

    this->blockSignals(false);

    m_wasEnterRowAdded = true;
    return true;
}


/*!
* @brief Switch focus to the Enter row (if the Enter row was created for this table).
*/
void FbItemsTableWidget::switchToEnterRow ()
{
    if (!m_wasEnterRowAdded)
        return;
    this->setFocus();
    this->setCurrentItem(this->item(this->rowCount() - 1, 0));
}


/*!
* @brief Clear the table. Leave the enter row if it was created for this table.
*/
void FbItemsTableWidget::removeAllItems ()
{
    int n = m_nMinRows;

    while (this->rowCount() > n + 1) // +1 because of Enter row
    {
        this->removeRow(this->rowCount() - 2); // -2 because of Enter row
    }

//    if (!hasUndefinedValue)
//    {
//        if (rowDefault > 1)
//        {
//            rowDefault = 0;
//            this->markDefaultRow(rowDefault);
//        }
//    }
//    else
//    {
//        rowDefault = -1;
//    }
}


/*!
* @brief [SLOT] ...
*/
void FbItemsTableWidget::addRow ()
{
    if (!m_wasEnterRowAdded) // because we take the item values only from the Enter row
        return;

    if (this->rowCount() == m_nMaxRows)
    {
        emit needShowMessage(tr("Unable to add new items. There is already a maximum number of "
                                "items in the table"), false);
        return;
    }

    if (this->isRowVoid(this->rowCount() - 1))
        return;

    this->blockSignals(true); // so at least not to trigger cellChanged() signal

    // Insert new row at the end (but before the Enter row) and copy the items from the Enter row
    // to the newly inserted one.
    // NOTE: after this we do not need to change the default row because the new row is always added
    // to the end.
    int nLastRow = this->rowCount() - 1;
    this->insertRow(nLastRow);
    for (int j = 0; j < this->columnCount(); j++)
    {
        QString str = this->item(nLastRow + 1, j)->text();
        this->setItem(nLastRow, j, new QTableWidgetItem(str));
        this->item(nLastRow + 1, j)->setText("");
    }

    this->blockSignals(false);

    this->scrollToBottom();
    this->switchToEnterRow();
}


/*!
* @brief [SLOT] ...
*/
void FbItemsTableWidget::deleteRow ()
{
    if (!m_wasEnterRowAdded) // some kind of "only for reading" mode if we have not got Enter row
        return;

    QTableWidgetItem *pCurrentItem = this->currentItem();
    if (pCurrentItem == nullptr)
        return;

    int nCurrentRow = pCurrentItem->row();
    if (nCurrentRow == this->rowCount() - 1) // we are in the Enter row
        return;

    if (this->rowCount() == m_nMinRows + 1) // + 1 because of Enter row
    {
        emit needShowMessage(tr("Unable to remove items. The table must have at least %1 items")
                             .arg(m_nMinRows), false);
        return;
    }

    // Remove the row.
    this->removeRow(nCurrentRow);

//    if (rowDefault == selectedRow)
//    {
//        if (!hasUndefinedValue)
//        {
//            rowDefault = 0;
//            this->markDefaultRow(0);
//        }
//        else
//        {
//            rowDefault = -1;
//        }
//    }
//    else if (selectedRow < rowDefault)
//    {
//        rowDefault--;
//    }

    this->setFocus();
}


/*!
* @brief [SLOT] ...
*/
void FbItemsTableWidget::makeDefaultRow ()
{

}


/// [SLOT] ...
void FbItemsTableWidget::onSelectionChanged ()
{
    // QUESTION. Maybe we need to add smth for gui buttons here ...
}


/// [SLOT] Some common actions for all rows when the current cell is changed.
void FbItemsTableWidget::onCellChanged (int nRow, int nCol)
{
    // a) Check if the string is too long and modify it if needed.
    QString str = this->item(nRow, nCol)->text();
    s_reduceString(str);
    this->item(nRow, nCol)->setText(str);

    // b) The rule: we cannot have void strings for the items.
    // NOTE: this will work also when table looses focus.
    this->completeRow(nRow);
}


/// [EVENT] Catch some keybord buttons and add required behavior for them.
void FbItemsTableWidget::keyPressEvent (QKeyEvent *pEvent)
{
    // Do smth only if there is an item selected.
    QTableWidgetItem *pItemSelected = this->currentItem();
    if (pItemSelected != nullptr)
    {
        if (pEvent->key() == Qt::Key_Return) // "Enter" key
        {
            // Anyway close persistant editor with saving data in the cell.
            bool wasEditor = this->u_commitAndClosePersistentEditor(pItemSelected);
//            if (!wasEditor)
//            {
                if (this->currentRow() == this->rowCount() - 1)
                {
                    this->addRow();
                    return;
                }
                else
                {
//                    this->makeDefaultRow();
                    return; // we add return here so not to let the standard event to occur
                }
//            }
        }
        else if (pEvent->key() == Qt::Key_Delete && this->currentRow() != this->rowCount() - 1)
        {
            this->deleteRow();
            return;
        }
    }

    // If there is no "reserved actions" for keys above - we call base keyPressEvent().
    // It is important e.g. for the (Enter pressing at the editable cell?) or for other keys like
    // arrows, Tab key (swithching to the next cell), Esc key (cancel enter in the editing line),
    // etc.
    QTableWidget::keyPressEvent(pEvent);
}


/// Complete the row with the items copying the text from the most first occured not-void item. Can
/// be reimplemented in subclasses to implement another "complete" strategy.
void FbItemsTableWidget::completeRow (int nRow)
{
    QString str = "";

    // Search for the first not-void item.
    for (int j = 0; j < this->columnCount(); j++)
    {
        if (s_isStringVoid(this->item(nRow, j)->text()))
            continue;
        str = this->item(nRow, j)->text();
        break;
    }

    // Replace all void items.
    for (int j = 0; j < this->columnCount(); j++)
    {
        if (s_isStringVoid(this->item(nRow, j)->text()))
            this->item(nRow, j)->setText(str);
    }
}


/// ...
void FbItemsTableWidget::unmarkDefaultRow ()
{

}

/// ...
void FbItemsTableWidget::markDefaultRow (int nRow)
{

}


/// Return true if all items in a row are void.
bool FbItemsTableWidget::isRowVoid (int nRow)
{
    for (int j = 0; j < this->columnCount(); j++)
    {
        if (!s_isStringVoid(this->item(nRow, j)->text()))
            return false;
    }
    return true;
}

/// Return true if at least one of the items in a row is void.
bool FbItemsTableWidget::isOneInRowVoid (int nRow)
{
    for (int j = 0; j < this->columnCount(); j++)
    {
        if (s_isStringVoid(this->item(nRow, j)->text()))
            return true;
    }
    return false;
}

/// Return true if the string is a void string (if all symols in a string are spaces we regard this
/// string also as a void one).
bool FbItemsTableWidget::s_isStringVoid (QString str)
{
    if (str == "")
        return true;
    for (int i = 0; i < str.size(); i++)
        if (str[i] != ' ')
            return false;
    return true;
}


/// Chop the string value if it is too long.
void FbItemsTableWidget::s_reduceString (QString &str)
{
    if (str.size() > MAX_ITEMSTABLE_STRINGSIZE)
        str.chop(str.size() - MAX_ITEMSTABLE_STRINGSIZE);
}


/// ...
void FbItemsTableWidget::u_setDefaultEditTriggers ()
{
    this->setEditTriggers(this->m_eEditTrigs);
}


/// ...
bool FbItemsTableWidget::u_commitAndClosePersistentEditor (QTableWidgetItem* pItem)
{
    if (pItem == nullptr)
        return false;

    QModelIndex i = this->indexFromItem(pItem);
    QWidget* w = this->indexWidget(i);
    if (w == nullptr) // no line edit opened for the cell for now
        return false;

    this->commitData(w);
    this->closePersistentEditor(pItem);
    return true;
}
