/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  NextGIS Web lookup table selection dialog
 * Author:   Mikhail Gusev, gusevmihs@gmail.com
 ******************************************************************************
*   Copyright (C) 2014-2016 NextGIS
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 2 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#include "attributes.h"

FBDialogDictionaryNgw::FBDialogDictionaryNgw (QWidget *parent, QString curNgwUrl,
                                              QString curNgwLogin, QString curNgwPass):
    FBDialogNgw (parent)
{
    this->setWindowTitle(tr("Select NextGIS Web lookup table ..."));

    availResTypes.append(FB_NGW_ITEMTYPE_LOOKUPTABLE);

    wEditUrl->setText(curNgwUrl);
    wEditLogin->setText(curNgwLogin);
    wEditPass->setText(curNgwPass);
    wEditUrl->setEnabled(false);
    wEditLogin->setEnabled(false);
    wEditPass->setEnabled(false);

    this->onConnectClicked(); // connect to the current ngw instance immidiately
}

