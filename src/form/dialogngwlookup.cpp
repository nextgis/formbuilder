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

FBDialogLookupNgw::FBDialogLookupNgw (QWidget *parent, QString curNgwUrl,
                                              QString curNgwLogin, QString curNgwPass):
    FBDialogNgw (parent)
{  
    this->setWindowTitle(tr("Select NextGIS Web lookup table"));

    availResTypes.append(FB_NGW_ITEMTYPE_LOOKUPTABLE);

    wEditUrl->setText(curNgwUrl);
    wEditLogin->setText(curNgwLogin);
    wEditPass->setText(curNgwPass);
    wEditUrl->setEnabled(false);
    wEditLogin->setEnabled(false);
    wEditPass->setEnabled(false);

    this->onConnectClicked(); // connect to the current ngw instance immidiately
}


void FBDialogLookupNgw::httpSelectedFinished ()
{
    if (httpSelectedReply->error() == QNetworkReply::NoError)
    {
        if (!receivedJson.empty())
        {
            Json::Reader json_reader;
            Json::Value json_root;
            if (json_reader.parse(receivedJson, json_root, false))
            {
                Json::Value json_items = json_root["lookup_table"]["items"];
                Json::Value::Members mems = json_items.getMemberNames();

                selectedLookup.clear();
                for (int i=0; i<mems.size(); i++)
                {
                    QString strName = QString::fromUtf8(mems[i].data());
                    QString strValue = QString::fromUtf8(json_items[mems[i]]
                                                        .asString().data());
                    selectedLookup.append(QPair<QString,QString>(strName,strValue));
                }

                wLabelStatus->setText(tr("Connection successful"));
                httpSelectedReply->deleteLater();

                this->accept();

                return; // TODO: do we need return here?
            }
            else
            {
                wLabelStatus->setText(tr("Error reading JSON"));
            }
        }
        else
        {
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


int FBDialogLookupNgw::getSelectedLookupTable(QList<QPair<QString,QString> > &list)
{
    list = this->selectedLookup;

    return strId.toInt();
}

