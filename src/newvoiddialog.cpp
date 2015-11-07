/******************************************************************************
 *
 * Name:     newvoiddialog.cpp
 * Project:  NextGIS Form Builder
 * Purpose:  Dialog for creating new void form.
 * Author:   NextGIS
 *
 ******************************************************************************
 * Copyright (c) 2014, 2015 NextGIS
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


FBNewVoidDialog::FBNewVoidDialog(QWidget *parent): QDialog(parent)
{
    this->setStyleSheet("QWidget { color: black }");

    this->setWindowTitle(tr("Задайте параметры слоя в новом проекте..."));

    this->setFont(QFont("Candara",11));

    QLabel *label1 = new QLabel(this);
    label1->setText(tr("Тип геометрии:"));
    comboGeom = new QComboBox(this);
    comboGeom->addItem("POINT",QString("POINT"));
    comboGeom->addItem("LINESTRING",QString("LINESTRING"));
    comboGeom->addItem("POLYGON",QString("POLYGON"));
    comboGeom->addItem("MULTIPOINT",QString("MULTIPOINT"));
    comboGeom->addItem("MULTILINESTRING",QString("MULTILINESTRING"));
    comboGeom->addItem("MULTIPOLYGON",QString("MULTIPOLYGON"));

    QLabel *label2 = new QLabel(this);
    label2->setText(tr("Система координат:"));
    comboSrs = new QComboBox(this);
    comboSrs->addItem("EPSG:4326",QString("4326"));
    comboSrs->addItem("EPSG:3857",QString("3857"));

    QPushButton *but1 = new QPushButton(this);
    but1->setText("OK");
    connect(but1,SIGNAL(clicked()),this,SLOT(accept()));

    QVBoxLayout *vlall = new QVBoxLayout(this);
    QHBoxLayout *hl1 = new QHBoxLayout();
    QHBoxLayout *hl2 = new QHBoxLayout();
    QHBoxLayout *hl3 = new QHBoxLayout();
    hl1->addWidget(label1);
    hl1->addWidget(comboGeom);
    hl2->addWidget(label2);
    hl2->addWidget(comboSrs);
    hl3->addWidget(but1);
    vlall->addLayout(hl1);
    vlall->addLayout(hl2);
    vlall->addLayout(hl3);
}


FBNewVoidDialog::~FBNewVoidDialog()
{
}


