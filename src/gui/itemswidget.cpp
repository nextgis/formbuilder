/**************************************************************************************************
 *                                                                                                *
 *    Project:  NextGIS Formbuilder                                                               *
 *    Authors:  Mikhail Gusev, gusevmihs@gmail.com                                                *
 *              Copyright (C) 2014-2018 NextGIS                                                   *
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

#include "itemswidget.h"

#include "gui_common.h"
#include "sizes.h"
#include "dialogs/csvcolumnsdialog.h"
#include "util/settings.h"
#include "util/csv_io.h"

#include <QVBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>

using namespace Fb;
using namespace Gui;


ItemsWidget::ItemsWidget (QWidget *parent, QString caption, const QStringList &column_names,
                          int min_rows, int max_rows, bool must_have_default_row):
    QWidget(parent)
{
    QLabel *lab_header = new QLabel(this);
    lab_header->setText(QString(" ") + caption);

    table = new CommonTableWidget(this, column_names, min_rows, max_rows, must_have_default_row);

    QToolButton *but_clear = this->createToolButton(
                ":/images/clear.svg",
                tr("Clear list of items"),
                &ItemsWidget::onClearClicked);

    but_csv = this->createToolButton(
                ":/images/csv.svg",
                tr("Load items from .csv file"),
                &ItemsWidget::onCsvClicked);

    QToolButton *but_add = this->createToolButton(
                ":/images/add.svg",
                tr("Add items from the last row"),
                &ItemsWidget::onAddClicked);

    QToolButton *but_remove = this->createToolButton(
                ":/images/remove.svg",
                tr("Remove selected row"),
                &ItemsWidget::onRemoveClicked);

    QToolButton *but_set_def = this->createToolButton(
                ":/images/set_default.svg",
                tr("Make selected row as default one for this list"),
                &ItemsWidget::onSetDefaultClicked);

    QHBoxLayout *lay_00 = new QHBoxLayout();
    lay_00->addWidget(lab_header);
    lay_00->addStretch();
    lay_00->addWidget(but_clear);
    lay_00->addWidget(but_csv);

    QVBoxLayout *lay_11 = new QVBoxLayout();
    lay_11->addWidget(but_add);
    lay_11->addWidget(but_remove);
    lay_11->addWidget(but_set_def);
    lay_11->addStretch();

    lay_grid = new QGridLayout(this);
    lay_grid->setContentsMargins(6, 6, 6, 6);
    lay_grid->setSpacing(5);
    lay_grid->addLayout(lay_00, 0, 0);
    lay_grid->addWidget(table, 1, 0);
    lay_grid->addLayout(lay_11, 1, 1);
}

ItemsWidget::~ItemsWidget ()
{
}


void ItemsWidget::onClearClicked ()
{
    if (table == nullptr)
        return;

    int ret = QMessageBox::Ok;
    if (table->getRealRowCount() != 0 && table->getRealRowCount() > table->getMinRows())
        ret = g_showQuestion(this, tr("Are you sure you want to remove all items in the list?"));

    if (ret != QMessageBox::Ok)
        return;

    table->clearItems();
}

void ItemsWidget::onCsvClicked ()
{
    if (table == nullptr)
        return;

    int ret = QMessageBox::Ok;
    if (table->getRealRowCount() != 0)
        ret = g_showQuestion(this, tr("This will remove all current items in the list. Are you "
                                      "sure you want to load new items?"));
    if (ret != QMessageBox::Ok)
        return;

    QFileDialog file_dialog(this);
    file_dialog.setAcceptMode(QFileDialog::AcceptOpen);
    file_dialog.setFileMode(QFileDialog::ExistingFile);
    file_dialog.setViewMode(QFileDialog::List);
    file_dialog.setDefaultSuffix("csv");
    file_dialog.setNameFilter("*.csv");
    file_dialog.setWindowTitle(tr("Select .csv file"));

    QFileInfo file_info(g_getSettings()->value(FB_STS_CSV_FILE).toString());
    file_dialog.setDirectory(file_info.absoluteDir());

    if (!file_dialog.exec())
        return;

    QString csv_path = file_dialog.selectedFiles()[0];
    g_getSettings()->setValue(FB_STS_CSV_FILE, csv_path);

    // Load csv contents.
    QList<QStringList> csv_contents;
    try
    {
        Util::CsvIo::read(csv_path, csv_contents);
    }
    catch (QString &msg)
    {
        g_showMessageDet(this, tr("Unable to read .csv file"), msg);
        return;
    }
    if (csv_contents.size() == 0)
    {
        g_showMessage(this, tr("Bad .csv file: no columns found"));
        return;
    }
    if (csv_contents[0].size() == 0)
    {
        g_showMessage(this, tr("Bad .csv file: no rows found"));
        return;
    }

    // TODO. Here we read all .csv contents into memory. Make reading only column names first and
    // than reading only the selected columns.

    // Select csv columns for table columns (that's why order of selected indexes is important).
    CsvColumnsDialog cols_dialog(this, csv_path, table->getHorizontalHeaderItems(), csv_contents);
    if (!cols_dialog.exec())
        return;
    QList<int> sel_indexes = cols_dialog.getSelectedIndexes();
    if (sel_indexes.isEmpty())
    {
        g_showMessage(this, tr("You have not selected any columns. Operation cancelled"));
        return;
    }

    // Move data for selected columns. If the csv column was not selected for the table column we
    // must make a void string list of the same size as others.
    QList<QStringList> csv_columns;
    for (int i = 0; i < sel_indexes.size(); i++)
    {
        if (sel_indexes[i] == -1)
        {
            QStringList list;
            for (int j = 0; j < csv_contents[0].size(); j++)
                list.append("");
            csv_columns.append(list);
        }
        else
        {
            csv_columns.append(std::move(csv_contents[sel_indexes[i]]));
        }

        csv_columns.last().removeFirst(); // also remove column name
    }

    // Put data into the table.
    try
    {
        table->putItems(csv_columns, -1);
    }
    catch (QString &msg)
    {
        g_showMessageDet(this, tr("Unable to load items to the table"), msg);
    }
}

void ItemsWidget::onAddClicked ()
{
    table->addRowFromInputRow();
}

void ItemsWidget::onRemoveClicked ()
{
    table->removeSelectedRow();
}

void ItemsWidget::onSetDefaultClicked ()
{
    table->makeSelectedRowDefault();
}


template <typename Slot>
QToolButton *ItemsWidget::createToolButton (QString icon_path, QString hint_text, Slot slot)
{
    QToolButton *but = new QToolButton(this);
    but->setIcon(QIcon(icon_path));
    but->setToolTip(hint_text);
    but->setFixedSize(DIALOG_TOOLB_SIZE, DIALOG_TOOLB_SIZE);
    connect(but, &QToolButton::clicked, this, slot);
    return but;
}



