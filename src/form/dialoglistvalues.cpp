/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  dialog for list elems
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

#include <QScrollBar>

#include "attributes.h"

FBDialogListvalues::FBDialogListvalues (QWidget *parent, bool addUndefinedValue)
    : QDialog(parent)
{
    hasUndefinedValue = addUndefinedValue;

    defaultRowIndex = -1;

    curNgwUrl = ""; // no ngw connections
    curNgwLogin = "";
    curNgwPass = "";

//    this->setStyleSheet("QWidget { color: black }");
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowTitle(tr("Define values of the element"));

    this->resize(320,400);

    QLabel *labelL = new QLabel(this);
    labelL->setText(tr("Values:"));

    butClearAll = new QToolButton(this);
    butClearAll->setIcon(QIcon(":/img/clear_all.png"));
    butClearAll->setToolTip(tr("Reset the list, deleting all values from it"));
    QObject::connect(butClearAll, SIGNAL(clicked()),
            this, SLOT(onClearAllClicked()));

    butLoadNgw = new QToolButton(this);
    butLoadNgw->setIcon(QIcon(":/img/load_ngw.png"));
    butLoadNgw->setToolTip(tr("Load NextGIS Web lookup table once ..."));
    QObject::connect(butLoadNgw, SIGNAL(clicked()),
            this, SLOT(onLoadNgwClicked()));

    butLoadNgwSync = new QToolButton(this);
    butLoadNgwSync->setIcon(QIcon(":/img/load_ngw_sync.png"));
    butLoadNgwSync->setToolTip(tr("Establish synchronisation with NextGIS"
                                  " Web lookup table ..."));
    QObject::connect(butLoadNgwSync, SIGNAL(clicked()),
            this, SLOT(onLoadNgwSyncClicked()));

    butLoadCsv = new QToolButton(this);
    butLoadCsv->setIcon(QIcon(":/img/load_csv.png"));
    butLoadCsv->setToolTip(tr("Load list from .csv file ..."));
    QObject::connect(butLoadCsv, SIGNAL(clicked()),
            this, SLOT(onLoadCsvClicked()));

    table = new FBTableDialoglistvalues(this);
    table->setColumnCount(2);
    QStringList headers;
    headers<<tr("Inner")<<tr("Displayed");
    table->setHorizontalHeaderLabels(headers);
    table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    table->horizontalHeader()->setStretchLastSection(true);
    table->verticalHeader()->setMinimumWidth(25);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    //table->setAlternatingRowColors(true);
    this->addEnterRow();
    QObject::connect(table, SIGNAL(itemSelectionChanged()),
            this,SLOT(onTableSelectionChanged()));
    QObject::connect(table, SIGNAL(cellChanged(int,int)),
            this,SLOT(onCellChanged(int,int)));
    QObject::connect(table, SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),
            this,SLOT(onCurrentItemChanged(QTableWidgetItem*,QTableWidgetItem*)));
    table->setStyleSheet(
                         "QTableWidget{"
                         "selection-background-color: rgb(230,230,230);"
                         "selection-color: black;}"
    //                     "QTableWidget::itrm:selected{"
    //                     "border: 1px solid red;}"
                         );

    butAdd = new QPushButton(this);
    butAdd->setText(tr("Add"));
    butAdd->setIcon(QIcon(":/img/add_item.png"));
    butAdd->setToolTip(tr("Add new void value"));
    QObject::connect(butAdd, SIGNAL(clicked()),
                     this, SLOT(onAddClicked()));
    butAdd->setEnabled(true);

    butDelete = new QPushButton(this);
    butDelete->setText(tr("Delete"));
    butDelete->setIcon(QIcon(":/img/delete_item.png"));
    butDelete->setToolTip(tr("Delete selected value"));
    QObject::connect(butDelete, SIGNAL(clicked()),
                     this, SLOT(onDeleteClicked()));
    butDelete->setEnabled(false);

    butDefault = new QPushButton(this);
    QObject::connect(butDefault, SIGNAL(clicked()),
                     this, SLOT(onDefaultClicked()));
    this->updateDefaultButton(NULL);

    butOk = new QPushButton(this);
    butOk->setText(tr("OK"));
    QObject::connect(butOk, SIGNAL(clicked()),
                     this, SLOT(onOkClicked()));

    QHBoxLayout *hlTop = new QHBoxLayout();
    hlTop->setSpacing(4);
    hlTop->setContentsMargins(0,0,0,0);
    hlTop->addWidget(labelL);
    hlTop->addStretch();
    hlTop->addWidget(butClearAll);
    hlTop->addWidget(butLoadNgw);
    hlTop->addWidget(butLoadNgwSync);
    hlTop->addWidget(butLoadCsv);
    QHBoxLayout *hlMiddle = new QHBoxLayout();
    hlMiddle->setSpacing(4);
    hlMiddle->setContentsMargins(0,0,0,0);
    hlMiddle->addWidget(butAdd);
    hlMiddle->addWidget(butDelete);
    hlMiddle->addWidget(butDefault);
    QVBoxLayout *v = new QVBoxLayout(this);
    v->addLayout(hlTop);
    v->addWidget(table);
    v->addLayout(hlMiddle);
    v->addWidget(butOk);

    // TEMPORARY:
    butLoadCsv->hide();

    // For radiogroup:
    // TODO: pass the name of the element/attribute to the constructor and use it
    // to define such conditions.
    if (!hasUndefinedValue)
    {
        butLoadNgw->hide();
        butLoadNgwSync->hide();
        butLoadCsv->hide();
    }

    // For no-button interface:
    butAdd->hide();
    butDelete->hide();
    butDefault->hide();
    QObject::connect(table, SIGNAL(keyLastEnterPressed()),
                     this, SLOT(onAddClicked()));
    QObject::connect(table, SIGNAL(keyNotLastDeletePressed()),
                     this, SLOT(onDeleteClicked()));
    QObject::connect(table, SIGNAL(keyNotLastEnterPressed()),
                     this, SLOT(onDefaultClicked()));
}

