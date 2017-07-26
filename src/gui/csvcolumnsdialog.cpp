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
    const QStringList &listTableColumns, const QStringList &listCsvColumns): QDialog(wParent)
{
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowTitle(tr("Select columns from the CSV file"));

    m_wLabFile = new QLabel(this);
    m_wLabFile->setText(sFile);

    m_wButOk = new QPushButton(this);
    m_wButOk->setText(tr("OK"));
    connect(m_wButOk, &QPushButton::clicked, this, &FbCsvColumnsDialog::onButOkClicked);

    m_lgGrid = new QGridLayout();
    for (int j = 0; j < listTableColumns.size(); j++)
    {
        QLabel *lab = new QLabel(this);
        lab->setText(listTableColumns[j]);

        QComboBox *cmb = new QComboBox(this);
        cmb->addItem("-");
        for (int k = 0; k < listCsvColumns.size(); k++)
        {
            cmb->addItem(listCsvColumns[k]);
        }

        m_listComboPtrs.append(cmb);

        m_lgGrid->addWidget(lab, j, 0);
        m_lgGrid->addWidget(cmb, j, 1);
    }

    QHBoxLayout *lhBottom = new QHBoxLayout();
    lhBottom->addWidget(m_wButOk);
    QVBoxLayout *lvAll = new QVBoxLayout(this);
    lvAll->addWidget(m_wLabFile);
    lvAll->addLayout(m_lgGrid);
    lvAll->addStretch();
    lvAll->addLayout(lhBottom);

    // FIXME: remove any numbers for GUI! But how else to increase the size of this dialog here?
    this->resize(300, 100);
}

/// Destructor.
FbCsvColumnsDialog::~FbCsvColumnsDialog ()
{
}


/*!
 * @brief ...
 */
QList<int> FbCsvColumnsDialog::getCsvFieldIndexes () const
{
    QList<int> list;

    for (int j = 0; j < m_listComboPtrs.size(); j++)
    {
        list.append(m_listComboPtrs[j]->currentIndex() - 1); // - 1 for "-" first item
    }

    return list;
}


/// [SLOT] ...
void FbCsvColumnsDialog::onButOkClicked ()
{
    this->accept();
}



