/**************************************************************************************************
 *                                                                                                *
 *    Project:  NextGIS Formbuilder                                                               *
 *    Authors:  Mikhail Gusev, gusevmihs@gmail.com                                                *
 *              Copyright (C) 2014-2018 NextGIS                                                   *
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

#include "tabpagesdialog.h"

#include "sizes.h"
#include "util/settings.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QToolButton>

using namespace Fb;
using namespace Gui;


TabPagesDialog::TabPagesDialog (QWidget *parent, int min_rows, int max_rows):
    CustomDialog(parent)
{
    this->setWindowTitle(tr("Modify headers"));

    int w = g_getSettings()->value(FB_STS_TPAGESDLG_W, 0).toInt();
    int h = g_getSettings()->value(FB_STS_TPAGESDLG_H, 0).toInt();
    if (w != 0 && h != 0)
        this->resize({w, h});

    table = new CommonTableWidget(this, {tr("Header text")}, min_rows, max_rows);

    QPushButton *but_ok = new QPushButton(this);
    but_ok->setText(tr("OK"));
    but_ok->setFocusPolicy(Qt::NoFocus); // so not to exit accidentally
    connect(but_ok, &QPushButton::clicked, this, &TabPagesDialog::accept);

    QToolButton *but_add = new QToolButton(this);
    but_add->setIcon(QIcon(":/images/add.svg"));
    but_add->setToolTip(tr("Add header from the last row"));
    but_add->setFixedSize(DIALOG_TOOLB_SIZE, DIALOG_TOOLB_SIZE);
    connect(but_add, &QToolButton::clicked, [&]()
    {
        table->addRowFromInputRow();
    });

    QToolButton *but_remove = new QToolButton(this);
    but_remove->setIcon(QIcon(":/images/remove.svg"));
    but_remove->setToolTip(tr("Remove selected header"));
    but_remove->setFixedSize(DIALOG_TOOLB_SIZE, DIALOG_TOOLB_SIZE);
    connect(but_remove, &QToolButton::clicked, [&]()
    {
        table->removeSelectedRow();
    });

    QVBoxLayout *lay_table = new QVBoxLayout();
    lay_table->addWidget(table);
    lay_table->addWidget(but_ok);

    QVBoxLayout *lay_buts = new QVBoxLayout();
    lay_buts->addWidget(but_add);
    lay_buts->addWidget(but_remove);
    lay_buts->addStretch();

    QHBoxLayout *lay_this = new QHBoxLayout(this);
    lay_this->addLayout(lay_table);
    lay_this->addLayout(lay_buts);
}

TabPagesDialog::~TabPagesDialog ()
{
    g_getSettings()->setValue(FB_STS_TPAGESDLG_W, this->width());
    g_getSettings()->setValue(FB_STS_TPAGESDLG_H, this->height());
}


void TabPagesDialog::putItems (const QList<QString> &items)
{
    QList<QStringList> list = {items};
    table->putItems(list, -1);
}

void TabPagesDialog::getItems (QList<QString> &items) const
{
    QList<QStringList> list;
    int stub_int;
    table->getItems(list, stub_int);
    items = list[0];
}



