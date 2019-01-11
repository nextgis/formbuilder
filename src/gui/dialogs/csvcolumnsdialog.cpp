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

#include "csvcolumnsdialog.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

using namespace Fb;
using namespace Gui;


CsvColumnsDialog::CsvColumnsDialog (QWidget *parent, QString file_path,
                                    const QStringList &table_cols,
                                    const QList<QStringList> &csv_contents):
    CustomDialog(parent)
{
    this->setWindowTitle(tr("Select columns from .csv file"));

    QLabel *lab_file = new QLabel(this);
    lab_file->setText(file_path);

    QPushButton *but_ok = new QPushButton(this);
    but_ok->setText(tr("OK"));
    connect(but_ok, &QPushButton::clicked, this, &QDialog::accept);

    QGridLayout *lay_grid = new QGridLayout();
    for (int i = 0; i < table_cols.size(); i++)
    {
        QLabel *lab = new QLabel(this);
        lab->setText(table_cols[i]);

        QComboBox *cmb = new QComboBox(this);
        cmb->addItem("-");
        for (int k = 0; k < csv_contents.size(); k++)
            cmb->addItem(csv_contents[k][0]);

        combos.append(cmb);

        lay_grid->addWidget(lab, i, 0);
        lay_grid->addWidget(cmb, i, 1);
    }

    QVBoxLayout *lay_this = new QVBoxLayout(this);
    lay_this->addWidget(lab_file);
    lay_this->addLayout(lay_grid);
    lay_this->addStretch();
    lay_this->addWidget(but_ok);
}

CsvColumnsDialog::~CsvColumnsDialog ()
{
}


// Return selected indexes of the csv columns or a void list if none of the columns has been
// selected.
QList<int> CsvColumnsDialog::getSelectedIndexes () const
{
    QList<int> list;
    bool all_skipped = true;

    for (int i = 0; i < combos.size(); i++)
    {
        int index = combos[i]->currentIndex() - 1; // - 1 because of the "-" item
        if (index != -1)
            all_skipped = false;
        list.append(index);
    }

    if (all_skipped)
        return QList<int>();
    return list;
}



