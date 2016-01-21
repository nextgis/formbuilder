/******************************************************************************
 *
 * Name:     dlistvaluesdialog.cpp
 * Project:  NextGIS Form Builder
 * Purpose:  Dialog for setting double lists values.
 * Author:   NextGIS
 *
 ******************************************************************************
 * Copyright (c) 2014, 2015 NextGIS
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#include "fb.h"


FBDoubleListValueDialog::FBDoubleListValueDialog (): QDialog()
{
    this->setWindowModality(Qt::ApplicationModal);
    this->setStyleSheet("QWidget { color: black }");
    this->setWindowTitle(tr("Ввод элементов сдвоенного списка..."));

    QLabel *label1 = new QLabel(this);
    label1->setText(tr("Главный список:"));
    QStringList headers;
    headers<<tr("Внутреннее")<<tr("Отображаемое");
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
    label1c->setText(tr("По умолчанию:"));
    combo1 = new QComboBox(this);
    combo1->addItem(FB_STR_NOT_SELECTED);
    combo1->addItem("");

    label2 = new QLabel(this);
    label2->setText(getTable2String(""));
    QLabel *label2c = new QLabel(this);
    label2c->setText(tr("По умолчанию:"));

    butOk = new QPushButton(this);
    butOk->setText("OK");

    connect(butOk, SIGNAL(clicked()),
            this, SLOT(onOkClicked()));
    connect(table1, SIGNAL(itemSelectionChanged()),
            this,SLOT(onTable1SelectionChanged()));
    connect(table1, SIGNAL(cellChanged(int,int)),
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

    addTable2();
    showTable2(0);
}


// Выход из диалога с подтверждением введённого.
void FBDoubleListValueDialog::onOkClicked ()
{
    this->accept();
}


// В главном списке выбран элемент.
void FBDoubleListValueDialog::onTable1SelectionChanged ()
{
    if (table1->currentRow() == table1->rowCount()-1)
    {
        // Добавляем пустю пару: строку в главном списке и таблицу для зависимого списка.
        table1->setRowCount(table1->rowCount()+1);
        table1->setItem(table1->rowCount()-1,0,new QTableWidgetItem(""));
        table1->setItem(table1->rowCount()-1,1,new QTableWidgetItem(""));
        combo1->addItem("");
        addTable2();
    }
    else
    {
        // Если одно из значений у элемента отсуствует - то присваиваем ему другое.
        // Если нажатия не было и так и осталось - см. соотв. атрибут - там перезаписываем.
        if (table1->currentColumn() == 1 && table1->currentItem()->text() == "")
        {
            table1->currentItem()->setText(table1->item(table1->currentRow(),0)->text());
        }
        else if (table1->currentColumn() == 0 && table1->currentItem()->text() == "")
        {
            table1->currentItem()->setText(table1->item(table1->currentRow(),1)->text());
        }
    }
    showTable2(table1->currentRow());
    label2->setText(getTable2String(table1->item(table1->currentRow(),1)->text()));
}


// В зависимом списке выбран элемент.
void FBDoubleListValueDialog::onTable2SelectionChanged ()
{
    // Всё аналогично главному списку.
    if (curTable2Ptr->currentRow() == curTable2Ptr->rowCount()-1)
    {
        curTable2Ptr->setRowCount(curTable2Ptr->rowCount()+1);
        curTable2Ptr->setItem(curTable2Ptr->rowCount()-1,0,new QTableWidgetItem(""));
        curTable2Ptr->setItem(curTable2Ptr->rowCount()-1,1,new QTableWidgetItem(""));
        curCombo2Ptr->addItem("");
    }
    else
    {
        if (curTable2Ptr->currentColumn() == 1 && curTable2Ptr->currentItem()->text() == "")
        {
            curTable2Ptr->currentItem()
                    ->setText(curTable2Ptr->item(curTable2Ptr->currentRow(),0)->text());
        }
        else if (curTable2Ptr->currentColumn() == 0 && curTable2Ptr->currentItem()->text() == "")
        {
            curTable2Ptr->currentItem()
                    ->setText(curTable2Ptr->item(curTable2Ptr->currentRow(),1)->text());
        }
    }
}


// Изменение элемента из основного списка.
void FBDoubleListValueDialog::onCell1Changed (int row, int col)
{
    if (col == 1)
    {
        combo1->setItemText(row+1,
             FB::shortenStringForOutput(table1->item(row,1)->text())); //+1, т.к. в комбобоксе первый элемент всегда "-"
        label2->setText(getTable2String(table1->item(row,1)->text()));
    }
}


// Изменение элемента из зависимого списка.
void FBDoubleListValueDialog::onCell2Changed (int row, int col)
{
    if (col == 1)
    {
        curCombo2Ptr->setItemText(row+1,
          FB::shortenStringForOutput(curTable2Ptr->item(row,1)->text()));
    }
}


void FBDoubleListValueDialog::addTable2 ()
{
    QStringList headers;
    headers<<tr("Внутреннее")<<tr("Отображаемое");
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
    combo2->addItem(FB_STR_NOT_SELECTED);

    connect(table2, SIGNAL(itemSelectionChanged()),
            this, SLOT(onTable2SelectionChanged()));
    connect(table2, SIGNAL(cellChanged(int,int)),
            this, SLOT(onCell2Changed(int,int)));

    tables2.append(table2);
    combos2.append(combo2);

    table2->hide();
    vlTable2->insertWidget(1,table2);
    combo2->hide();
    hlCombo2->addWidget(combo2);
}


void FBDoubleListValueDialog::removeTable2 (int row)
{

}


void FBDoubleListValueDialog::showTable2 (int row)
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


void FBDoubleListValueDialog::showMsgBox (QString msg)
{
    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setWindowTitle(tr("Сообщение"));
    msgBox.exec();
}


QString FBDoubleListValueDialog::getTable2String (QString srcStr)
{
    srcStr = FB::shortenStringForOutput(srcStr);
    srcStr.prepend(tr("Зависимый список для: "));
    return srcStr;
}


