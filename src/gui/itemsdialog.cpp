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


/*!
* @brief Constructor.
* @see FbInputTableWidget::FbInputTableWidget
*/
FbItemsDialog::FbItemsDialog (QWidget *wParent, const QStringList &listColumns):
    QDialog(wParent)
{
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowTitle(tr("Enter the items"));

    m_wLabCaption = new QLabel(this);
    m_wLabCaption->setText(tr("Items:"));

    m_wButDefault = new QToolButton(this);
    m_wButDefault->setIcon(QIcon(":/img/make_default.png"));
    m_wButDefault->setToolTip(tr("Make selected row the default one"));
    connect(m_wButDefault, &QToolButton::clicked, this, &FbItemsDialog::onButDefaultClicked);

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

    m_wTable = new FbInputTableWidget(this, listColumns);
    m_wTable->setFocus();

    QHBoxLayout *lhTop = new QHBoxLayout();
    lhTop->setSpacing(4);
    lhTop->setContentsMargins(0,0,0,0);
    lhTop->addWidget(m_wLabCaption);
    lhTop->addStretch();
    lhTop->addWidget(m_wButDefault);
    lhTop->addWidget(m_wButClear);
    lhTop->addWidget(m_wButCsv);
    QVBoxLayout *lvAll= new QVBoxLayout(this);
    lvAll->addLayout(lhTop);
    lvAll->addWidget(m_wTable);
    lvAll->addWidget(m_wButOk);

    // FIXME: remove any numbers for GUI! But how else to increase the size of this dialog here?
    this->resize(400, 500);
}

/*!
* @brief Destructor.
*/
FbItemsDialog::~FbItemsDialog ()
{
}


/*!
* @brief Put items to the dialog.
* @see FbInputTableWidget::putItems()
*/
bool FbItemsDialog::putItems(const QList<QStringList> &listItems, int nDefaultRow)
{
    return m_wTable->putItems(listItems, nDefaultRow);
}

/*!
* @brief Get items from the dialog.
* @see FbInputTableWidget::getItems()
*/
void FbItemsDialog::getItems (QList<QStringList> &listItems, int &nDefaultRow)
{
    m_wTable->getItems(listItems, nDefaultRow, true);
}


/// Slot. Show message box with text.
void FbItemsDialog::showMessage (QString sText, bool bIsCritical)
{
    g_showMsgBox(this, sText, bIsCritical);
}

/// Slot. Show message box with text and two buttons.
void FbItemsDialog::showQuestion (QString sText)
{
    g_showQuestionBox(this, sText);
}


/// Slot. Button "Make default" pressed.
void FbItemsDialog::onButDefaultClicked ()
{
    m_wTable->makeCurrentRowDefault();
}


/// Slot. Button "Clear table" pressed.
void FbItemsDialog::onButClearClicked ()
{
    if (m_wTable->rowCount() <= 1) // 1 for "input" row
        return;

    int ret = g_showQuestionBox(this, tr("Do you want to delete all items from the list? This "
                                         "action can not be undone"));
    if (ret != QMessageBox::Ok)
        return;

    m_wTable->clearItems();
}


/// Slot. Button "Load from CSV" pressed.
void FbItemsDialog::onButCsvClicked ()
{
    if (m_wTable->rowCount() > 1) // 1 for "input" row
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

    // TODO: get the last CSV file path from settings.
    // ...

    if (!oFileDialog.exec())
        return;

    QString sFile = oFileDialog.selectedFiles()[0];

    QStringList listTableColumns = this->m_wTable->getHorizontalHeaderItems();

    // TODO: rewrite here all the work with GDAL. At least create some smart pointer so not to call
    // GDALClose each time.

    GDALDataset *pDataset = temp_getGdalDataset(sFile, listTableColumns.size());
    if (pDataset == NULL)
    {
        g_showMsgBox(this, tr("Unable to open CSV file via GDAL."), true);
        return;
    }

    QStringList listCsvColumns = temp_getCsvColumns(pDataset);
    if (listCsvColumns.empty())
    {
        GDALClose(pDataset);
        g_showMsgBox(this, tr("Unable to read columns in the CSV file via GDAL."), true);
        return;
    }

    FbCsvColumnsDialog oCsvColsDialog(this, sFile, listTableColumns, listCsvColumns);
    if (!oCsvColsDialog.exec())
    {
        GDALClose(pDataset);
        return;
    }

    QList<int> listCsvFieldIndexes = oCsvColsDialog.getCsvFieldIndexes();
    if (!this->u_areCsvFieldIndexesSet(listCsvFieldIndexes))
    {
        GDALClose(pDataset);
        g_showMsgBox(this, tr("No CSV columns were selected. No data will be imported."), false);
        return;
    }

    QList<QStringList> listCsvData = temp_getCsvData(pDataset, listCsvFieldIndexes);
    if (listCsvData.empty())
    {
        GDALClose(pDataset);
        g_showMsgBox(this, tr("Unable to read data from the CSV file via GDAL."), true);
        return;
    }

    GDALClose(pDataset);

    this->putItems(listCsvData, -1);
}


