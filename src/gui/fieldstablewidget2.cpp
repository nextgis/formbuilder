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

#include "fieldstablewidget2.h"

#include "gui/sizes.h"

#include <QHeaderView>
#include <QLabel>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QStandardItemModel>

using namespace Fb;
using namespace Gui;

using namespace Core;


FieldsTableWidget2::FieldsTableWidget2 (QWidget *parent):
    QTableWidget(parent)
{
    this->setObjectName("table_fields");
    this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setFocusPolicy(Qt::NoFocus);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
    this->setColumnCount(3);

    this->stretchColumns();
}

FieldsTableWidget2::~FieldsTableWidget2 ()
{
}


void FieldsTableWidget2::updateSelf (const QList<Field*> &fields, const Elem *selected_elem)
{
    this->clearSelf();

    for (auto field: fields)
    {
        this->setRowCount(this->rowCount() + 1);
        int last_row = this->rowCount() - 1;

        QString tooltip;
        QString icon_path;
        if (field->getElem() == nullptr)
        {
            tooltip = tr("This field was not yet bound to an element. Bind it in the element's "
                         "properties");
            icon_path = ":/images/cancel.svg";
        }
        else
        {
            tooltip = tr("This field is successfully bound to an element");
            icon_path = ":/images/ok.svg";
        }
        QWidget *widget_img = new QWidget();
        QLabel *label_img = new QLabel(widget_img);
        label_img->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        label_img->setStatusTip(tooltip);
        label_img->setPixmap(QIcon(icon_path).pixmap({FIELD_ICON_SIZE, FIELD_ICON_SIZE}));
        QHBoxLayout *layout_img = new QHBoxLayout(widget_img);
        layout_img->setContentsMargins(6, 0, 6, 0);
        layout_img->setSpacing(6);
        layout_img->addWidget(label_img);

        QLineEdit *edit_alias = new QLineEdit();
        edit_alias->setText(field->getAlias());
        edit_alias->setMaxLength(FIELD_NAME_MAX_LEN);
        QFont font = edit_alias->font();
        if (field->getElem() == selected_elem && selected_elem != nullptr)
            font.setBold(true);
        edit_alias->setFont(font);
        connect(edit_alias, &QLineEdit::textChanged,
                field, &Field::onNeedToSetNewAlias);
        connect(edit_alias, &QLineEdit::textChanged,
                this, &FieldsTableWidget2::fieldAliasChanged);

        int cur_index = -1;
        NoWheelCombobox *combo_type = new NoWheelCombobox();
        combo_type->setFocusPolicy(Qt::NoFocus);
        const auto &all_field_types = g_getFieldTypes().keys();
        std::set<FieldType> allowed_field_types;
        if (field->getElem() != nullptr)
        {
            auto field_slots = field->getElem()->getFieldSlots();
            allowed_field_types = field_slots.value(field->getSlot());
        }
        for (int i = 0; i < all_field_types.size(); i++)
        {
            combo_type->addItem(g_getFieldTypeDispName(all_field_types[i]),
                                QVariant::fromValue(all_field_types[i]));
            if (!allowed_field_types.empty() &&
                allowed_field_types.find(all_field_types[i]) == allowed_field_types.end())
            {
                QStandardItemModel *model = qobject_cast<QStandardItemModel*>(combo_type->model());
                QStandardItem *item = model->item(combo_type->count() - 1);
                item->setFlags(item->flags() & ~Qt::ItemIsEnabled); // make item disabled
            }
            if (field->getType() == all_field_types[i])
                cur_index = i;
        }
        combo_type->setCurrentIndex(cur_index);
        connect(combo_type, QOverload<int>::of(&QComboBox::currentIndexChanged),
                field, &Field::onNeedToSetNewFieldType);
        connect(combo_type, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &FieldsTableWidget2::fieldTypeChanged);

        this->setCellWidget(last_row, 0, widget_img);
        this->setCellWidget(last_row, 1, edit_alias);
        this->setCellWidget(last_row, 2, combo_type);
    }

//    this->sortItems(1, Qt::AscendingOrder);
//    this->stretchColumns(); // otherwise sorting brakes stretch effect (why?)
}

void FieldsTableWidget2::clearSelf ()
{
    for (int row = 0; row < this->rowCount(); row++)
    {
        this->removeCellWidget(row, 0);
        this->removeCellWidget(row, 2);
    }
    this->clear();
    this->setRowCount(0);
}


void FieldsTableWidget2::stretchColumns ()
{
    this->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    this->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    this->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
}



