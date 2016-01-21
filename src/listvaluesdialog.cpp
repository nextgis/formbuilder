/******************************************************************************
 *
 * Name:     listvaluesdialog.cpp
 * Project:  NextGIS Form Builder
 * Purpose:  Dialog for setting lists values.
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

FBListValuesDialog::FBListValuesDialog(QString elemName): QDialog()
{
    this->setWindowModality(Qt::ApplicationModal);
    this->setStyleSheet("QWidget { color: black }");

    this->elemName = elemName;

    QLabel *labelL = new QLabel(this);
    labelL->setText(tr("Элементы списка:"));
    listL = new QListWidget();
    connect(listL, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(onLeftClicked(QListWidgetItem*)));
    butAddL = new QToolButton(this);
    butAddL->setIcon(QIcon(":/img/plus.png"));
    butAddL->setToolTip(tr("Добавить новый элемент"));
    connect(butAddL, SIGNAL(clicked()),
            this, SLOT(onLeftAddClicked()));
    butRemoveL = new QToolButton(this);
    butRemoveL->setIcon(QIcon(":/img/minus.png"));
    butRemoveL->setToolTip(tr("Удалить выделенный элемент"));
    butRemoveL->setEnabled(false);
    connect(butRemoveL, SIGNAL(clicked()),
            this, SLOT(onLeftRemoveClicked()));
    butChangeL = new QToolButton(this);
    butChangeL->setIcon(QIcon(":/img/sharp.png"));
    butChangeL->setToolTip(tr("Изменить выделенный элемент"));
    butChangeL->setEnabled(false);
    connect(butChangeL, SIGNAL(clicked()),
            this, SLOT(onLeftChangeClicked()));
    QGroupBox *groupBoxL = new QGroupBox(this);
    groupBoxL->setTitle(tr("Значение"));
    editInnerL = new QLineEdit(groupBoxL);
    QLabel *labNameL = new QLabel(groupBoxL);
    labNameL->setText(tr("Внутреннее:     "));
    editOuterL = new QLineEdit(groupBoxL);
    QLabel *labAliasL = new QLabel(groupBoxL);
    labAliasL->setText(tr("Отображаемое:"));
    QLabel *labDefL = new QLabel(this);
    labDefL->setText(tr("По умолчанию:"));
    comboL = new QComboBox();
    if (elemName != FB_JSON_RADIOGROUP)
    {
        comboL->addItem(FB_STR_NOT_SELECTED);
    }

    QVBoxLayout *vl1 = new QVBoxLayout();
    vl1->addWidget(butAddL);
    vl1->addWidget(butRemoveL);
    vl1->addWidget(butChangeL);
    QHBoxLayout *hl1 = new QHBoxLayout();
    hl1->addWidget(labNameL);
    hl1->addWidget(editInnerL);
    QHBoxLayout *hl2 = new QHBoxLayout();
    hl2->addWidget(labAliasL);
    hl2->addWidget(editOuterL);
    QVBoxLayout *vl2 = new QVBoxLayout(groupBoxL);
    vl2->addLayout(hl1);
    vl2->addLayout(hl2);
    QHBoxLayout *hl3 = new QHBoxLayout();
    hl3->addLayout(vl1);
    hl3->addWidget(groupBoxL);
    QHBoxLayout *hl4 = new QHBoxLayout();
    hl4->addWidget(labDefL);
    hl4->addWidget(comboL);
    QVBoxLayout *vl0 = new QVBoxLayout();
    vl0->addWidget(labelL);
    vl0->addWidget(listL);
    vl0->addLayout(hl3);
    vl0->addLayout(hl4);

    butOk = new QPushButton(this);
    butOk->setText(tr("ОК"));
    connect(butOk, SIGNAL(clicked()), this, SLOT(onOkClicked()));

    QHBoxLayout *h = new QHBoxLayout();
    h->addLayout(vl0);
    QVBoxLayout *v = new QVBoxLayout();
    v->addLayout(h);
    v->addWidget(butOk);
    setLayout(v);

    this->setWindowTitle(tr("Задайте элементы списка ..."));
}


void FBListValuesDialog::onOkClicked()
{
    this->accept();
}

void FBListValuesDialog::onCancelClicked()
{
    this->reject();
}


void FBListValuesDialog::onLeftClicked (QListWidgetItem* item)
{
    editOuterL->setText(item->text());
    editInnerL->setText(item->data(Qt::UserRole).toString());
    butRemoveL->setEnabled(true);
    butChangeL->setEnabled(true);
}


void FBListValuesDialog::onLeftAddClicked()
{
    int limit;
    if (elemName == FB_JSON_RADIOGROUP)
        limit = FB_LIMIT_RADIOGROUP_ELEMS;
    else
        limit = FB_LIMIT_COMBOBOX_ELEMS;

    if (listL->count() == limit || editInnerL->text() == ""
            || editOuterL->text() == "")
        return;

    QListWidgetItem *item = new QListWidgetItem(editOuterL->text());
    item->setData(Qt::UserRole,editInnerL->text());
    listL->addItem(item);
    listL->setCurrentRow(listL->count()-1);
    butRemoveL->setEnabled(true);
    butChangeL->setEnabled(true);
    comboL->addItem(editOuterL->text());
}


void FBListValuesDialog::onLeftRemoveClicked ()
{
    // В радиогруппе должно быть как минимум 2 элемента.
    if (elemName == FB_JSON_RADIOGROUP && listL->count() <= 2)
        return;

    int curListRow = listL->currentRow();
    QListWidgetItem *item = listL->takeItem(curListRow);

    if (item == NULL)
        return;

    editInnerL->setText("");
    editOuterL->setText("");
    listL->setCurrentRow(-1);
    butRemoveL->setEnabled(false);
    butChangeL->setEnabled(false);
    int row;
    if (elemName == FB_JSON_RADIOGROUP)
        row = curListRow;
    else
        row = curListRow + 1;
    comboL->removeItem(row);

    delete item;
}


void FBListValuesDialog::onLeftChangeClicked ()
{
    QListWidgetItem *item = listL->currentItem();

    if (item == NULL || editOuterL->text() == "" || editInnerL->text() == "")
        return;

    item->setText(editOuterL->text());
    item->setData(Qt::UserRole,editInnerL->text());
    int row;
    if (elemName == FB_JSON_RADIOGROUP)
        row = listL->currentRow();
    else
        row = listL->currentRow()+1;
    comboL->setItemText(row, editOuterL->text());
}