// Load values to the dialog.
// Call only right after opening the dialog!
void FBDialogListvalues::putValues (QList<QPair<QString,QString> > values,
                                    int valueDefault)
{
    table->blockSignals(true); // so not to trigger cellChanged() signal
    for (int i=0; i<values.size(); i++)
    {
        // TODO: here we must check the length of added items (could be changed
        // manually outside) and the count of items. See according slots of the
        // dialog where it is done for added-via-dialog values.
        table->insertRow(table->rowCount()-1);
        table->setItem(table->rowCount()-2,0,new QTableWidgetItem(values[i].first));
        table->setItem(table->rowCount()-2,1,new QTableWidgetItem(values[i].second));
    }
    defaultRowIndex = valueDefault;
    this->markDefaultRow(defaultRowIndex);
    this->switchToEnterRow();
    table->blockSignals(false);
}


// Obtain values from the dialog.
void FBDialogListvalues::getValues (QList<QPair<QString,QString> > &values,
                                    int &valueDefault)
{
    values.clear();
    for (int i=0; i<table->rowCount()-1; i++) // -1 so not to touch last enter row
    {
        QPair<QString,QString> pair;
//        this->completeRow(i,0);
//        this->completeRow(i,1);
        pair.first = table->item(i,0)->text();
        pair.second = table->item(i,1)->text();
        values.append(pair);
    }
    valueDefault = defaultRowIndex;
}

void FBDialogListvalues::putNgwParams (QString curNgwUrl, QString curNgwLogin,
                                       QString curNgwPass)
{
    this->curNgwUrl = curNgwUrl;
    this->curNgwLogin = curNgwLogin;
    this->curNgwPass = curNgwPass;
}


/*****************************************************************************/
/*                                                                           */
/*                                 SLOTS                                     */
/*                                                                           */
/*****************************************************************************/

void FBDialogListvalues::onTableSelectionChanged ()
{
    QTableWidgetItem *selectedItem = table->currentItem();

    if (selectedItem != NULL && selectedItem->row() == table->rowCount()-1)
    {
        butAdd->setEnabled(true);
    }
    else
    {
        butAdd->setEnabled(false);
    }

    if (selectedItem == NULL || selectedItem->row() == table->rowCount()-1)
    {
        butDelete->setEnabled(false);
    }
    else
    {
        butDelete->setEnabled(true);
    }

    this->updateDefaultButton(selectedItem);
}


