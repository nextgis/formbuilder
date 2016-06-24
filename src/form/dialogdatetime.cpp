/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  dialog for datetime attribute
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

#include <QDateTimeEdit>
 
#include "attributes.h"

FBDialogDatetime::FBDialogDatetime(QWidget *parent):
    QDialog(parent)
{
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowTitle(tr("Define date/time attribute's value"));

    chbCurrent = new QCheckBox(this);
    chbCurrent->setChecked(false);
    chbCurrent->setText(tr("Current date/time value"));
    QObject::connect(chbCurrent, SIGNAL(clicked(bool)),
            this, SLOT(onCheckboxClicked(bool)));

    wDateTime = new QDateTimeEdit(this);
    wDateTime->setMinimumDate(QDate(100,1,1));
    wDateTime->setEnabled(true);

    QPushButton *butOk = new QPushButton(this);
    butOk->setText("OK");
    QObject::connect(butOk, SIGNAL(clicked()),
            this, SLOT(accept()));
    QPushButton *butCancel = new QPushButton(this);
    butCancel->setText(tr("Cancel"));
    QObject::connect(butCancel, SIGNAL(clicked()),
            this, SLOT(reject()));

    QHBoxLayout *hl = new QHBoxLayout();
    hl->addWidget(butOk);
    hl->addWidget(butCancel);
    QVBoxLayout *vl = new QVBoxLayout(this);
    vl->addWidget(chbCurrent);
    vl->addWidget(wDateTime);
    vl->addLayout(hl);
}

void FBDialogDatetime::putValues (QDateTime value, bool ignoreValue,
                                  FBDatetimeFormat *format)
{
    if (ignoreValue)
    {
        chbCurrent->setChecked(true);
        wDateTime->setEnabled(false);
    }
    else
    {
        chbCurrent->setChecked(false);
        wDateTime->setEnabled(true);
    }
    // Set this anyway in case the input to the dialog will be cancelled:
    wDateTime->setDateTime(value);
    wDateTime->setDisplayFormat(format->strDisplayEn);
}

void FBDialogDatetime::getValues (QDateTime &value, bool &ignoreValue)
{
    if (chbCurrent->isChecked())
    {
        ignoreValue = true;
    }
    else
    {
        ignoreValue = false;
        value = wDateTime->dateTime();
    }
}

void FBDialogDatetime::onCheckboxClicked (bool isChecked)
{
    wDateTime->setEnabled(!isChecked);
}


