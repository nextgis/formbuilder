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

#include <QVBoxLayout>

using namespace Fb::Gui;


/// Constructor.
FbItemsDialog::FbItemsDialog (QWidget *wParent):
    QDialog(wParent)
{
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowTitle(tr("Enter the items"));
//    this->resize(320, 400);

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

    m_wTable = new FbItemsTableWidget(this);
    connect(m_wTable, &FbItemsTableWidget::needShowMessage, this, &FbItemsDialog::showMessage);
    connect(m_wTable, &FbItemsTableWidget::needShowQuestion, this, &FbItemsDialog::showQuestion);

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
}

/// Destructor.
FbItemsDialog::~FbItemsDialog ()
{
}


/// SLOT: button "Clear" pressed.
void FbItemsDialog::onButClearClicked ()
{

}


/// SLOT: button "Load from CSV" pressed.
void FbItemsDialog::onButCsvClicked ()
{

}


/// SLOT: button "OK" pressed.
void FbItemsDialog::onButOkClicked ()
{
    this->accept();
}


/// SLOT: ...
void FbItemsDialog::showMessage (QString sText, bool isCritical)
{
    g_showMsgBox(this, sText, isCritical);
}

/// SLOT: ...
void FbItemsDialog::showQuestion (QString sText)
{
    g_showQuestionBox(this, sText);
}


