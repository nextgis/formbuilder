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

#pragma once

#include "gui/custom_dialog.h"
#include "core/layer.h"

#include <QToolButton>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QKeyEvent>

namespace Fb
{
namespace Gui
{


class LayerDialog2: public CustomDialog
{
    Q_OBJECT

    public:

     explicit LayerDialog2 (QWidget *parent, const Core::Layer *layer, bool is_editable,
                            QString res_group_url, QString res_group_name);
     virtual ~LayerDialog2 ();

     void setOkButtonText (QString text) { but_ok->setText(text); }

     void getLayerMeta (QString &new_name, Core::GeomType &new_geom_type);

    protected slots:

     void onOkClicked ();

    protected:

     QLineEdit *edit_name;
     QComboBox *combo_geom;

     QPushButton *but_ok;

     const Core::Layer *layer;
};


}
}



