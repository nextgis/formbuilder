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

#include "fieldstablewidget.h"

#include "gui/sizes.h"

#include <QHeaderView>
#include <QLabel>
#include <QHBoxLayout>

using namespace Fb;
using namespace Gui;


FieldsTableWidget::FieldsTableWidget (QWidget *parent):
    QTableWidget(parent)
{
    this->setObjectName("table_fields");
    this->verticalHeader()->hide();
    this->setColumnCount(1);
    this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setFocusPolicy(Qt::NoFocus);
    this->horizontalHeader()->hide();
    this->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}

FieldsTableWidget::~FieldsTableWidget ()
{
}


void FieldsTableWidget::updateSelf (const QList<Core::Field *> &fields)
{
    this->clearSelf();

    for (auto field: fields)
    {
        this->setRowCount(this->rowCount() + 1);
        int last_row = this->rowCount() - 1;

        QString tooltip = {tr("This field was not yet bound to an element. Bind it in the "
                              "element's properties")};
        QString icon_path = {":/images/cancel.svg"};
        if (field->getElem() != nullptr)
        {
            tooltip = {tr("This field is successfully bound to an element")};
            icon_path = {":/images/ok.svg"};
        }

        QWidget *widget = new QWidget();

        QHBoxLayout *layout = new QHBoxLayout(widget);
        layout->setContentsMargins(6, 0, 6, 0);
        layout->setSpacing(6);

        QLabel *label_img = new QLabel(widget);
        label_img->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        label_img->setStatusTip(tooltip);
        label_img->setPixmap(QIcon(icon_path).pixmap({FIELD_ICON_SIZE, FIELD_ICON_SIZE}));

        QLabel *label_text = new QLabel(widget);
        label_text->setObjectName("lab_fieldstable_text");
        label_text->setStatusTip(tooltip);
        label_text->setText(QString(" %1").arg(field->getName()));
//        label_text->setText(QString(" %1 (%2)")
//                    .arg(field.getName()).arg(g_getFieldTypeDispName(field.getType())));

        layout->addWidget(label_img);
        layout->addWidget(label_text);

        label_ptrs.append(label_img);

        this->setCellWidget(last_row, 0, widget);
    }

    this->sortItems(0, Qt::AscendingOrder);
}

void FieldsTableWidget::clearSelf ()
{
    for (int row = 0; row < this->rowCount(); row++)
        this->removeCellWidget(row, 0);
    this->clear();
    this->setRowCount(0);
}





