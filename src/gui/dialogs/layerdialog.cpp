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

#include "layerdialog.h"

#include "gui/sizes.h"
#include "util/settings.h"
#include "gui_common.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QLineEdit>

using namespace Fb;
using namespace Gui;

using namespace Core;

#define FB_LAYERTABLE_ITEM_DIS_FLAGS Qt::ItemIsSelectable | Qt::ItemIsEnabled


LayerDialog::LayerDialog (QWidget *parent, const Layer *layer, bool is_editable,
                          QString res_group_url, QString res_group_name):
    CustomDialog(parent),
    layer(layer),
    is_editable(is_editable)
{
    if (is_editable)
        this->setWindowTitle(tr("Edit layer"));
    else
        this->setWindowTitle(tr("View layer"));

    int w = g_getSettings()->value(FB_STS_LAYERDLG_W, 0).toInt();
    int h = g_getSettings()->value(FB_STS_LAYERDLG_H, 0).toInt();
    if (w != 0 && h != 0)
        this->resize({w, h});

    // Resource group:

    QLabel *lab_res_gr_name = new QLabel(this);
    lab_res_gr_name->setText(tr("Resource group:"));

    QLabel *lab_res_gr_name2 = new QLabel(this);
    lab_res_gr_name2->setText(QString("<a href=\"%1\">%2</a>").arg(res_group_url).arg(res_group_name));
    lab_res_gr_name2->setOpenExternalLinks(true);
    lab_res_gr_name2->setToolTip(res_group_url);

    // Name:

    QLabel *lab_name = new QLabel(this);
    lab_name->setText(tr("Name:"));

    QString url = layer->getNgwUrl();
    QLabel *lab_name2 = new QLabel(this);
    if (url == "")
    {
        lab_name2->setText(layer->getName());
    }
    else
    {
        lab_name2->setText(QString("<a href=\"%1\">%2</a>").arg(url).arg(layer->getName()));
        lab_name2->setToolTip(url);
    }
    //lab_name2->setTextInteractionFlags(Qt::TextBrowserInteraction);
    lab_name2->setOpenExternalLinks(true);

    edit_name = new QLineEdit(this);
    edit_name->setText(layer->getName());
    edit_name->setMaxLength(LAYER_NAME_MAX_LEN);

    // Geom type:

    QLabel *lab_geom_type = new QLabel(this);
    lab_geom_type->setText(tr("Geometry:"));

    QLabel *lab_geom_type2 = new QLabel(this);
    lab_geom_type2->setText(g_getGeomTypeDispName(layer->getGeomType()));

    combo_geom = new QComboBox(this);
    int i = 0;
    int cur_index;
    for (auto key: g_getGeomTypes().keys())
    {
        auto geom_type_data = g_getGeomTypes().value(key);
        combo_geom->addItem(geom_type_data.display_name, QVariant::fromValue(key));

        if (key == layer->getGeomType())
            cur_index = i;
        i++;
    }
    combo_geom->setCurrentIndex(cur_index);

    // Fields:

    QLabel *lab_fields = new QLabel(this);
    lab_fields->setText(tr("Fields:"));

    table = new CommonTableWidget(this, {""}, LAYER_MIN_FIELD_COUNT, LAYER_MAX_FIELD_COUNT,
                                  false, is_editable);
    table->setMaxStrSize(Core::FIELD_NAME_MAX_LEN);
    table->setColumnCount(2); // the real_column_count will stay = 1 so not to touch comboboxes
    table->setHorizontalHeaderLabels({tr("Name"), tr("Type")});
    if (is_editable)
    {
        combo = this->createFieldTypeCombo();
        table->setCellWidget(0, 1, combo);
        connect(table, &CommonTableWidget::beforeAddRow, this, &LayerDialog::onBeforeAddRow);
        connect(table, &CommonTableWidget::rowAdded, this, &LayerDialog::onRowAdded);
        connect(table, &CommonTableWidget::beforeRemoveRow, this, &LayerDialog::onBeforeRemoveRow);
    }

    int col_w = g_getSettings()->value(FB_STS_LAYERDLG_COL0_W, 0).toInt();
    if (col_w != 0)
        table->horizontalHeader()->resizeSection(0, col_w);

    // Buttons:

    QToolButton *but_add = this->createToolButton(
                ":/images/add.svg",
                tr("Add field from the last row"),
                &LayerDialog::onAddClicked);

    QToolButton *but_remove = this->createToolButton(
                ":/images/remove.svg",
                tr("Remove selected field"),
                &LayerDialog::onRemoveClicked);

    but_ok = new QPushButton(this);
    but_ok->setText(tr("OK"));
    but_ok->setFocusPolicy(Qt::NoFocus); // so not to exit accidentally
    connect(but_ok, &QPushButton::clicked, this, &LayerDialog::onOkClicked);

    // Layout:

    QGridLayout *lay_00 = new QGridLayout();
    lay_00->setHorizontalSpacing(10);

    if (res_group_name != "")
    {
        lay_00->addWidget(lab_res_gr_name, 0, 0);
        lay_00->addWidget(lab_res_gr_name2, 0, 1);
        QLabel *lab_stretch_gr = new QLabel(this);
        lab_stretch_gr->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        lay_00->addWidget(lab_stretch_gr, 1, 2);
    }
    else
    {
        lab_res_gr_name->hide();
        lab_res_gr_name2->hide();
    }

    lay_00->addWidget(lab_name, 2, 0);
    if (is_editable)
    {
        edit_name->show();
        lab_name2->hide();
        lay_00->addWidget(edit_name, 2, 1);
    }
    else
    {
        edit_name->hide();
        lab_name2->show();
        lay_00->addWidget(lab_name2, 2, 1);
    }
    QLabel *lab_stretch_name = new QLabel(this);
    lab_stretch_name->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    lay_00->addWidget(lab_stretch_name, 2, 2);

    lay_00->addWidget(lab_geom_type, 3, 0);
    if (is_editable)
    {
        combo_geom->show();
        lab_geom_type2->hide();
        lay_00->addWidget(combo_geom, 3, 1);
    }
    else
    {
        combo_geom->hide();
        lab_geom_type2->show();
        lay_00->addWidget(lab_geom_type2, 3, 1);
    }
    QLabel *lab_stretch_geom = new QLabel(this);
    lab_stretch_geom->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    lay_00->addWidget(lab_stretch_geom, 3, 2);

    lay_00->addWidget(new QLabel(this), 4, 0);

    lay_00->addWidget(lab_fields, 5, 0);

    QVBoxLayout *lay_11 = new QVBoxLayout();
    lay_11->addWidget(but_add);
    lay_11->addWidget(but_remove);
    lay_11->addStretch();

    QVBoxLayout *lay_10 = new QVBoxLayout();
    lay_10->addWidget(table);
    lay_10->addWidget(but_ok);

    QGridLayout *lay_grid = new QGridLayout(this);
    lay_grid->setContentsMargins(8, 8, 8, 8);
    lay_grid->setSpacing(5);
    lay_grid->addLayout(lay_00, 0, 0);
    lay_grid->addLayout(lay_10, 1, 0);
    lay_grid->addLayout(lay_11, 1, 1);

    // Put fields to the table:

    // Note: we do not use CommonTableWidget::putItems() here.
    auto fields = layer->getFields();
    for (int r = fields.size() - 1; r >= 0; r--)
    {
        QString f_name = fields[r]->getName();
        Core::FieldType f_type = fields[r]->getType();
        //bool has_data = fields[r].hasData();

        QTableWidgetItem *item1 = new QTableWidgetItem();
        item1->setText(f_name);
        item1->setData(Qt::UserRole, QVariant::fromValue(f_type));
        item1->setFlags(FB_LAYERTABLE_ITEM_DIS_FLAGS);
//        item1->setIcon(QIcon(has_data ? ":/images/data.svg" : ":/images/no_data.svg"));

        QTableWidgetItem *item2 = this->createFieldTypeItem(Core::g_getFieldTypeDispName(f_type));

        table->insertRow(0);
        table->setItem(0, 0, item1);
        table->setItem(0, 1, item2);
    }
}

