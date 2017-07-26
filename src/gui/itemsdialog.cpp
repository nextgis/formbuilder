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

#include "itemsdialog.h"
#include "gui_globals.h"
#include "csvcolumnsdialog.h"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QFileDialog>

using namespace Fb::Gui;


/// Constructor.
FbItemsDialog::FbItemsDialog (QWidget *wParent, const QStringList &listColumns):
    QDialog(wParent)
{
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowTitle(tr("Enter the items"));

    m_wLabCaption = new QLabel(this);
    m_wLabCaption->setText(tr("Items:"));

    m_wButClear = new QToolButton(this);
    m_wButClear->setIcon(QIcon(":/img/clear_all.png"));
    m_wButClear->setToolTip(tr("Delete all items from the list"));
    connect(m_wButClear, &QToolButton::clicked, this, &FbItemsDialog::onButClearClicked);

    m_wButCsv = new QToolButton(this);
    m_wButCsv->setIcon(QIcon(":/img/load_csv.png"));
    m_wButCsv->setToolTip(tr("Load items from a CSV file"));
    connect(m_wButCsv, &QToolButton::clicked, this, &FbItemsDialog::onButCsvClicked);

    m_wButOk = new QPushButton(this);
    m_wButOk->setText(tr("OK"));
    connect(m_wButOk, &QPushButton::clicked, this, &FbItemsDialog::onButOkClicked);

    m_wTable = new FbInputTableWidget(this, listColumns, 0);
    m_wTable->setFocus();

    QHBoxLayout *lhTop = new QHBoxLayout();
    lhTop->setSpacing(4);
    lhTop->setContentsMargins(0,0,0,0);
    lhTop->addWidget(m_wLabCaption);
    lhTop->addStretch();
    lhTop->addWidget(m_wButClear);
    lhTop->addWidget(m_wButCsv);
    QVBoxLayout *lvAll= new QVBoxLayout(this);
    lvAll->addLayout(lhTop);
    lvAll->addWidget(m_wTable);
    lvAll->addWidget(m_wButOk);

    // FIXME: remove any numbers for GUI! But how else to increase the size of this dialog here?
    this->resize(400, 500);
}

/// Destructor.
FbItemsDialog::~FbItemsDialog ()
{
}


/// [SLOT] ...
void FbItemsDialog::showMessage (QString sText, bool bIsCritical)
{
    g_showMsgBox(this, sText, bIsCritical);
}

/// [SLOT] ...
void FbItemsDialog::showQuestion (QString sText)
{
    g_showQuestionBox(this, sText);
}


/// [SLOT] button "Clear" pressed.
void FbItemsDialog::onButClearClicked ()
{
    if (m_wTable->rowCount() <= 1) // 1 for "input" row
        return;

    int ret = g_showQuestionBox(this, tr("Do you want to delete all items from the list? This "
                                         "action can not be undone"));
    if (ret != QMessageBox::Ok)
        return;

    m_wTable->deleteItems();
}


/// [SLOT] button "Load from CSV" pressed.
void FbItemsDialog::onButCsvClicked ()
{
    if (m_wTable->rowCount() > 1) // 1 for "input" row
    {
        int ret = g_showQuestionBox(this, tr("If you load a items from a CSV file all current"
                                             "items in the table will be removed. Continue?"));
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

    // Get the last CSV file path from settings.
    // ...
    oFileDialog.setDirectory(QDir());

    if (!oFileDialog.exec())
        return;

    QString sFile;
    sFile = oFileDialog.selectedFiles()[0];

    QStringList hdrs = this->m_wTable->getHorizontalHeaderItems();

    FbCsvColumnsDialog oCsvColsDialog(this, sFile, hdrs);
    if (!oCsvColsDialog.exec())
        return;


}


/// [SLOT] button "OK" pressed.
void FbItemsDialog::onButOkClicked ()
{
    this->accept();
}



