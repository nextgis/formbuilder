/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  about dialog
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

#include "fb.h"

FBDialogAbout::~FBDialogAbout ()
{
}

FBDialogAbout::FBDialogAbout (QWidget *parent):
    QDialog(parent)
{
    //this->setStyleSheet("QWidget { color: black }");

    this->setModal(true);

    this->setWindowTitle(tr("Graphics in program"));
//    this->setFont(QFont(FB_GUI_FONTTYPE, FB_GUI_FONTSIZE_SMALL));

    QLabel *lab1 = new QLabel(this);
    lab1->setText(tr("Some images in program were modified and differ from"
                     " the originals.\nList of authors:"));

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
    edit->append("trasnik");

    QLabel *lab2 = new QLabel(this);
    QString strCc = "http://creativecommons.org/licenses/by/3.0/us/legalcode";
    lab2->setText(QString("<a href=\"" + strCc + "\">")
                         + tr("Creative Commons license") + "</a>");
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