void FBDialogListvalues::onCellChanged (int row, int col)
{
    // Check the length of an item's string and modify string if needed.
    QString itemText = table->item(row,col)->text();
    if (itemText.size() > FB_ATTRLIMIT_LISTVALUE_MAXLENGTH)
    {
        itemText.chop(itemText.size() - FB_ATTRLIMIT_LISTVALUE_MAXLENGTH);
        table->item(row,col)->setText(itemText);
    }

    // Remove void items.
    this->completeRow(row,col); // will work also when table looses focus
}

void FBDialogListvalues::onCurrentItemChanged (QTableWidgetItem *current,
                                               QTableWidgetItem *previous)
{
    /*
    if (current->row() == 0 && current->column() == 0
            && (previous != NULL && previous->row() == table->rowCount()-1)
            && !this->isRowVoid(table->rowCount()-1))
    {
        //table->closePersistentEditor(current);
        this->appendRow();
        //table->clearSelection();
        //table->commitAndClosePersistentEditor(current);
        //table->selectRow(table->rowCount()-1);
        //table->setCurrentItem(table->item(table->rowCount()-1,0));
        //table->openPersistentEditor(table->item(table->rowCount()-1,0));
    }
    */
}

void FBDialogListvalues::onLastTableTabPressed ()
{
    /*
    if (!this->isRowVoid(table->rowCount()-1))
    {
        this->appendRow();
        this->switchToEnterRow();
    }
    */
}


void FBDialogListvalues::onAddClicked ()
{
    QTableWidgetItem *selectedItem = table->currentItem();
    if (selectedItem == NULL)
        return;
    int selectedRow = selectedItem->row();
    if (selectedRow != table->rowCount()-1)
        return;

    if (table->rowCount() == FB_ATTRLIMIT_LISTVALUES_MAXCOUNT)
    {
        this->onShowMsgBox(tr("Unable to add new value. There is already a maximum"
                              " number of items in the list"),
                           QMessageBox::Warning);
        return;
    }

    if (this->isOneInRowVoid(table->rowCount()-1))
        return;

    this->appendRow();
}


void FBDialogListvalues::onDeleteClicked ()
{
    QTableWidgetItem *selectedItem = table->currentItem();
    if (selectedItem == NULL)
        return;
    int selectedRow = selectedItem->row();
    if (selectedRow == table->rowCount()-1)
        return;
    if (!hasUndefinedValue && table->rowCount() <= 2+1)
        return;

    table->removeRow(selectedRow);
    if (defaultRowIndex == selectedRow)
    {
        if (!hasUndefinedValue)
        {
            defaultRowIndex = 0;
            this->markDefaultRow(0);
        }
        else
        {
            defaultRowIndex = -1;
        }
    }
    else if (selectedRow < defaultRowIndex)
    {
        defaultRowIndex--;
    }

    table->setFocus();
}


void FBDialogListvalues::onDefaultClicked ()
{
    QTableWidgetItem *selectedItem = table->currentItem();
    if (selectedItem == NULL)
        return;
    int selectedRow = selectedItem->row();
    if (selectedRow == table->rowCount()-1)
        return;

    if (defaultRowIndex == selectedRow)
    {
        if (hasUndefinedValue)
        {
            this->unmarkDefaultRow();
            defaultRowIndex = -1;
        }
    }
    else
    {
        this->unmarkDefaultRow();
        defaultRowIndex = selectedRow;
        this->markDefaultRow(defaultRowIndex);
    }

    table->setFocus();
}


void FBDialogListvalues::onClearAllClicked ()
{
    int ret;
    if (hasUndefinedValue)
        ret = this->onShowAlertBox(tr("Do you want to delete all values from the list?"
                                  " This action can not be undone."),
                                   QMessageBox::Warning);
    else
        ret = this->onShowAlertBox(tr("Do you want to delete all values (except first two)"
                                      " from the list? This action can not be undone."),
                                   QMessageBox::Warning);
    if (ret != QMessageBox::Ok)
        return;

    int n = 1; // don't forget about last enter row
    if (!hasUndefinedValue)
        n = 3;

    while (table->rowCount() > n)
    {
        table->removeRow(table->rowCount()-2);
    }

    if (!hasUndefinedValue)
    {
        if (defaultRowIndex > 1)
        {
            defaultRowIndex = 0;
            this->markDefaultRow(defaultRowIndex);
        }
    }
    else
    {
        defaultRowIndex = -1;
    }

    this->switchToEnterRow();
}


