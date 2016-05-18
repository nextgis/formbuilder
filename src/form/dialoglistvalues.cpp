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

#include "attributes.h"


FBDialogListvalues::FBDialogListvalues (QWidget *parent, bool addUndefinedValue)
    : QDialog(parent)
{
    hasUndefinedValue = addUndefinedValue;

//    this->setStyleSheet("QWidget { color: black }");
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowTitle(tr("Define items for the list ..."));

    QLabel *labelL = new QLabel(this);
    labelL->setText(tr("List elems:"));
    listL = new QListWidget();
    QObject::connect(listL, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(onLeftClicked(QListWidgetItem*)));
    butAddL = new QToolButton(this);
    butAddL->setIcon(QIcon(":/img/plus.png"));
    butAddL->setToolTip(tr("Add new item"));
    QObject::connect(butAddL, SIGNAL(clicked()),
            this, SLOT(onLeftAddClicked()));
    butRemoveL = new QToolButton(this);
    butRemoveL->setIcon(QIcon(":/img/minus.png"));
    butRemoveL->setToolTip(tr("Delete selected item"));
    butRemoveL->setEnabled(false);
    QObject::connect(butRemoveL, SIGNAL(clicked()),
            this, SLOT(onLeftRemoveClicked()));
    butChangeL = new QToolButton(this);
    butChangeL->setIcon(QIcon(":/img/sharp.png"));
    butChangeL->setToolTip(tr("Change selected item"));
    butChangeL->setEnabled(false);
    QObject::connect(butChangeL, SIGNAL(clicked()),
            this, SLOT(onLeftChangeClicked()));
    QGroupBox *groupBoxL = new QGroupBox(this);
    groupBoxL->setTitle(tr("Value"));
    editInnerL = new QLineEdit(groupBoxL);
    QLabel *labNameL = new QLabel(groupBoxL);
    labNameL->setText(tr("Inner:        "));
    editOuterL = new QLineEdit(groupBoxL);
    QLabel *labAliasL = new QLabel(groupBoxL);
    labAliasL->setText(tr("Displayed: "));
    QLabel *labDefL = new QLabel(this);
    labDefL->setText(tr("Default:    "));
    labDefL->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    comboL = new QComboBox(this);
    comboL->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    if (addUndefinedValue)
    {
        comboL->addItem(FB_DEFVALUE_NOTSELECTED);
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
    butOk->setText(tr("OK"));
    QObject::connect(butOk, SIGNAL(clicked()),
                     this, SLOT(onOkClicked()));

    QHBoxLayout *h = new QHBoxLayout();
    h->addLayout(vl0);
    QVBoxLayout *v = new QVBoxLayout();
    v->addLayout(h);
    v->addWidget(butOk);
    this->setLayout(v);
}


// Load values to the dialog.
void FBDialogListvalues::putValues (QList<QPair<QString,QString> > values,
                                    int valueDefault)
{
    for (int i=0; i<values.size(); i++)
    {
        QListWidgetItem *item = new QListWidgetItem(values[i].second);
        item->setData(Qt::UserRole,values[i].first);
        listL->addItem(item);
        comboL->addItem(shortenStr(values[i].second));
    }
    int realIndex = valueDefault;
    if (hasUndefinedValue)
    {
        realIndex++; //need to shift indexes, because there is a stub item in the combobox
    }
    comboL->setCurrentIndex(realIndex);
}


// Obtain values from the dialog.
void FBDialogListvalues::getValues (QList<QPair<QString,QString> > &values,
                                    int &valueDefault)
{
    values.clear();
    for (int i=0; i<listL->count(); i++)
    {
        QPair<QString,QString> pair(listL->item(i)->data(Qt::UserRole).toString(),
                                    listL->item(i)->text());
        values.append(pair);
    }
    int realIndex = comboL->currentIndex();
    if (hasUndefinedValue)
    {
        realIndex--; //need to shift indexes, because there is a stub item in combobox
    }
    valueDefault = realIndex;
}


QString FBDialogListvalues::shortenStr (QString str)
{
    if (str.size() > FB_ATTRLIMIT_STRDISPLAY_MAXSIZE)
    {
        str.remove(0, str.size() - FB_ATTRLIMIT_STRDISPLAY_MAXSIZE);
        str.append("...");
    }
    return str;
}


void FBDialogListvalues::onOkClicked()
{
    this->accept();
}

void FBDialogListvalues::onCancelClicked()
{
    this->reject();
}


void FBDialogListvalues::onLeftClicked (QListWidgetItem* item)
{
    editOuterL->setText(item->text());
    editInnerL->setText(item->data(Qt::UserRole).toString());
    butRemoveL->setEnabled(true);
    butChangeL->setEnabled(true);
}


void FBDialogListvalues::onLeftAddClicked()
{
    int limit;
    // TODO: set different limits.
    limit = FB_ATTRLIMIT_LISTVALUES_MAXCOUNT;
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


void FBDialogListvalues::onLeftRemoveClicked ()
{
    // For some attributes restrict to have less than 2 items in a list.
    if (!hasUndefinedValue && listL->count() <= 2)
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
    int rowCombo = curListRow;
    if (hasUndefinedValue)
    {
        rowCombo++; //need to shift indexes, because there is a stub item in combobox
    }
    comboL->removeItem(rowCombo);
    delete item;
}


void FBDialogListvalues::onLeftChangeClicked ()
{
    QListWidgetItem *item = listL->currentItem();
    if (item == NULL || editOuterL->text() == "" || editInnerL->text() == "")
        return;
    item->setText(editOuterL->text());
    item->setData(Qt::UserRole,editInnerL->text());
    int rowCombo = listL->currentRow();
    if (hasUndefinedValue)
    {
        rowCombo++; //need to shift indexes, because there is a stub item in combobox
    }
    comboL->setItemText(rowCombo,editOuterL->text());
}


