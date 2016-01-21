/******************************************************************************
 *
 * Name:     datetimedialog.cpp
 * Project:  NextGIS Form Builder
 * Purpose:  Dialog for selecting date/time.
 * Author:   NextGIS
 *
 ******************************************************************************
 * Copyright (c) 2014, 2015, 2016 NextGIS
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#include "fb.h"


FBDateTimeDialog::FBDateTimeDialog(): QDialog()
{
    this->setWindowModality(Qt::ApplicationModal);
    this->setStyleSheet("QWidget { color: black }");
    this->setWindowTitle(tr("Задайте значение по умолчанию для Даты/Времени ..."));
    this->setFont(QFont("Candara",11));

    chbCurrent = new QCheckBox(this);
    chbCurrent->setChecked(false);
    chbCurrent->setText(tr("Текущее значение даты/времени"));
    connect(chbCurrent,SIGNAL(clicked(bool)),this,SLOT(onCheckboxClicked(bool)));

    wDateTime = new QDateTimeEdit(this);
    wDateTime->setMinimumDate(QDate(100,1,1));
    wDateTime->setEnabled(true);

    QPushButton *butOk = new QPushButton(this);
    butOk->setText("OK");
    connect(butOk, SIGNAL(clicked()), this, SLOT(accept()));
    QPushButton *butCancel = new QPushButton(this);
    butCancel->setText(tr("Отмена"));
    connect(butCancel, SIGNAL(clicked()), this, SLOT(reject()));

    QHBoxLayout *hl = new QHBoxLayout();
    hl->addWidget(butOk);
    hl->addWidget(butCancel);
    QVBoxLayout *vl = new QVBoxLayout(this);
    vl->addWidget(chbCurrent);
    vl->addWidget(wDateTime);
    vl->addLayout(hl);
}


FBDateTimeDialog::~FBDateTimeDialog()
{
}


void FBDateTimeDialog::onCheckboxClicked (bool isChecked)
{
    wDateTime->setEnabled(!isChecked);
}


