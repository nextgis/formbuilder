/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  dialog for doubled list elems
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

#include "attributes.h"

#include <QMessageBox>
#include <QHeaderView>

FBDialogDlistvalues::FBDialogDlistvalues (QWidget *parent):
    QDialog(parent)
{
//    this->setStyleSheet("QWidget { color: black }");
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowTitle(tr("Define items for lists ..."));

    QLabel *label1 = new QLabel(this);
    label1->setText(tr("Main list:"));
    QStringList headers;
    headers<<tr("Inner")<<tr("Displayed");
    table1 = new QTableWidget(this);
    table1->setColumnCount(2);
    table1->setRowCount(1);
    table1->setItem(0,0,new QTableWidgetItem(""));
    table1->setItem(0,1,new QTableWidgetItem(""));
    table1->setHorizontalHeaderLabels(headers);
    table1->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    table1->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    table1->horizontalHeader()->setSectionResizeMode(0,//QHeaderView::Stretch
                                                      QHeaderView::Interactive);
    table1->setSelectionMode(QAbstractItemView::SingleSelection);
    QLabel *label1c = new QLabel(this);
    label1c->setText(tr("Default:"));
    combo1 = new QComboBox(this);
    combo1->addItem(FB_DEFVALUE_NOTSELECTED);
    combo1->addItem("");

    label2 = new QLabel(this);
    label2->setText(this->getTable2String(""));
    QLabel *label2c = new QLabel(this);
    label2c->setText(tr("Default:"));

    butOk = new QPushButton(this);
    butOk->setText("OK");

    QObject::connect(butOk, SIGNAL(clicked()),
            this, SLOT(onOkClicked()));
    QObject::connect(table1, SIGNAL(itemSelectionChanged()),
            this,SLOT(onTable1SelectionChanged()));
    QObject::connect(table1, SIGNAL(cellChanged(int,int)),
            this,SLOT(onCell1Changed(int,int)));

    QVBoxLayout *vlAll = new QVBoxLayout(this);
    vlAll->setSpacing(24);
    QHBoxLayout *hl = new QHBoxLayout();
    hl->setSpacing(24);
    QVBoxLayout *vl1 = new QVBoxLayout();
    vl1->setSpacing(6);
    vlTable2 = new QVBoxLayout();
    vlTable2->setSpacing(6);
    QHBoxLayout *hl1c = new QHBoxLayout();
    hl1c->setSpacing(6);
    hlCombo2 = new QHBoxLayout();
    hlCombo2->setSpacing(6);

    hl1c->addWidget(label1c);
    hl1c->addWidget(combo1);
    hlCombo2->addWidget(label2c);
    vl1->addWidget(label1);
    vl1->addWidget(table1);
    vl1->addLayout(hl1c);
    vlTable2->addWidget(label2);
    vlTable2->addLayout(hlCombo2);
    hl->addLayout(vl1);
    hl->addLayout(vlTable2);
    vlAll->addLayout(hl);
    vlAll->addWidget(butOk);

    curTable2Ptr = NULL;
    curCombo2Ptr = NULL;

    this->addTable2();
    this->showTable2(0);
}

// Load values to the dialog.
void FBDialogDlistvalues::putValues (QList<QPair<QString,QString> > vs, int vDef,
                QList<QList<QPair<QString,QString> > > vs2,
                QList<int> vsDef2)
{
    for (int i=0; i<vs.size(); i++)
    {
        table1->setCurrentCell(i,0); // this will also add the string and according table
        table1->currentItem()->setText(vs[i].first);
        table1->setCurrentCell(i,1); // this will also add item to the combobox
        table1->currentItem()->setText(vs[i].second);
        for (int j=0; j<vs2[i].size(); j++)
        {
            tables2[i]->setCurrentCell(j,0);
            tables2[i]->currentItem()->setText(vs2[i][j].first);
            tables2[i]->setCurrentCell(j,1);
            tables2[i]->currentItem()->setText(vs2[i][j].second);
        }
        combos2[i]->setCurrentIndex(vsDef2[i]+1);
    }
    combo1->setCurrentIndex(vDef+1); // if index == -1 it will be = 0
}

