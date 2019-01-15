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

#include "tripleitemsdialog.h"

#include "util/settings.h"

#include <QHeaderView>

using namespace Fb;
using namespace Gui;


TripleItemsDialog::TripleItemsDialog (QWidget *parent, int min_rows, int max_rows,
                                      bool must_have_default_row):
    CustomDialog(parent)
{
    this->setWindowTitle(tr("Modify items"));

    int w = g_getSettings()->value(FB_STS_TITMDLG_W).toInt();
    int h = g_getSettings()->value(FB_STS_TITMDLG_H).toInt();
    int width0 = g_getSettings()->value(FB_STS_TITMDLG_COL0_W).toInt();
    int width1 = g_getSettings()->value(FB_STS_TITMDLG_COL1_W).toInt();

    if (w != 0 && h != 0)
        this->resize({w, h});

    widget = new ItemsWidget(this, tr("Items:"), {tr("Value"), tr("Display text (left)"),
                            tr("Display text (right)")}, min_rows, max_rows, must_have_default_row);
    if (width0 != 0)
        widget->getTable()->horizontalHeader()->resizeSection(0, width0);
    if (width1 != 0)
        widget->getTable()->horizontalHeader()->resizeSection(1, width1);

    QPushButton *but_ok = new QPushButton(this);
    but_ok->setText(tr("OK"));
    but_ok->setFocusPolicy(Qt::NoFocus); // so not to exit accidentally
    connect(but_ok, &QPushButton::clicked, this, &TripleItemsDialog::accept);

    QVBoxLayout *lay_this = new QVBoxLayout(this);
    lay_this->setContentsMargins(2, 2, 2, 2);
    lay_this->setSpacing(0);
    lay_this->addWidget(widget);

    widget->getMainLayout()->addWidget(but_ok, 2, 0);
}

TripleItemsDialog::~TripleItemsDialog ()
{
    g_getSettings()->setValue(FB_STS_TITMDLG_W, this->width());
    g_getSettings()->setValue(FB_STS_TITMDLG_H, this->height());
    g_getSettings()->setValue(FB_STS_TITMDLG_COL0_W,
                              widget->getTable()->horizontalHeader()->sectionSize(0));
    g_getSettings()->setValue(FB_STS_TITMDLG_COL1_W,
                              widget->getTable()->horizontalHeader()->sectionSize(1));
}


void TripleItemsDialog::putItems (const Core::TripleItemsValue &value)
{
    widget->getTable()->putItems(
                {value.inners, value.outers_left, value.outers_right}, value.def_index);
}

void TripleItemsDialog::getItems (Core::TripleItemsValue &value) const
{
    QList<QStringList> items;
    widget->getTable()->getItems(items, value.def_index);
    value.inners = items[0];
    value.outers_left = items[1];
    value.outers_right = items[2];
}