LayerDialog::~LayerDialog ()
{
    g_getSettings()->setValue(FB_STS_LAYERDLG_W, this->width());
    g_getSettings()->setValue(FB_STS_LAYERDLG_H, this->height());
    g_getSettings()->setValue(FB_STS_LAYERDLG_COL0_W, table->horizontalHeader()->sectionSize(0));
}


void LayerDialog::getFields (QList<FieldInfo> &new_fields_info)
{
    new_fields_info.clear();

    for (int r = 0; r < table->getRealRowCount(); r++)
    {
        QString field_name = table->item(r, 0)->text();
        Core::FieldType field_type = table->item(r, 0)->data(Qt::UserRole).value<FieldType>();
        new_fields_info.append({field_name, "TEMP", field_type});
    }
}

void LayerDialog::getLayerMeta (QString &new_name, GeomType &new_geom_type)
{
    if (is_editable)
    {
        new_name = edit_name->text();
        new_geom_type = combo_geom->currentData(Qt::UserRole).value<GeomType>();
    }
}


void LayerDialog::onAddClicked ()
{
    table->addRowFromInputRow();
}

void LayerDialog::onRemoveClicked ()
{
    table->removeSelectedRow();
}

void LayerDialog::onOkClicked ()
{
    if (is_editable)
    {
        if (!Layer::isNameCorrect(edit_name->text()))
        {
            g_showWarning(this, tr("Unable to rename layer: the name contains incorrect symbols. "
                                   "Use only latin letters, numbers and '_' symbol. Name must also "
                                   "start from letter"));
            return;
        }

        // Add field from "input" row if some.
        table->addRowFromInputRow();

        // Check fields count.
//        if (table->getRealRowCount() == 0)
//        {
//            g_showMessage(this, tr("Please add at least one field before confirm"));
//            return;
//        }
    }

    this->accept();
}