// Obtain values from the dialog.
void FBDialogDlistvalues::getValues (QList<QPair<QString,QString> > &vs, int &vDef,
                QList<QList<QPair<QString,QString> > > &vs2,
                QList<int> &vsDef2)
{
    vs.clear();
    vs2.clear();
    vDef = -1; // in case if default item was a void item
    vsDef2.clear();
    for (int i=0; i<table1->rowCount()-1; i++) // -1 because we do not take the
    {                                                 // last always void item
        QString inStr1 = table1->item(i,0)->text();
        QString outStr1 = table1->item(i,1)->text();
        if (inStr1 == "" && outStr1 == "") // not save void strings at all
        {
            continue;
        }
        else if (outStr1 == "") // do not allow to save value with the void area -
        {                       // - complete it
            outStr1 = inStr1;
        }
        else if (inStr1 == "")
        {
            inStr1 = outStr1;
        }
        vs.append(QPair<QString,QString>(inStr1,outStr1));
        if (i == combo1->currentIndex()-1)
        {
            vDef = i; // will be assigned only if continue occurs - otherwise will = -1
        }
        QList<QPair<QString,QString> > tempList;
        vsDef2.append(-1); // will be replaced with defined, if it exists
                  // Create = -1 here anyway, while we definitely have the top-level item
        for (int j=0; j<tables2[i]->rowCount()-1; j++)
        {
            QString inStr2 = tables2[i]->item(j,0)->text();
            QString outStr2 = tables2[i]->item(j,1)->text();
            if (inStr2 == "" && outStr2 == "")
            {
                continue;
            }
            else if (outStr2 == "")
            {
                outStr2 = inStr2;
            }
            else if (inStr2 == "")
            {
                inStr2 = outStr2;
            }
            tempList.append(QPair<QString,QString>(inStr2,outStr2));
            if (j == combos2[i]->currentIndex()-1)
            {
                vsDef2[i] = j;
            }
        }
        vs2.append(tempList);
    }
}

// Exit from the dialog.
void FBDialogDlistvalues::onOkClicked ()
{
    this->accept();
}

// SLOT: in the main list some item was selected.
void FBDialogDlistvalues::onTable1SelectionChanged ()
{
    if (table1->currentRow() == table1->rowCount()-1)
    {
        // Add void pair: string in the main list and the accroding table.
        table1->setRowCount(table1->rowCount()+1);
        table1->setItem(table1->rowCount()-1,0,new QTableWidgetItem(""));
        table1->setItem(table1->rowCount()-1,1,new QTableWidgetItem(""));
        combo1->addItem("");
        this->addTable2();
    }
    else
    {
        // If an item has one of its two values unset we set the same one to it.
        // If there were no pressing - see get-values method how this is solved.
        if (table1->currentColumn() == 1 && table1->currentItem()->text() == "")
        {
            table1->currentItem()->setText(table1->item(table1->currentRow(),0)->text());
        }
        else if (table1->currentColumn() == 0 && table1->currentItem()->text() == "")
        {
            table1->currentItem()->setText(table1->item(table1->currentRow(),1)->text());
        }
    }

    this->showTable2(table1->currentRow());
    label2->setText(this->getTable2String(table1->item(table1->currentRow(),1)->text()));
}