void FBDialogListvalues::onLoadNgwClicked ()
{
    if (curNgwUrl == "")
    {
        this->onShowMsgBox(tr("Unable to establish NextGIS Web connection. The project"
                      " must be created on the base of the NextGIS Web instance"),
                           QMessageBox::Warning);
        return;
    }

    if (table->rowCount() > 1)
    {
        int ret = this->onShowAlertBox(tr("If you load lookup table from NextGIS Web -"
                  " all current items in the list will be removed. Continue?"),
                                       QMessageBox::Warning);
        if (ret != QMessageBox::Ok)
            return;
    }

    FBDialogDictionaryNgw dialog(this,curNgwUrl,curNgwLogin,curNgwPass);
    if (dialog.exec())
    {

    }
}


void FBDialogListvalues::onLoadNgwSyncClicked ()
{

}


void FBDialogListvalues::onLoadCsvClicked ()
{

}


void FBDialogListvalues::onOkClicked()
{
    this->accept();
}

void FBDialogListvalues::onCancelClicked()
{
    this->reject();
}


void FBDialogListvalues::onShowMsgBox (QString msg, QMessageBox::Icon icon)
{
    QMessageBox msgBox(this);
    msgBox.setText(msg);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setWindowTitle(tr("Info"));
    msgBox.setIcon(icon);
    msgBox.exec();
}
int FBDialogListvalues::onShowAlertBox (QString msg, QMessageBox::Icon icon)
{
    QMessageBox msgBox(this);
    msgBox.setText(msg);
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setWindowTitle(tr("Warning"));
    msgBox.setIcon(icon);
    return msgBox.exec();
}


void FBDialogListvalues::keyPressEvent (QKeyEvent *event)
{
    return; // do not allow all buttons (especially Escape) for the dialog
}


/*****************************************************************************/
/*                                                                           */
/*                                METHODS                                    */
/*                                                                           */
/*****************************************************************************/

void FBDialogListvalues::addEnterRow ()
{
    table->setRowCount(table->rowCount()+1);
    QTableWidgetItem *item00 = new QTableWidgetItem("");
    item00->setBackgroundColor(QColor(255,255,150));
    QTableWidgetItem *item01 = new QTableWidgetItem("");
    item01->setBackgroundColor(QColor(255,255,150));
    table->setVerticalHeaderItem(table->rowCount()-1, new QTableWidgetItem("*"));
    table->setItem(table->rowCount()-1,0,item00);
    table->setItem(table->rowCount()-1,1,item01);
}

void FBDialogListvalues::removeEnterRow ()
{
    table->removeRow(table->rowCount()-1);
}


void FBDialogListvalues::appendRow ()
{
    table->blockSignals(true); // so not to trigger cellChanged() signal
    int lastIndex = table->rowCount()-1;
    QString str0 = table->item(lastIndex,0)->text();
    QString str1 = table->item(lastIndex,1)->text();
    table->insertRow(lastIndex);
    table->setItem(lastIndex,0,new QTableWidgetItem(str0));
    table->setItem(lastIndex,1,new QTableWidgetItem(str1));
    table->item(lastIndex+1,0)->setText("");
    table->item(lastIndex+1,1)->setText("");
    table->blockSignals(false);
    // Note: no need to change defaultRowIndex because new row is always added
    // to the end.
    table->scrollToBottom();
    this->switchToEnterRow();
}


void FBDialogListvalues::unmarkDefaultRow ()
{
    if (defaultRowIndex == -1)
        return;
    table->item(defaultRowIndex,0)->setBackgroundColor(QColor(255,255,255));
    table->item(defaultRowIndex,1)->setBackgroundColor(QColor(255,255,255));
    table->clearSelection();
}

void FBDialogListvalues::markDefaultRow (int row)
{
    if (row == -1)
        return;
    table->item(row,0)->setBackgroundColor(QColor(139,183,224));
    table->item(row,1)->setBackgroundColor(QColor(139,183,224));
    table->clearSelection();
}

