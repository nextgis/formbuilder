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

#pragma once

#include "gui/custom_dialog.h"
#include "languages.h"

#include "framework/access/signdialog.h"

namespace Ui { class FbAboutDialog; }


class FbAboutDialog: public Fb::Gui::CustomDialog
{
    Q_OBJECT

    public:

     explicit FbAboutDialog (QWidget *parent, Language language);
     virtual ~FbAboutDialog ();

     void setNgAccountWidget (NGSignDialog *new_ng_dialog);
     void removeNgAccountWidget ();

    private slots:

     void onNeedToUpdateAccountInfo ();

    private:

     Ui::FbAboutDialog *ui;

     NGSignDialog *ng_dialog;
     QSize old_ng_dialog_size;
     Qt::WindowFlags old_ng_dialog_w_flags;
};