/// Slot. Button "OK" pressed.
void FbItemsDialog::onButOkClicked ()
{
    this->accept();
}


// ...
bool FbItemsDialog::u_areCsvFieldIndexesSet (QList<int> listCsvFieldIndexes) const
{
    // At least one index should be set.
    for (int j = 0; j < listCsvFieldIndexes.size(); j++)
    {
        if (listCsvFieldIndexes[j] >= 0)
            return true;
    }
    return false;
}


// ...
GDALDataset *FbItemsDialog::temp_getGdalDataset (QString sPath, int nTableColumnCount)
{
    QByteArray baPath;
    baPath = sPath.toUtf8();

    // Note: all-register function had been already called outside.

    char **papszAllowedDrivers = NULL;
    papszAllowedDrivers = CSLAddString(papszAllowedDrivers, "CSV");

//    CPLErrorReset();
    GDALDataset *poDS = (GDALDataset*) GDALOpenEx(baPath.data(), GDAL_OF_VECTOR | GDAL_OF_READONLY,
        papszAllowedDrivers, NULL, NULL);
    CSLDestroy(papszAllowedDrivers);
    if (poDS == NULL)
    {
        FB_DEBUG("Unable to open CSV dataset via GDAL.");
        return NULL;
    }

//    CPLErrorReset();
    OGRLayer *poLayer = poDS->GetLayer(0);
    if (poLayer == NULL)
    {
        FB_DEBUG("Unable to read the layer in a CSV dataset via GDAL.");
        GDALClose(poDS);
        return NULL;
    }

    OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
    int fieldCount = poFDefn->GetFieldCount();
//    if (fieldCount < nTableColumnCount)
//    {
//        qDebug() << "[GDAL] The amount of columns in the CSV file can not be less than ...";
//        GDALClose(poDS);
//        return NULL;
//    }

    int countFeat = poLayer->GetFeatureCount();
//    if (countFeat < 2) // QUESTION: one for headers and one for data ???
//    {
//        qDebug() << "[GDAL] The amount of rows in the CSV file can not be less than 2.";
//        GDALClose(poDS);
//        return NULL;
//    }

    return poDS;
}

// ...
QStringList FbItemsDialog::temp_getCsvColumns (GDALDataset *poDS)
{
    QStringList listCsvColumns;

    if (poDS == NULL)
        return listCsvColumns;

    OGRLayer *poLayer = poDS->GetLayer(0);
    // TODO: check for NULL

    OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
    int fieldCount = poFDefn->GetFieldCount();

    for (int i=0; i<fieldCount; i++)
    {
        QString sLayerName(poFDefn->GetFieldDefn(i)->GetNameRef());
        listCsvColumns.append(sLayerName);
    }

    return listCsvColumns;
}

// ...
QList<QStringList> FbItemsDialog::temp_getCsvData (GDALDataset *poDS, QList<int> listFieldIndexes)
{
    QList<QStringList> listCsvData;

    if (poDS == NULL)
        return listCsvData;

    OGRLayer *poLayer = poDS->GetLayer(0);
    // TODO: check for NULL

    for (int l = 0; l < listFieldIndexes.size(); l++)
    {
        listCsvData.append(QStringList());
    }

    int nChoppedStrings = 0;
    OGRFeature *poFeature;
    poLayer->ResetReading();
    while((poFeature = poLayer->GetNextFeature()) != NULL)
    {
        for (int l = 0; l < listFieldIndexes.size(); l++)
        {
            int i = listFieldIndexes[l];
            if (i < 0)
            {
                listCsvData[l].append("");
            }
            else
            {
                QString s = QString::fromUtf8(poFeature->GetFieldAsString(i));
                if (FbInputTableWidget::s_chopString(s))
                    nChoppedStrings++;
                listCsvData[l].append(s);
            }
        }
        OGRFeature::DestroyFeature(poFeature);
    }

    if (nChoppedStrings > 0)
    {
        FB_DEBUG(QString("%1 strings have been cut due to their length").arg(nChoppedStrings));
    }

    return listCsvData;
}



