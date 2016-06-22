/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  nextgisweb access dialog
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

#include "fb.h"

FBDialogProjectNgw::FBDialogProjectNgw (QWidget *parent, QString lastNgwUrl,
                                        QString lastNgwLogin):
    FBDialogNgw (parent)
{
    availResTypes.append(FB_NGW_ITEMTYPE_VECTORLAYER);
    availResTypes.append(FB_NGW_ITEMTYPE_POSTGISLAYER);

    wEditUrl->setText(lastNgwUrl);
    wEditLogin->setText(lastNgwLogin);
}

// Get the full path to JSON dataset (GeoJSON layer) which was selected by user. The
// returned string can be passed to GDALDataset creation. Also the additional
// NGW connection parameters are returned via method parameters.
// WARNING. Call this method only after the dialogue returns its end code.
QString FBDialogProjectNgw::getSelectedNgwResource (QString &strUrl, QString &strUrlName,
    QString &strLogin, QString &strPass, int &strId, Json::Value &jsonLayerMeta)
{
    strUrl = this->strUrl; // delete the last '/' (it is not neccessery)
    strUrlName = this->strUrlName;
    strLogin = this->strLogin;
    strPass = this->strPass;
    strId = this->strId.toInt(); // TODO: should we check for conversion errors?
    jsonLayerMeta = this->jsonLayerMeta;

    QString ret = "";
    ret = strUrl + "/resource/" + QString::number(strId) + "/geojson/";
    return ret;
}
