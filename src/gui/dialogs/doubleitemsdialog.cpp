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

#include "doubleitemsdialog.h"

#include "util/settings.h"

#include <QHeaderView>

using namespace Fb;
using namespace Gui;


DoubleItemsDialog::DoubleItemsDialog (QWidget *parent, int min_rows, int max_rows,
                                      bool must_have_default_row):
    CustomDialog(parent)
{
    this->setWindowTitle(tr("Modify items"));

    int w = g_getSettings()->value(FB_STS_DITMDLG_W).toInt();
    int h = g_getSettings()->value(FB_STS_DITMDLG_H).toInt();
    int width = g_getSettings()->value(FB_STS_DITMDLG_COL0_W).toInt();

    if (w != 0 && h != 0)
        this->resize({w, h});

    widget = new ItemsWidget(this, tr("Items:"), {tr("Value"), tr("Display text")},
                             min_rows, max_rows, must_have_default_row);
    if (width != 0)
        widget->getTable()->horizontalHeader()->resizeSection(0, width);

    QPushButton *but_ok = new QPushButton(this);
    but_ok->setText(tr("OK"));
    but_ok->setFocusPolicy(Qt::NoFocus); // so not to exit accidentally
    connect(but_ok, &QPushButton::clicked, this, &DoubleItemsDialog::accept);

    QVBoxLayout *lay_this = new QVBoxLayout(this);
    lay_this->setContentsMargins(2, 2, 2, 2);
    lay_this->setSpacing(0);
    lay_this->addWidget(widget);

    widget->getMainLayout()->addWidget(but_ok, 2, 0);
}

DoubleItemsDialog::~DoubleItemsDialog ()
{
    g_getSettings()->setValue(FB_STS_DITMDLG_W, this->width());
    g_getSettings()->setValue(FB_STS_DITMDLG_H, this->height());
    g_getSettings()->setValue(FB_STS_DITMDLG_COL0_W,
                              widget->getTable()->horizontalHeader()->sectionSize(0));
}


void DoubleItemsDialog::putItems (const Core::DoubleItemsValue &value)
{
    widget->getTable()->putItems({value.inners, value.outers}, value.def_index);
}

void DoubleItemsDialog::getItems (Core::DoubleItemsValue &value) const
{
    QList<QStringList> items;
    widget->getTable()->getItems(items, value.def_index);
    value.inners = items[0];
    value.outers = items[1];
}