void FBDialogListvalues::updateDefaultButton (QTableWidgetItem *selectedItem)
{
    if (selectedItem == NULL || selectedItem->row() == table->rowCount()-1)
        butDefault->setEnabled(false);
    else
        butDefault->setEnabled(true);
    if (selectedItem != NULL
            && selectedItem->row() == defaultRowIndex
            && hasUndefinedValue)
    {
        butDefault->setText(tr("Default Off"));
        butDefault->setIcon(QIcon(":/img/default_item_off.png"));
        butDefault->setToolTip(tr("Unmark default selected value"));
    }
    else
    {
        butDefault->setText(tr("Default On"));
        butDefault->setIcon(QIcon(":/img/default_item_on.png"));
        butDefault->setToolTip(tr("Mark selected value as default"));
    }
}


void FBDialogListvalues::completeRow (int row, int col)
{
    /*
    int otherCol;
    if (col == 0)
        otherCol = 1;
    else
        otherCol = 0;
    if (this->isItemVoid(table->item(row,otherCol)))
        table->item(row,otherCol)->setText(table->item(row,col)->text());
    */
    if (this->isItemVoid(table->item(row,0)))
        table->item(row,0)->setText(table->item(row,1)->text());
    if (this->isItemVoid(table->item(row,1)))
        table->item(row,1)->setText(table->item(row,0)->text());
}


void FBDialogListvalues::switchToEnterRow ()
{
    table->setFocus();
    table->setCurrentItem(table->item(table->rowCount()-1,0));
}


bool FBDialogListvalues::isRowVoid (int row)
{
    if (this->isItemVoid(table->item(row,0))
            && this->isItemVoid(table->item(row,1)))
        return true;
    return false;
}

bool FBDialogListvalues::isOneInRowVoid (int row)
{
    if (this->isItemVoid(table->item(row,0))
            || this->isItemVoid(table->item(row,1)))
        return true;
    return false;
}

bool FBDialogListvalues::isItemVoid (QTableWidgetItem *item)
{
    QString text = item->text();
    if (text == "")
        return true;
    for (int i=0; i<text.size(); i++)
        if (text[i] != ' ')
            return false;
    return true;
}


QString FBDialogListvalues::shortenStr (QString str) // STATIC
{
    if (str.size() > FB_ATTRLIMIT_STRDISPLAY_MAXSIZE)
    {
        str.remove(0, str.size() - FB_ATTRLIMIT_STRDISPLAY_MAXSIZE);
        str.append("...");
    }
    return str;
}


/*****************************************************************************/
/*                                                                           */
/*                         FBTableDialoglistvalues                           */
/*                                                                           */
/*****************************************************************************/

// Catch some keybord buttons and add required behaviour for them.
void FBTableDialoglistvalues::keyPressEvent (QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return ||
            event->key() == Qt::Key_Enter)
    {
        // Emit signals for Enter key only if there is no persistant editor for
        // the cell. ANYWAY close persistant editor with saving data in the cell.
        bool wasEditor = this->commitAndClosePersistentEditor(this->currentItem());
        if (!wasEditor) // NOTE: just remove this check for the last line if users
        {               // will ask for the single-press of Enter key for addition.
            if (this->currentRow() == this->rowCount()-1)
            {
                emit keyLastEnterPressed();
                return;
            }
            else
            {
                emit keyNotLastEnterPressed();
                return;
            }
        }
    }

    else if (event->key() == Qt::Key_Delete
             && this->currentRow() != this->rowCount()-1)
    {
        emit keyNotLastDeletePressed();
        return;
    }

    // If there is no "reserved actions" for keys above - we call base keyPressEvent().
    // It is important e.g. for the (Enter pressing at the editable cell?) or for other
    // keys like arrows, Tab key (swithching to the next cell), Esc key (cancel enter in
    // the editing line), etc.
    QTableWidget::keyPressEvent(event);
}

bool FBTableDialoglistvalues::commitAndClosePersistentEditor (QTableWidgetItem* item)
{
    if (item == NULL)
        return false;
    QModelIndex index = this->indexFromItem(item);
    QWidget* editor = this->indexWidget(index);
    if (editor == NULL) // no line edit opened for the cell for now
        return false;
    this->commitData(editor);
    this->closePersistentEditor(item);
    return true;
}



