/**************************************************************************************************
 *                                                                                                *
 *    Project:  NextGIS Formbuilder                                                               *
 *    Authors:  Mikhail Gusev, gusevmihs@gmail.com                                                *
 *              Copyright (C) 2014-2020 NextGIS                                                   *
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

#include "framework/access/access.h"
#include "framework/access/signdialog.h"

#define FB_DEFAULT_SCOPE "user_info.read" // see constructor in signbutton.h
#define FB_DEFAULT_AUTH_ENDPOINT "https://my.nextgis.com"

namespace Ui { class FbAuthDialog; }


class FbAuthDialog: public Fb::Gui::CustomDialog
{
    Q_OBJECT

    public:

     explicit FbAuthDialog (QWidget *parent);
     virtual ~FbAuthDialog ();

     void setNgAccountWidget (NGSignDialog *new_ng_dialog);
     void removeNgAccountWidget ();

    private slots:

     void onNeedToUpdateAccountInfo ();
     void onSignButtonClick ();
     void onEndpointEditFinished ();
     void onGroupSettingsToggled (bool checked);

    private:

     QString getEndpoint ();

     Ui::FbAuthDialog *ui;

     NGSignDialog *ng_dialog;
     QSize old_ng_dialog_size;
     Qt::WindowFlags old_ng_dialog_w_flags;
};



