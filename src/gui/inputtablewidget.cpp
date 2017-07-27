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

#include "inputtablewidget.h"
#include "gui_globals.h"

#include <QHeaderView>
#include <QKeyEvent>

using namespace Fb::Gui;


/// Constructor.
FbInputTableWidget::FbInputTableWidget (QWidget *wParent, const QStringList &listColumns):
    QTableWidget(wParent)
{
    this->setColumnCount(listColumns.size());
    this->setRowCount(0);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->horizontalHeader()->setStretchLastSection(true);
    this->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setHorizontalHeaderLabels(listColumns);
    this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    // TODO: load the style from the array of styles which was read from the qss file.
//    this->setStyleSheet(FbStyles->getStyle(FbItemsTableWidget::staticMetaObject.className()));
    // TEMP:
    this->setStyleSheet("QTableWidget{selection-background-color: rgb(230,230,230);selection-color: black;}");

    connect(this, &QTableWidget::cellChanged, this, &FbInputTableWidget::onCellChanged);

    // The following line added because there is no any word about what are the defaults for this in
    // the Qt docs:
    m_eEditTrigs = this->editTriggers();

    m_nDefaultRow = -1;

    m_bHasInputRow = false;
    this->addInputRow();
}

/// Destructor.
FbInputTableWidget::~FbInputTableWidget ()
{
}


/*!
* @brief Put the lists of items to the table in a "bulk" mode.
* @param listItems The size of the passed QList equals to the amount of lists which should be loaded
* to the columns of the table.
*/
bool FbInputTableWidget::putItems (const QList<QStringList> &listItems, int nDefaultRow)
{
    // Check if the amount of lists is not equal to the amount of columns of this table.
    if (listItems.size() != this->columnCount())
        return false;

    // Firstly save all items that should not be deleted because of the "minimum rows count"
    // restriction.
    // ...

    // We do not care about the "input" row. Just firstly remove all rows and after that recreate
    // the "input row". As well for the default row.
    this->setRowCount(0);
    this->blockSignals(true); // so e.g. not to trigger cellChanged() signal
    for (int j = 0; j < listItems.size(); j++)
    {
        if (listItems[j].size() > this->rowCount())
            this->setRowCount(listItems[j].size());

        for (int i = 0; i < listItems[j].size(); i++)
        {
            QTableWidgetItem *it = new QTableWidgetItem(listItems[j][i]);
            this->setItem(i, j, it);
        }
    }
    this->blockSignals(false);

    m_bHasInputRow = false;
    this->addInputRow();

    if (nDefaultRow <= -1 || nDefaultRow >= this->rowCount() - 1) // - 1 for "input" row
    {
        m_nDefaultRow = -1;
    }
    else
    {
        m_nDefaultRow = nDefaultRow;
        this->colorRow(m_nDefaultRow, COLOR_INPUTTABLE_DEF);
    }

    return true;
}

/*!
* @brief Return the lists of items in the parameter.
* @param getFromInputRow If true the items from the "input" row will be also added to the lists.
*/
void FbInputTableWidget::getItems (QList<QStringList> &listItems, int &nDefaultRow,
                                   bool bGetFromInputRow)
{
    listItems.clear();

    // Determine if we should also get the items from the last "input" row. If this row is partly
    // void we should complete it.
    int nRowCount = this->rowCount();
    if (!bGetFromInputRow || this->isRowVoid(nRowCount - 1))
    {
        nRowCount--;
    }
    else
    {
        this->completeRow(nRowCount - 1);
    }

    for (int j = 0; j < this->columnCount(); j++)
    {
        QStringList list;

        for (int i = 0; i < nRowCount; i++)
        {
            list.append(this->item(i, j)->text());
        }

        listItems.append(list);
    }

    nDefaultRow = m_nDefaultRow;
}


/*!
* @brief Clear the table, but leave the "input" row.
*/
void FbInputTableWidget::clearItems ()
{
    while (this->rowCount() > 1) // 1 because of "input" row
    {
        this->removeRow(this->rowCount() - 2); // -2 because of "input" row
    }

    m_nDefaultRow = -1;

    this->switchToInputRow();
}


/*!
* @brief ...
*/
void FbInputTableWidget::makeCurrentRowDefault ()
{
    QTableWidgetItem *pCurrentItem = this->currentItem();
    if (pCurrentItem == nullptr)
        return;

    int nCurrentRow = pCurrentItem->row();
    if (nCurrentRow == this->rowCount() - 1) // we are in the "input" row
        return;

    if (m_nDefaultRow == nCurrentRow) // remove the default mark from the row
    {
        this->colorRow(m_nDefaultRow, COLOR_INPUTTABLE_NOTSEL);
        m_nDefaultRow = -1;
    }
    else // mark this row as a new default row
    {
        this->colorRow(m_nDefaultRow, COLOR_INPUTTABLE_NOTSEL);
        this->colorRow(nCurrentRow, COLOR_INPUTTABLE_DEF);
        m_nDefaultRow = nCurrentRow;
    }
}


/// ...
int FbInputTableWidget::getHorizontalHeaderWidth () const
{
    QHeaderView* h = this->horizontalHeader();
    if (h != nullptr)
        return h->width();
    return 0;
}


/// ...
QStringList FbInputTableWidget::getHorizontalHeaderItems () const
{
    QStringList list;
    for (int i = 0; i < this->horizontalHeader()->count(); i++)
    {
        list.append(this->horizontalHeaderItem(i)->text());
    }
    return list;
}


/// Return true if the string is a void string (if all symols in a string are spaces we regard this
/// string also as a void one).
bool FbInputTableWidget::s_isStringVoid (QString str)
{
    if (str == "")
        return true;
    for (int i = 0; i < str.size(); i++)
        if (str[i] != ' ')
            return false;
    return true;
}


