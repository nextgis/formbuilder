/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  
 * Author:   Mikhail Gusev, gusevmihs@gmail.com
 ******************************************************************************
*   Copyright (C) 2014-2017 NextGIS
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

#include "fb.h"

#include "gui/gui_globals.h"

#include <QHeaderView>

#define FB_LISTS_MAX_COLUMNS 64
#define FB_LISTS_MAX_ROWS 1024
#define FB_LISTS_MAX_FIELDNAMELENGTH 32

#define FB_LISTS_COLOR_DEFAULT QColor(255,255,255)
#define FB_LISTS_COLOR_KEY_COLUMN QColor(139,183,224)


FBDialogAddColumn::FBDialogAddColumn (QWidget *parent, QStringList existedNames):
    QDialog(parent)
{
    this->setWindowTitle(tr("Give a name to the new column"));

    edit = new QLineEdit(this);
    edit->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    edit->setText("List" + QString::number(existedNames.size()));
    edit->setMaxLength(FB_LISTS_MAX_FIELDNAMELENGTH);

    butOk = new QPushButton(this);
    butOk->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    butOk->setText(tr("OK"));
    QObject::connect(butOk, SIGNAL(clicked()), this, SLOT(onOkClick()));

    QVBoxLayout *lvAll = new QVBoxLayout(this);
    lvAll->addWidget(edit);
    lvAll->addWidget(butOk);

    this->existedNames = existedNames;

    // TODO: widen this list.
    badSymbols.append(' ');
}

QString FBDialogAddColumn::getName ()
{
    return edit->text();
}

void FBDialogAddColumn::onOkClick ()
{
    if (!this->isNameCorrect(edit->text()))
        return;
    this->accept();
}

bool FBDialogAddColumn::isNameCorrect (QString string)
{
    if (string.isEmpty())
        return false;

    bool hasBadSymbols = false;
    for (int i=0; i<badSymbols.size(); i++)
    {
        if (string.contains(badSymbols[i]))
        {
            hasBadSymbols = true;
            break;
        }
    }
    if (hasBadSymbols)
    {
        QMessageBox msgBox(this);
        msgBox.setText(tr("The name contains unsupported symbols. Select another one"));
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return false;
    }

    if (existedNames.contains(string))
    {
        QMessageBox msgBox(this);
        msgBox.setText(tr("There is already a column with this name. Select another one"));
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return false;
    }

    return true;
}


// ====================================================================================================


FBDialogLists::FBDialogLists (QWidget *parent):
    QDialog(parent)
{
    this->setWindowTitle(tr("Lists editor"));

    table = new QTableWidget(this);
    table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    table->setRowCount(1);

    butAddCol = new QToolButton(this);
    butAddCol->setIcon(QIcon(":img/add_col.png"));
    butAddCol->setToolTip(tr("Add new column"));
    QObject::connect(butAddCol, SIGNAL(clicked()), this, SLOT(onAddColumnClick()));

    butRemoveCol = new QToolButton(this);
    butRemoveCol->setIcon(QIcon(":img/rem_col.png"));
    butRemoveCol->setToolTip(tr("Remove selected column"));
    QObject::connect(butRemoveCol, SIGNAL(clicked()), this, SLOT(onRemoveColumnClick()));

    butAddRow = new QToolButton(this);
    butAddRow->setIcon(QIcon(":img/add_row.png"));
    butAddRow->setToolTip(tr("Append row"));
    QObject::connect(butAddRow, SIGNAL(clicked()), this, SLOT(onAddRowClick()));

    butRemoveRow = new QToolButton(this);
    butRemoveRow->setIcon(QIcon(":img/rem_row.png"));
    butRemoveRow->setToolTip(tr("Remove selected row"));
    QObject::connect(butRemoveRow, SIGNAL(clicked()), this, SLOT(onRemoveRowClick()));

    butMakeKey = new QToolButton(this);
    butMakeKey->setIcon(QIcon(":img/make_key.png"));
    butMakeKey->setToolTip(tr("Mark selected column as a key column"));
    QObject::connect(butMakeKey, SIGNAL(clicked()), this, SLOT(onMakeKeyClick()));

    butLoadCsv = new QToolButton(this);
    butLoadCsv->setIcon(QIcon(":/img/load_csv.png"));
    butLoadCsv->setToolTip(tr("Load items from a CSV file"));
    QObject::connect(butLoadCsv, SIGNAL(clicked()), this, SLOT(onLoadCsvClick()));

    butOk = new QPushButton(this);
    butOk->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    butOk->setText(tr("OK"));
    QObject::connect(butOk, SIGNAL(clicked()), this, SLOT(accept()));

    QVBoxLayout *lvButs = new QVBoxLayout();
    lvButs->addWidget(butAddCol);
    lvButs->addWidget(butAddRow);
    lvButs->addWidget(butRemoveCol);
    lvButs->addWidget(butRemoveRow);
    lvButs->addWidget(butMakeKey);
    lvButs->addWidget(butLoadCsv);
    lvButs->addStretch();

    QHBoxLayout *lhMain = new QHBoxLayout();
    lhMain->addWidget(table);
    lhMain->addLayout(lvButs);

    QHBoxLayout *lhBottom = new QHBoxLayout();
    lhBottom->addStretch();
    lhBottom->addWidget(butOk);
    lhBottom->addStretch();

    QVBoxLayout *lvAll = new QVBoxLayout(this);
    lvAll->addLayout(lhMain);
    lvAll->addLayout(lhBottom);

    indexKeyList = -1;
}