void LayerDialog::onBeforeAddRow ()
{
    QString field_name = table->item(table->rowCount() - 1, 0)->text();

    // Check name correctness.
    if (!Core::Field::isNameCorrect(field_name))
    {
        g_showWarning(this, tr("Unable to add the field: the name contains incorrect symbols. "
                               "Use only latin letters, numbers and '_' symbol. Name must also "
                               "start from letter"));
        table->abortOperation();
        return;
    }

    // Check name uniqueness.
    if (this->containsFieldName(field_name))
    {
        g_showWarning(this, tr("Unable to add the field: the field with this name already exists"));
        table->abortOperation();
        return;
    }

    // Check restricted names.
    if (Layer::areNamesEqual(field_name, "id"))
    {
        g_showWarning(this, tr("Unable to add the field: the name \"ID\" is forbidden"));
        table->abortOperation();
        return;
    }
}

void LayerDialog::onRowAdded ()
{
    // Save selected field type in the table item.
    table->item(table->getRealRowCount() - 1, 0)->setData(Qt::UserRole,
                 QVariant::fromValue(combo->currentData(Qt::UserRole).value<Core::FieldType>()));

    // Get selected field type and add an item with it.
    table->setItem(table->getRealRowCount() - 1, 1,
                 this->createFieldTypeItem(combo->currentText()));

    // No field name change after parsing.
    // NOTE: if to change this in futre be sure that the name of the field after adding is correct.
    table->item(table->getRealRowCount() - 1, 0)->setFlags(FB_LAYERTABLE_ITEM_DIS_FLAGS);
}

void LayerDialog::onBeforeRemoveRow ()
{
    // Warn if data is going to be deleted with the field.
//    if (Core::Field::hasData(field_name))
//    {
//        g_showWarning(this, tr("This field contains data which is going to be deleted with the "
//                               "field. Do you really want to this?"));
//        table->abortOperation();
//        return;
//    }
}


template <typename Slot>
QToolButton *LayerDialog::createToolButton (QString icon_path, QString hint_text, Slot slot)
{
    QToolButton *but = new QToolButton(this);
    but->setIcon(QIcon(icon_path));
    but->setToolTip(hint_text);
    but->setFixedSize(DIALOG_TOOLB_SIZE, DIALOG_TOOLB_SIZE);
    if (!is_editable)
        but->setEnabled(false);
    connect(but, &QToolButton::clicked, this, slot);
    return but;
}


QComboBox *LayerDialog::createFieldTypeCombo ()
{
    QComboBox *combo = new QComboBox();
    combo->setFocusPolicy(Qt::NoFocus);
    for (auto key: Core::g_getFieldTypes().keys())
        combo->addItem(Core::g_getFieldTypeDispName(key), QVariant::fromValue(key));
    return combo;
}

QTableWidgetItem *LayerDialog::createFieldTypeItem (QString text)
{
    QTableWidgetItem *item = new QTableWidgetItem();
    item->setText(text);
    item->setTextColor({100, 100, 100});
    item->setFlags(FB_LAYERTABLE_ITEM_DIS_FLAGS);
    return item;
}


bool LayerDialog::containsFieldName (QString name)
{
    for (int r = 0; r < table->getRealRowCount(); r++)
    {
        if (Layer::areNamesEqual(table->item(r, 0)->text(), name))
            return true;
    }
    return false;
}



