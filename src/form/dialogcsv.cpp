/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  CSV data selection dialog
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

#include "ogrsf_frmts.h"

#include "attributes.h"

FBDialogCsv::FBDialogCsv (QWidget *parent, QString pathCsv):
    QDialog(parent)
{
    baPathCsv = pathCsv.toUtf8();
    iInner = -1;
    iDisplayed = -1;

    this->setWindowTitle(tr("Select only 2 columns in CSV dataset"));

    listMain = new QListWidget(this);

    butInner = new QPushButton(this);
    butInner->setText(tr("For inner"));
    QObject::connect(butInner,SIGNAL(clicked()),
                     this,SLOT(onInnerClicked()));
    butDisplayed = new QPushButton(this);
    butDisplayed->setText(tr("For displayed"));
    QObject::connect(butDisplayed,SIGNAL(clicked()),
                     this,SLOT(onDisplayedClicked()));

    labInner = new QLabel(this);
    this->onInnerClicked();
    labDisplayed = new QLabel(this);
    this->onDisplayedClicked();

    butOk = new QPushButton(this);
    butOk->setText(tr("OK"));
    QObject::connect(butOk,SIGNAL(clicked()),
                     this,SLOT(onOkClicked()));
    butOk->setEnabled(false);

    QHBoxLayout *lhButs = new QHBoxLayout();
    lhButs->addWidget(butInner);
    lhButs->addWidget(butDisplayed);
    QVBoxLayout *lvMain = new QVBoxLayout(this);
    lvMain->addWidget(listMain);
    lvMain->addLayout(lhButs);
    lvMain->addWidget(labInner);
    lvMain->addWidget(labDisplayed);
    lvMain->addWidget(butOk);
}


// Initializing of CSV dataset with according common checks.
GDALDataset *getDataset (QString &errString, QByteArray baPathCsv)
{
    //GDALAllRegister(); // Note: gdal was most likely initialized in the app class

    char **allowedDrivers = NULL;
    allowedDrivers = CSLAddString(allowedDrivers,"CSV");
    GDALDataset *poDS = (GDALDataset*)GDALOpenEx(baPathCsv.data(),
                      GDAL_OF_VECTOR | GDAL_OF_READONLY, allowedDrivers, NULL, NULL );
    CSLDestroy(allowedDrivers);
    if(poDS == NULL)
    {
        errString = QObject::tr("Unable to open CSV dataset via GDAL");
        return NULL;
    }

    OGRLayer *poLayer = poDS->GetLayer(0);
    if (poLayer == NULL)
    {
        errString = QObject::tr("Unable to read the layer in CSV dataset via GDAL");
        GDALClose(poDS);
        return NULL;
    }

    OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
    int fieldCount = poFDefn->GetFieldCount();
    if (fieldCount < 2)
    {
        errString = QObject::tr("The amount of columns in the dataset can not "
                                "be less than 2");
        GDALClose(poDS);
        return NULL;
    }

    int countFeat = poLayer->GetFeatureCount();
    if (countFeat < 2)
    {
        errString = QObject::tr("The amount of rows in the dataset can not be less than 2");
        GDALClose(poDS);
        return NULL;
    }

    return poDS;
}


bool FBDialogCsv::loadValues (QString &errString)
{
    GDALDataset *poDS = getDataset(errString,baPathCsv);
    if (poDS == NULL)
        return false;

    OGRLayer *poLayer = poDS->GetLayer(0);
    OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
    int fieldCount = poFDefn->GetFieldCount();
    for (int i=0; i<fieldCount; i++)
    {
        QString nameLayer(poFDefn->GetFieldDefn(i)->GetNameRef());
        listMain->addItem(nameLayer);
    }

    GDALClose(poDS);
    return true;
}


bool FBDialogCsv::getSelectedValues (QList<QPair<QString,QString> > &values,
                                     QString &errString)
{
    if (iInner == -1 || iDisplayed == -1)
        return false;

    GDALDataset *poDS = getDataset(errString,baPathCsv);
    if (poDS == NULL)
        return false;

    OGRLayer *poLayer = poDS->GetLayer(0);
    OGRFeature *poFeature;
    poLayer->ResetReading();
    values.clear();
    while((poFeature = poLayer->GetNextFeature()) != NULL)
    {
        values.append(QPair<QString,QString>(
                   QString::fromUtf8(poFeature->GetFieldAsString(iInner)),
                   QString::fromUtf8(poFeature->GetFieldAsString(iDisplayed))));
        OGRFeature::DestroyFeature(poFeature);
    }

    GDALClose(poDS);
    return true;
}


void FBDialogCsv::onInnerClicked ()
{
    this->anyButtonClick(true);
}

void FBDialogCsv::onDisplayedClicked ()
{
    this->anyButtonClick(false);
}

void FBDialogCsv::anyButtonClick (bool isInner)
{
    QLabel *lab1;
    QLabel *lab2;
    int *index1;
    int *index2;
    QString str1;
    QString str2;
    if (isInner)
    {
        lab1 = labInner;
        lab2 = labDisplayed;
        index1 = &iInner;
        index2 = &iDisplayed;
        str1 = tr("Column for inner value: ");
        str2 = tr("Column for displayed value: ");
    }
    else
    {
        lab1 = labDisplayed;
        lab2 = labInner;
        index1 = &iDisplayed;
        index2 = &iInner;
        str1 = tr("Column for displayed value: ");
        str2 = tr("Column for inner value: ");
    }

    if (listMain->count() == 0
            || listMain->currentRow() == -1)
    {
        lab1->setText(str1 + "...");
        return;
    }

    lab1->setText(str1 + listMain->currentItem()->text());
    if (*index2 == listMain->currentRow())
    {
        lab2->setText(str2 + "...");
        *index2 = -1;
    }
    *index1 = listMain->currentRow();

    if (iInner != -1 && iDisplayed != -1)
    {
        butOk->setEnabled(true);
    }
    else
    {
        butOk->setEnabled(false);
    }
}



