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

#include "newvoidprojectdialog.h"
#include "ui_newvoidprojectdialog.h"

using namespace Fb;
using namespace Gui;


NewVoidProjectDialog::NewVoidProjectDialog (QWidget *parent):
    CustomDialog(parent),
    ui(new Ui::NewVoidProjectDialog)
{
    ui->setupUi(this);

    connect(ui->but_ok, &QPushButton::clicked, this, &NewVoidProjectDialog::accept);

    this->putGeomTypes();
}


NewVoidProjectDialog::~NewVoidProjectDialog ()
{
    delete ui;
}


void NewVoidProjectDialog::putGeomTypes ()
{
    for (auto key: Core::g_getGeomTypes().keys())
    {
        auto geom_type = Core::g_getGeomTypes().value(key);
        ui->combo_geom->addItem(geom_type.display_name, QVariant::fromValue(key));
    }
}

Core::GeomType NewVoidProjectDialog::getGeomType() const
{
    QVariant var = ui->combo_geom->currentData(Qt::UserRole);
    return var.value<Core::GeomType>();
}



