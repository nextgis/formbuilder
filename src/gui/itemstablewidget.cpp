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
* @brief Adds "enter" row to this table. The row should be added if there is a need to enter new
* items in this table.
*/
bool FbItemsTableWidget::addEnterRow ()
{
    if (m_wasEnterRowAdded)
        return false;

//    this->blockSignals(true); // so e.g. not to trigger cellChanged() signal?

    this->setRowCount(this->rowCount() + 1);
    this->setVerticalHeaderItem(this->rowCount() - 1, new QTableWidgetItem("*"));
    for (int c = 0; c < this->columnCount(); c++)
    {
        QTableWidgetItem *it = new QTableWidgetItem("");
        it->setBackgroundColor(COLOR_ITEMSTABLE_LAST);
        this->setItem(this->rowCount() - 1, c, it);
    }

//    this->blockSignals(false);

    m_wasEnterRowAdded = true;
    return true;
}


/*!
* @brief SLOT: ...
*/
void FbItemsTableWidget::addRow ()
{
    if (!m_wasEnterRowAdded) // because we take the item values from the Enter row
        return;

    if (this->rowCount() == m_nMaxRows)
    {
        emit needShowMessage(tr("Unable to add new item. There is already a maximum number of items"
                                " in the table"), false);
        return;
    }

//    if (this->isOneInRowVoid(table->rowCount()-1))
//        return;

    this->blockSignals(true); // so at least not to trigger cellChanged() signal
    int nLastRow = this->rowCount() - 1;
    this->insertRow(nLastRow);
    for (int c = 0; c < this->columnCount(); c++)
    {
        QString s = this->item(nLastRow + 1, c)->text();
        this->setItem(nLastRow, c, new QTableWidgetItem(s));
        this->item(nLastRow + 1, c)->setText("");
    }
    this->blockSignals(false);

    this->scrollToBottom();

    // NOTE: no need to change default row because the new row has been added to the end.

    this->switchToEnterRow();
}


/*!
* @brief SLOT: ...
*/
void FbItemsTableWidget::deleteRow ()
{

}


/*!
* @brief SLOT: ...
*/
void FbItemsTableWidget::makeDefaultRow ()
{

}


/*!
* @brief SLOT: ...
*/
void FbItemsTableWidget::onSelectionChanged ()
{
    // QUESTION. Maybe we need to add smth for gui buttons here ...
    // this->updateItemButtons(true);
}


/*!
* @brief SLOT: some common actions for all rows when the current cell is changed.
*/
void FbItemsTableWidget::onCellChanged (int nRow, int nCol)
{
    // a) Check if the string is too long and modify it if needed.
    QString s = this->item(nRow, nCol)->text();
    FbItemsTableWidget::reduceString(s);
    this->item(nRow, nCol)->setText(s);

    // b) The rule: we cannot have void strings for the items.
    // NOTE: this will work also when table looses focus.
    this->completeRow(nRow);
}


/*!
* @brief Key press event. Catch some keybord buttons and add required behavior for them.
*/
void FbItemsTableWidget::keyPressEvent (QKeyEvent *pEvent)
{
    // Do smth only if there is an item selected.
    QTableWidgetItem *pItemSelected = this->currentItem();
    if (pItemSelected != nullptr)
    {
        if (pEvent->key() == Qt::Key_Return) // "Enter" key
        {
            // Emit signals for Enter key only if there is no persistant editor for the cell. Anyway
            // close persistant editor with saving data in the cell.
            bool wasEditor = this->u_commitAndClosePersistentEditor(this->currentItem());

            // NOTE: just remove the following check for the last line if users will ask for the
            // single-press of Enter key for addition:
            if (!wasEditor)
            {
                if (this->currentRow() == this->rowCount() - 1)
                {
                    this->addRow();
                    return;
                }
                else
                {
                    this->makeDefaultRow();
                    return;
                }
            }
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


/// ...
void FbItemsTableWidget::switchToEnterRow ()
{
    this->setFocus();
    this->setCurrentItem(this->item(this->rowCount() - 1, 0));
}


/// ...
void FbItemsTableWidget::completeRow (int nRow)
{
//    table->item(nRow,1)->setText(table->item(nRow,0)->text());
}


/// ...
void FbItemsTableWidget::unmarkDefaultRow ()
{
//    if (rowDefault == -1)
//        return;
//    table->item(rowDefault,0)->setBackgroundColor(QColor(255,255,255));
//    table->item(rowDefault,1)->setBackgroundColor(QColor(255,255,255));
//    table->clearSelection();
}

/// ...
void FbItemsTableWidget::markDefaultRow (int nRow)
{
//    if (nRow == -1)
//        return;
//    table->item(nRow,0)->setBackgroundColor(QColor(139,183,224));
//    table->item(nRow,1)->setBackgroundColor(QColor(139,183,224));
//    table->clearSelection();
}


/// ...
bool FbItemsTableWidget::isRowVoid (int nRow)
{
    for (int c = 0; c < this->columnCount(); c++)
    {
        if (!this->isItemVoid(this->item(nRow, c)))
            return false;
    }
    return true;
}

/// ...
bool FbItemsTableWidget::isOneInRowVoid (int nRow)
{
    for (int c = 0; c < this->columnCount(); c++)
    {
        if (this->isItemVoid(this->item(nRow, c)))
            return true;
    }
    return false;
}

/// STATIC: ...
bool FbItemsTableWidget::isItemVoid (QTableWidgetItem *pItem)
{
    return FbItemsTableWidget::isStringVoid(pItem->text());
}

/// STATIC: ...
bool FbItemsTableWidget::isStringVoid (QString s)
{
    if (s == "")
        return true;
    for (int i=0; i<s.size(); i++)
        if (s[i] != ' ')
            return false;
    return true;
}


/// STATIC: ...
void FbItemsTableWidget::reduceString (QString &s)
{
    if (s.size() > MAX_ITEMSTABLE_STRINGSIZE)
        s.chop(s.size() - MAX_ITEMSTABLE_STRINGSIZE);
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
