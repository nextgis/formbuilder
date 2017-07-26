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

#include "csvcolumnsdialog.h"

#include <QVBoxLayout>

using namespace Fb::Gui;


/// Constructor.
FbCsvColumnsDialog::FbCsvColumnsDialog (QWidget *wParent, QString sFile,
    const QStringList &listColumns): QDialog(wParent)
{
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowTitle(tr("Select the columns from the CSV file"));

    m_wLabFile = new QLabel(this);
    m_wLabFile->setText(sFile);

    m_wButOk = new QPushButton(this);
    m_wButOk->setText(tr("OK"));
    connect(m_wButOk, &QPushButton::clicked, this, &FbCsvColumnsDialog::onButOkClicked);

    m_lgGrid = new QGridLayout();
    for (int i = 0; i < listColumns.size(); i++)
    {
        QLabel *lab = new QLabel(this);
        lab->setText(listColumns[i]);

        QComboBox *cmb = new QComboBox(this);
        cmb->addItem("-");

        m_lgGrid->addWidget(lab, i, 0);
        m_lgGrid->addWidget(cmb, i, 1);
    }

    QVBoxLayout *lvAll= new QVBoxLayout(this);
    lvAll->addWidget(m_wLabFile);
    lvAll->addLayout(m_lgGrid);
    lvAll->addStretch();
    lvAll->addWidget(m_wButOk);

    // FIXME: remove any numbers for GUI! But how else to increase the size of this dialog here?
    this->resize(300, 100);
}

/// Destructor.
FbCsvColumnsDialog::~FbCsvColumnsDialog ()
{
}


/// [SLOT] ...
void FbCsvColumnsDialog::onButOkClicked ()
{

}