void FBDialogLists::onAddColumnClick ()
{
    if (table->columnCount() == FB_LISTS_MAX_COLUMNS)
        return;

    QStringList existedNames;
    for (int i=0; i<table->columnCount(); i++)
    {
        existedNames.append(table->horizontalHeaderItem(i)->text());
    }

    FBDialogAddColumn dialog(this,existedNames);

    if (dialog.exec())
    {
        QString columnName = dialog.getName();
        table->setColumnCount(table->columnCount() + 1);
        table->setHorizontalHeaderItem(table->columnCount() - 1, new QTableWidgetItem(columnName));

        // Create table widget items so we can color them if this will be a key column.
        for (int j=0; j<table->rowCount(); j++)
        {
            QTableWidgetItem *item = table->item(j, table->columnCount() - 1);
            if (item == 0)
            {
                table->setItem(j, table->columnCount() - 1, new QTableWidgetItem(""));
            }
        }

        if (table->columnCount() == 1) // if this is a first column - make it default
        {
            indexKeyList = 0;
            this->colorColumn(0, FB_LISTS_COLOR_KEY_COLUMN);
        }
    }
}


void FBDialogLists::onRemoveColumnClick ()
{
    if (table->currentColumn() == -1)
        return;

    QMessageBox msgBox(this);
    msgBox.setText(tr("Do you really want to remove a column with the name \"")
                      + table->horizontalHeaderItem(table->currentColumn())->text() + "\"?");
    msgBox.setWindowTitle(tr("Warning"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    if (msgBox.exec() == QMessageBox::Ok)
    {    
        if (indexKeyList == table->currentColumn() && table->currentColumn() != 0)
        {
            indexKeyList = 0;
            // Note: nothing to color back to white - column will be deleted.
        }
        // Note: if we delete 0 column - the key index will stay 0 untill we delete the full table -
        // see below.

        table->removeColumn(table->currentColumn());

        if (table->columnCount() == 0)
        {
            indexKeyList = -1;
            table->setRowCount(1);
        }
        else
        {
            this->colorColumn(indexKeyList, FB_LISTS_COLOR_KEY_COLUMN);
        }
    }
}


void FBDialogLists::onAddRowClick ()
{
    if (table->rowCount() == FB_LISTS_MAX_ROWS)
        return;

    table->setRowCount(table->rowCount() + 1);

    // Create items in cells so we can color new item in the key column.
    for (int i=0; i<table->columnCount(); i++)
    {
        QTableWidgetItem *item = table->item(table->rowCount() - 1, i);
        if (item == 0)
        {
            table->setItem(table->rowCount() - 1, i, new QTableWidgetItem(""));
        }
    }

    this->colorColumn(indexKeyList, FB_LISTS_COLOR_KEY_COLUMN);
}


void FBDialogLists::onRemoveRowClick ()
{
    if (table->currentRow() == -1)
        return;

    if (table->rowCount() == 1)
        return;

    QMessageBox msgBox(this);
    msgBox.setText(tr("Do you really want to remove the selected row?"));
    msgBox.setWindowTitle(tr("Warning"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    if (msgBox.exec() == QMessageBox::Ok)
    {
        table->removeRow(table->currentRow());
    }
}


void FBDialogLists::onMakeKeyClick ()
{
    if (table->columnCount() == 0 || table->currentColumn() == -1)
        return;

    if (indexKeyList == table->currentColumn())
        return;

    this->colorColumn(indexKeyList, FB_LISTS_COLOR_DEFAULT); // remove color from previous key column
    indexKeyList = table->currentColumn();
    this->colorColumn(indexKeyList, FB_LISTS_COLOR_KEY_COLUMN);
}


void FBDialogLists::onLoadCsvClick ()
{
    using namespace Fb;
    using namespace Gui;

    if (table->rowCount() > 0 && table->columnCount() > 0)
    {
        int ret = g_showQuestionBox(this, tr("If you load items from a CSV file all current items "
                                             "in the table will be removed. Continue?"));
        if (ret != QMessageBox::Ok)
            return;
    }

    QFileDialog oFileDialog(this);
    oFileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    oFileDialog.setFileMode(QFileDialog::ExistingFile);
    oFileDialog.setViewMode(QFileDialog::List);
    oFileDialog.setDefaultSuffix("csv");
    oFileDialog.setNameFilter("*.csv");
    oFileDialog.setWindowTitle(tr("Select a CSV file"));
    oFileDialog.setDirectory(QDir());
    if (!oFileDialog.exec())
        return;

    QString sPath = oFileDialog.selectedFiles()[0];

    QByteArray baPath;
    baPath = sPath.toUtf8();
    char **papszAllowedDrivers = NULL;
    papszAllowedDrivers = CSLAddString(papszAllowedDrivers, "CSV");
    GDALDataset *poDS = (GDALDataset*) GDALOpenEx(baPath.data(), GDAL_OF_VECTOR | GDAL_OF_READONLY, papszAllowedDrivers, NULL, NULL);
    CSLDestroy(papszAllowedDrivers);
    if (poDS == NULL)
    {
        g_showMsgBox(this, tr("Unable to open CSV file via GDAL."), true);
        return;
    }
    OGRLayer *poLayer = poDS->GetLayer(0);
    if (poLayer == NULL)
    {
        g_showMsgBox(this, tr("Unable to read the layer in a CSV dataset via GDAL."), true);
        GDALClose(poDS);
        return;
    }
    OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
    int fieldCount = poFDefn->GetFieldCount();

    table->clear();
    table->setRowCount(0);
    table->setColumnCount(fieldCount);

    // Get column names.
    QStringList listCsvColumns;
    for (int i = 0; i < fieldCount; i++)
    {
        QString sLayerName(poFDefn->GetFieldDefn(i)->GetNameRef());
        listCsvColumns.append(sLayerName);
    }

    if (listCsvColumns.empty())
    {
        GDALClose(poDS);
        g_showMsgBox(this, tr("Unable to read columns in the CSV file via GDAL."), true);
        return;
    }

    table->setHorizontalHeaderLabels(listCsvColumns);

    OGRFeature *poFeature;
    poLayer->ResetReading();
    while((poFeature = poLayer->GetNextFeature()) != NULL)
    {
        table->setRowCount(table->rowCount() + 1);

        for (int i = 0; i < fieldCount; i++)
        {
            QString str = QString::fromUtf8(poFeature->GetFieldAsString(i));
            table->setItem(table->rowCount() - 1, i, new QTableWidgetItem(str));
        }

        OGRFeature::DestroyFeature(poFeature);
    }

    indexKeyList = 0;
    this->colorColumn(indexKeyList, FB_LISTS_COLOR_KEY_COLUMN);

    GDALClose(poDS);
}


void FBDialogLists::loadLists (QList<QStringList> lists, QString keyList)
{
    if (lists.size() == 0) // skip loading so not to clear table
        return;

    table->setColumnCount(0); // clear table
    table->setRowCount(0);

    for (int i=0; i<lists.size(); i++) // columns
    {
        table->setColumnCount(i+1);

        table->setHorizontalHeaderItem(i, new QTableWidgetItem(lists[i][0]));

        for (int j=1; j<lists[i].size(); j++) // rows, except 0 item - it is column name
        {
            if (i == 0) // increase row count only at first time
            {
                table->setRowCount(j);
            }

            table->setItem(j-1, i, new QTableWidgetItem(lists[i][j]));
        }

        if (keyList == lists[i][0]) // set key list by its name
        {
            indexKeyList = i;
        }
    }

    this->colorColumn(indexKeyList, FB_LISTS_COLOR_KEY_COLUMN);
}


QList<QStringList> FBDialogLists::getLists ()
{
    QList<QStringList> lists;

    for (int i=0; i<table->columnCount(); i++)
    {
        QStringList list;

        list.append(table->horizontalHeaderItem(i)->text()); // the 0 item will be the name of the list.

        for (int j=0; j<table->rowCount(); j++)
        {
            QTableWidgetItem *item = table->item(j,i);
            if (item == 0)
                list.append("");
            else
                list.append(item->text());
        }

        lists.append(list);
    }

    return lists;
}


QString FBDialogLists::getKeyList ()
{
    if (indexKeyList == -1)
        return "";
    return table->horizontalHeaderItem(indexKeyList)->text();
}


void FBDialogLists::colorColumn (int index, QColor color)
{
    if (index == -1)
        return;

    for (int j=0; j<table->rowCount(); j++)
    {
        QTableWidgetItem *item = table->item(j, index);
        if (item != 0)
        {
            item->setBackgroundColor(color);
        }
    }
}


