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

#include "layerdialog2.h"

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


LayerDialog2::LayerDialog2 (QWidget *parent, const Layer *layer, bool is_editable,
                            QString res_group_url, QString res_group_name):
    CustomDialog(parent),
    layer(layer)
{
    this->setWindowTitle(tr("Form properties"));

    res_group_name.replace("<", "&lt;");
    res_group_name.replace(">", "&gt;");

    // Resource group:

    QLabel *lab_res_gr_name = new QLabel(this);
    lab_res_gr_name->setText(tr("Resource group:"));

    QLabel *lab_res_gr_name2 = new QLabel(this);
    lab_res_gr_name2->setText(QString("<a href=\"%1\">%2</a>").arg(res_group_url).arg(res_group_name));
    lab_res_gr_name2->setToolTip(res_group_url);
    lab_res_gr_name2->setOpenExternalLinks(true);

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
        lab_name2->setOpenExternalLinks(true);
    }

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

    // Buttons:

    but_ok = new QPushButton(this);
    but_ok->setText(tr("OK"));
    but_ok->setFocusPolicy(Qt::NoFocus); // so not to exit accidentally
    connect(but_ok, &QPushButton::clicked, this, &LayerDialog2::onOkClicked);

    // Layout:

    QGridLayout *lay_grid = new QGridLayout(this);

    if (res_group_name != "")
    {
        lay_grid->addWidget(lab_res_gr_name, 0, 0);
        lay_grid->addWidget(lab_res_gr_name2, 0, 1);
        QLabel *lab_stretch_gr = new QLabel(this);
        lab_stretch_gr->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        lay_grid->addWidget(lab_stretch_gr, 1, 2);
    }
    else
    {
        lab_res_gr_name->hide();
        lab_res_gr_name2->hide();
    }

    lay_grid->addWidget(lab_name, 2, 0);
    if (is_editable)
    {
        edit_name->show();
        lab_name2->hide();
        lay_grid->addWidget(edit_name, 2, 1);
    }
    else
    {
        edit_name->hide();
        lab_name2->show();
        lay_grid->addWidget(lab_name2, 2, 1);
    }

    lay_grid->addWidget(lab_geom_type, 3, 0);
    if (is_editable)
    {
        combo_geom->show();
        lab_geom_type2->hide();
        lay_grid->addWidget(combo_geom, 3, 1);
    }
    else
    {
        combo_geom->hide();
        lab_geom_type2->show();
        lay_grid->addWidget(lab_geom_type2, 3, 1);
    }

    lay_grid->addWidget(but_ok, 4, 0, 1, 2);
}

LayerDialog2::~LayerDialog2 ()
{
}


void LayerDialog2::getLayerMeta (QString &new_name, GeomType &new_geom_type)
{
    new_name = edit_name->text();
    new_geom_type = combo_geom->currentData(Qt::UserRole).value<GeomType>();
}


void LayerDialog2::onOkClicked ()
{
    if (!Layer::isNameCorrect(edit_name->text()))
    {
        g_showWarning(this, tr("Unable to rename layer"));
        return;
    }

    this->accept();
}



