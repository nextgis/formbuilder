/**************************************************************************************************
 *                                                                                                *
 *    Project:  NextGIS Formbuilder                                                               *
 *    Authors:  Mikhail Gusev, gusevmihs@gmail.com                                                *
 *              Copyright (C) 2014-2019 NextGIS                                                   *
 *                                                                                                *
 *    This program is free software: you can redistribute it and/or modify it under the terms     *
 *    of the GNU General Public License as published by the Free Software Foundation, either      *
 *    version 2 of the License, or (at your option) any later version.                            *
 *                                                                                                *
 *    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;   *
 *    without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   *
 *    See the GNU General Public License for more details.                                        *
 *                                                                                                *
 *    You should have received a copy of the GNU General Public License along with this program.  *
 *    If not, see http://www.gnu.org/licenses/.                                                   *
 *                                                                                                *
 **************************************************************************************************/

#include "fb_aboutdialog.h"
#include "ui_fb_aboutdialog.h"

#include "util/settings.h"

#include "framework/access/access.h"

#include <QTextStream>
#include <QPushButton>
#include <QVBoxLayout>

using namespace Fb;
using namespace Gui;


FbAboutDialog::FbAboutDialog (QWidget *parent, Language language, QString version):
    CustomDialog(parent),
    ui(new Ui::FbAboutDialog)
{
    ui->setupUi(this);

    int w = g_getSettings()->value(FB_STS_ABOUTDLG_W, 0).toInt();
    int h = g_getSettings()->value(FB_STS_ABOUTDLG_H, 0).toInt();
    if (w != 0 && h != 0)
        this->resize({w, h});

    ui->list_about->addItem(tr("About program"));
    ui->list_about->addItem(tr("Authors of images"));

    ui->list_about->setCurrentRow(0);
    ui->stw_about->setCurrentIndex(0);

    connect(ui->list_about, &QListWidget::itemClicked,
    [&](QListWidgetItem *item)
    {
        ui->stw_about->setCurrentIndex(ui->list_about->currentRow());
    });

    QFile file_authors(":/data/img_authors.txt");
    if (file_authors.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file_authors);
        while (!in.atEnd())
        {
            QString str = in.readLine();
            ui->tedit_cc->appendPlainText(str);
        }
        file_authors.close();
    }

    ui->lab_version->setText(version);

    ui->lab_offpage->setText(QString("<a href=\"%1\">%1</a>")
                             .arg(LANGUAGES.value(language).app_link));
}

FbAboutDialog::~FbAboutDialog ()
{
    g_getSettings()->setValue(FB_STS_ABOUTDLG_W, this->width());
    g_getSettings()->setValue(FB_STS_ABOUTDLG_H, this->height());

    delete ui;
}



