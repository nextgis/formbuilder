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

void FBDialogProjectNgw::httpSelectedFinished ()
{
    if (httpSelectedReply->error() == QNetworkReply::NoError)
    {
        // Get the last needed information (metada) about NGW layer.
        // We do not check its correctness while it is done by project class at the
        // creation step.
        if (!receivedJson.empty())
        {
            Json::Reader json_reader;
            Json::Value json_root;
            if (json_reader.parse(receivedJson, json_root, false))
            {
                Json::Value json_fields;
                Json::Value json_new_fields;
                json_fields = json_root["feature_layer"]["fields"];
                for (int i=0; i<json_fields.size(); ++i)
                {
                    std::string str_keyname
                            = json_fields[i]["keyname"].asString();
                    std::string str_datatype
                            = json_fields[i]["datatype"].asString();
                    std::string str_display_name
                            = json_fields[i]["display_name"].asString();

                    Json::Value json_new_field;
                    json_new_field[FB_JSON_META_KEYNAME] = str_keyname;
                    json_new_field[FB_JSON_META_DATATYPE] = str_datatype;
                    json_new_field[FB_JSON_META_DISPLAY_NAME] = str_display_name;
                    json_new_fields.append(json_new_field);
                }
                jsonLayerMeta[FB_JSON_META_FIELDS] = json_new_fields;

                jsonLayerMeta[FB_JSON_META_GEOMETRY_TYPE]
                        = json_root["vector_layer"]["geometry_type"];

                jsonLayerMeta[FB_JSON_META_SRS]
                        = json_root["vector_layer"]["srs"];

                wLabelStatus->setText(tr("Connection successful"));
                httpSelectedReply->deleteLater();

                this->accept();

                return; // TODO: do we need return here?
            }
            else
            {
                // Unable to read fields decription.
                wLabelStatus->setText(tr("Error reading JSON"));
            }
        }
        else
        {
            // Unable to read fields decription.
            wLabelStatus->setText(tr("Error reading JSON"));
        }
    }
    else
    {
        wLabelStatus->setText(tr("Connection error"));
    }

    receivedJson = "";
    httpSelectedReply->deleteLater();
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