// SLOT: in the dependant list an elem was selected.
void FBDialogDlistvalues::onTable2SelectionChanged ()
{
    // All is equal to the main list. See comments there.
    if (curTable2Ptr->currentRow() == curTable2Ptr->rowCount()-1)
    {
        curTable2Ptr->setRowCount(curTable2Ptr->rowCount()+1);
        curTable2Ptr->setItem(curTable2Ptr->rowCount()-1,0,new QTableWidgetItem(""));
        curTable2Ptr->setItem(curTable2Ptr->rowCount()-1,1,new QTableWidgetItem(""));
        curCombo2Ptr->addItem("");
    }
    else
    {
        if (curTable2Ptr->currentColumn() == 1
                && curTable2Ptr->currentItem()->text() == "")
        {
            curTable2Ptr->currentItem()
                    ->setText(curTable2Ptr->item(curTable2Ptr->currentRow(),0)->text());
        }
        else if (curTable2Ptr->currentColumn() == 0
                 && curTable2Ptr->currentItem()->text() == "")
        {
            curTable2Ptr->currentItem()
                    ->setText(curTable2Ptr->item(curTable2Ptr->currentRow(),1)->text());
        }
    }
}

// SLOT: an item from the main list was modified.
void FBDialogDlistvalues::onCell1Changed (int row, int col)
{
    if (col == 1)
    {
        // Shorten the string, so the combobox will not be extended.
        // Add +1, while there is always "-" first item in the combobox.
        combo1->setItemText(row+1,
             FBDialogDlistvalues::shortenStringForOutput(table1->item(row,1)->text()));
        label2->setText(this->getTable2String(table1->item(row,1)->text()));
    }
}

// SLOT: an item from the dependant list was modified.
void FBDialogDlistvalues::onCell2Changed (int row, int col)
{
    if (col == 1)
    {
        // Shorten the string, so the combobox will not be extended.
        // Add +1, while there is always "-" first item in the combobox.
        curCombo2Ptr->setItemText(row+1,
         FBDialogDlistvalues::shortenStringForOutput(curTable2Ptr->item(row,1)->text()));
    }
}


void FBDialogDlistvalues::addTable2 ()
{
    QStringList headers;
    headers<<tr("Inner")<<tr("Displayed");
    QTableWidget* table2;
    table2 = new QTableWidget(this);
    table2->setColumnCount(2);
    table2->setRowCount(1);
    table2->setItem(0,0,new QTableWidgetItem(""));
    table2->setItem(0,1,new QTableWidgetItem(""));
    table2->setHorizontalHeaderLabels(headers);
    table2->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    table2->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    table2->horizontalHeader()->setSectionResizeMode(0,//QHeaderView::Stretch
                                                     QHeaderView::Interactive);
    table2->setSelectionMode(QAbstractItemView::SingleSelection);
    QComboBox* combo2;
    combo2 = new QComboBox(this);
    combo2->addItem(FB_DEFVALUE_NOTSELECTED);

    QObject::connect(table2, SIGNAL(itemSelectionChanged()),
            this, SLOT(onTable2SelectionChanged()));
    QObject::connect(table2, SIGNAL(cellChanged(int,int)),
            this, SLOT(onCell2Changed(int,int)));

    tables2.append(table2);
    combos2.append(combo2);

    table2->hide();
    vlTable2->insertWidget(1,table2);
    combo2->hide();
    hlCombo2->addWidget(combo2);
}


void FBDialogDlistvalues::removeTable2 (int row)
{

}


void FBDialogDlistvalues::showTable2 (int row)
{
    if (curTable2Ptr != NULL)
        curTable2Ptr->hide();
    curTable2Ptr = tables2[row];
    curTable2Ptr->show();

    if (curCombo2Ptr != NULL)
        curCombo2Ptr->hide();
    curCombo2Ptr = combos2[row];
    curCombo2Ptr->show();
}


void FBDialogDlistvalues::showMsgBox (QString msg)
{
    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setWindowTitle(tr("Message"));
    msgBox.exec();
}


QString FBDialogDlistvalues::getTable2String (QString srcStr)
{
    srcStr = FBDialogDlistvalues::shortenStringForOutput(srcStr);
    srcStr.prepend(tr("Dependant list for: "));
    return srcStr;
}


QString FBDialogDlistvalues::shortenStringForOutput(QString inStr) // STATIC
{
    if (inStr.size() > 23)
    {
        inStr.remove(0,inStr.size()-23);
        inStr.append("...");
    }
    return inStr;
}