/// Chop the string value if it is too long.
bool FbInputTableWidget::s_chopString (QString &str)
{
    if (str.size() > MAX_INPUTTABLE_STRINGSIZE)
    {
        str.chop(str.size() - MAX_INPUTTABLE_STRINGSIZE);
        return true;
    }
    return false;
}


/*!
* @brief Add "input" row to this table.
*/
bool FbInputTableWidget::addInputRow ()
{
    if (m_bHasInputRow)
        return false;

    this->blockSignals(true); // so e.g. not to trigger cellChanged() signal

    this->setRowCount(this->rowCount() + 1);
    this->setVerticalHeaderItem(this->rowCount() - 1, new QTableWidgetItem("*"));
    for (int j = 0; j < this->columnCount(); j++)
    {
        QTableWidgetItem *it = new QTableWidgetItem("");
        it->setBackgroundColor(COLOR_INPUTTABLE_LAST);
        this->setItem(this->rowCount() - 1, j, it);
    }

    this->blockSignals(false);

    m_bHasInputRow = true;
    return true;
}


/*!
* @brief Switch focus to the "input" row.
*/
void FbInputTableWidget::switchToInputRow ()
{
    if (!m_bHasInputRow)
        return;
    this->setFocus();
    this->setCurrentItem(this->item(this->rowCount() - 1, 0));
}


/*!
* @brief Add items once from "input" row.
*/
void FbInputTableWidget::addItemsFromInputRow ()
{
    if (!m_bHasInputRow)
        return;

    if (this->rowCount() == MAX_INPUTTABLE_ROWS) // some restrictions
    {
        FB_DEBUG("A maximum number of items in the table. The row was not added");
        return;
    }

    if (this->isRowVoid(this->rowCount() - 1)) // do not allow if the "input" row is fully void
        return;

    this->blockSignals(true); // so at least not to trigger cellChanged() signal

    // Insert new row at the end but before the "input" row, and copy the items from it to the newly
    // inserted row.
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
    this->switchToInputRow();
}


/*!
* @brief ...
*/
void FbInputTableWidget::deleteCurrentRow ()
{
    if (!m_bHasInputRow) // some kind of "only for reading" mode if we have not got "input" row
        return;

    QTableWidgetItem *pCurrentItem = this->currentItem();
    if (pCurrentItem == nullptr)
        return;

    int nCurrentRow = pCurrentItem->row();
    if (nCurrentRow == this->rowCount() - 1) // we are in the "input" row
        return;

    // Remove the row.
    this->removeRow(nCurrentRow);

    if (m_nDefaultRow == nCurrentRow) // if it was the default row
        m_nDefaultRow = -1;
    else if (nCurrentRow < m_nDefaultRow) // if the current row was upper than the default one
        m_nDefaultRow--;

//    this->setFocus();
}


/// Complete the row with the items copying the text from the most first occured not-void item.
void FbInputTableWidget::completeRow (int nRow)
{
    if (nRow < 0 || nRow >= this->rowCount())
        return;

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


/// Return true if all items in a row are void.
bool FbInputTableWidget::isRowVoid (int nRow) const
{
    // TODO: check nRow and rise exception if it is incorrect.
    // QUESTION: which exception to rise? Or better just to return true/false here?

    for (int j = 0; j < this->columnCount(); j++)
    {
        if (!s_isStringVoid(this->item(nRow, j)->text()))
            return false;
    }
    return true;
}

/// Return true if at least one of the items in a row is void.
bool FbInputTableWidget::isOneInRowVoid (int nRow) const
{
    // TODO: check passed indexes.

    for (int j = 0; j < this->columnCount(); j++)
    {
        if (s_isStringVoid(this->item(nRow, j)->text()))
            return true;
    }
    return false;
}


/// [EVENT] Catch some keybord buttons and add required behavior for them.
void FbInputTableWidget::keyPressEvent (QKeyEvent *pEvent)
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
                    this->addItemsFromInputRow();
                    return;
                }
                else
                {
                    return; // we add return here so not to let the standard event to occur
                }
//            }
        }
        else if (pEvent->key() == Qt::Key_Delete && this->currentRow() != this->rowCount() - 1)
        {
            this->deleteCurrentRow();
            return;
        }
    }

    // If there is no "reserved actions" for keys above - we call base keyPressEvent().
    // It is important e.g. for the (Enter pressing at the editable cell?) or for other keys like
    // arrows, Tab key (swithching to the next cell), Esc key (cancel enter in the editing line),
    // etc.
    QTableWidget::keyPressEvent(pEvent);
}


/// [SLOT] Some common actions for all rows when the current cell is changed.
void FbInputTableWidget::onCellChanged (int nRow, int nCol)
{
    // a) Check if the string is too long and modify it if needed.
    QString str = this->item(nRow, nCol)->text();
    s_chopString(str);
    this->item(nRow, nCol)->setText(str);

    // b) The rule: we cannot have void strings for the items.
    // NOTE: this will work also when table looses focus.
    this->completeRow(nRow);
}


/// ...
void FbInputTableWidget::colorRow (int nRow, QColor oColor)
{
    if (nRow < 0 || nRow >= this->rowCount())
        return;

    for (int j = 0; j < this->columnCount(); j++)
        this->item(nRow, j)->setBackgroundColor(oColor);

    this->clearSelection();
}


/// ...
void FbInputTableWidget::u_setDefaultEditTriggers ()
{
    this->setEditTriggers(this->m_eEditTrigs);
}


/// ...
bool FbInputTableWidget::u_commitAndClosePersistentEditor (QTableWidgetItem* pItem)
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



