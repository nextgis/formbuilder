/******************************************************************************
 *
 * Name:     aboutdialog.cpp
 * Project:  NextGIS Form Builder
 * Purpose:  Dialog for showing icons authors according to the liscence.
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


FBAboutDialog::FBAboutDialog (QWidget *parent): QDialog(parent)
{
    setWindowTitle(tr("Графика в программе"));
    setModal(true);

    QLabel *lab1 = new QLabel(this);
    lab1->setText(tr("Некоторые изображения в программе\n"
                     "были отредактированы и отличаются\n"
                     "от оригиналов. Список авторов:"));

    QTextEdit *edit = new QTextEdit(this);
    edit->setReadOnly(true);
    edit->append("hunotika, 2014");
    edit->append("Zac Freundt, 2014");
    edit->append("Creative Stall, 2015");
    edit->append("Maximilian Becker, 2012");
    edit->append("Diego Naive, 2012");
    edit->append("Ilsur Aptukov");
    edit->append("Aha-Soft");
    edit->append("Jack Zwanenburg");
    edit->append("Christopher Holm-Hansen");

    QLabel *lab2 = new QLabel(this);
    lab2->setText(QString("<a href=\"http://creativecommons.org/licenses/by/3.0/us/legalcode\">")
                         + tr("Лицензия Creative Commons") + "</a>");
    lab2->setTextFormat(Qt::RichText);
    lab2->setTextInteractionFlags(Qt::TextBrowserInteraction);
    lab2->setOpenExternalLinks(true);

    QPushButton *but = new QPushButton(this);
    but->setText("OK");
    connect(but,SIGNAL(clicked()),this,SLOT(accept()));

    QVBoxLayout *vlay = new QVBoxLayout(this);
    vlay->addWidget(lab1);
    vlay->addWidget(edit);
    vlay->addWidget(lab2);
    vlay->addWidget(but);
}

FBAboutDialog::~FBAboutDialog()
{
}


