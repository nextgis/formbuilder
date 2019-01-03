/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  nextgisweb access dialog for layer creation
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

//#include <QLayout>

#include "fb.h"

FBDialogLayerNgw::FBDialogLayerNgw (QWidget *parent, QString lastNgwUrl,QString lastNgwLogin,
                                    QMap<QString,FBField> fields, FbGeomType *geomType):
    FBDialogNgw (parent)
{
    this->fields = fields;
    this->geomType = geomType;
    groupId = 0;

    wEditUrl->setText(lastNgwUrl);
    wEditLogin->setText(lastNgwLogin);

    wTree->setHeaderLabel(tr("Select resource group (optionally):"));

    QLabel *labName = new QLabel(this);
    labName->setText(tr("Layer name: "));

    leditName = new QLineEdit(this);
    leditName->setText("my_layer");

    QHBoxLayout *lName = new QHBoxLayout();
    lName->addWidget(labName);
    lName->addWidget(leditName);
    lMain->insertLayout(lMain->count()-1,lName); // before Select button

    // Reconnect signals to THIS (not parent) class.
    // TODO: make another correct classes for workig with NGW, because this approach is bad.
    QObject::disconnect(wTree,SIGNAL(itemClicked(QTreeWidgetItem*,int)),0,0);
    QObject::connect(wTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(httpOnItemClicked(QTreeWidgetItem*,int)));
    QObject::disconnect(wButSelect,SIGNAL(clicked()),0,0);
    QObject::connect(wButSelect, SIGNAL(clicked()),
            this, SLOT(onSelectClicked()));

    wButSelect->setText(tr("Create layer"));
}

void FBDialogLayerNgw::httpFinished ()
{
    // Allow to select no resource group - user may want to create layer in a root
    // resource of NGW instance.
    groupId = 0;
    if (httpReply->error() == QNetworkReply::NoError)
    {
        wButSelect->setEnabled(true);
    }
    else
    {
        wButSelect->setEnabled(false);
    }
    FBDialogNgw::httpFinished();
}

void FBDialogLayerNgw::httpOnItemClicked(QTreeWidgetItem *treeItem, int treeItemColumn)
{
    // Here listed only resource groups.
    QVariant var = treeItem->data(0,Qt::UserRole);
    FBNgwResData itemData = var.value<FBNgwResData>();
    groupId = itemData.first;
    wButSelect->setEnabled(true);
}

void FBDialogLayerNgw::onSelectClicked ()
{
    if (fields.contains("id") || fields.contains("ID") || fields.contains("Id")
            || fields.contains("iD"))
    {
        this->showMsg(tr("Unable to create layer! Reason: ") +
         tr("underlying layer contains field(s) with restricted name: \"id\""));
        return;
    }

    receivedJson = "";

    // Prepare data for layer creation.
    Json::Value jsonNull;
    QString strVecLay("vector_layer");
    QByteArray baVecLay = strVecLay.toUtf8();
    int nId = groupId;
    QString strDispName = leditName->text().toUtf8();
    QByteArray baDispName = strDispName.toUtf8();
    int nSrsId = 3857;
    QString strGeomType = geomType->aliasNgw.toUtf8();
    QByteArray baGeomType = strGeomType.toUtf8();
    QList<QByteArray> strsFields;
    QMap<QString,FBField>::const_iterator it = fields.constBegin();
    while (it != fields.constEnd())
    {
        strsFields.append(it.key().toUtf8());
        strsFields.append(it.value().datataype->aliasNgw.toUtf8());
        strsFields.append(it.value().display_name.toUtf8());
        ++it;
    }

    // Form data for request.
    Json::Value jsonData;
    Json::Value jRes;
    jRes["cls"] = baVecLay.data();
    Json::Value jParent;
    jParent["id"] = nId;
    jRes["parent"] = jParent;
    jRes["display_name"] = baDispName.data();
    jRes["keyname"] = jsonNull;
    jRes["description"] = jsonNull;
    jsonData["resource"] = jRes;
    Json::Value jVl;
    Json::Value jSrs;
    jSrs["id"] = nSrsId;
    jVl["srs"] = jSrs;
    jVl["geometry_type"] = baGeomType.data();
    Json::Value jFields;
    for (int i=0; i<strsFields.size(); i=i+3)
    {
        Json::Value jField;
        jField["keyname"] = strsFields[i].data();
        jField["datatype"] = strsFields[i+1].data();
        jField["display_name"] = strsFields[i+2].data();
        jFields.append(jField);
    }
    jVl["fields"] = jFields;
    jsonData["vector_layer"] = jVl;
    QByteArray baData = jsonData.toStyledString().data();

    // Save known parameters so not to do it further.
    jsonLayerMeta[FB_JSON_META_FIELDS] = jFields;
    jsonLayerMeta[FB_JSON_META_GEOMETRY_TYPE] = baGeomType.data();
    jsonLayerMeta[FB_JSON_META_SRS] = jSrs;

    // Form request.
    QUrl url;
    url.setUrl(strUrl + "/api/resource/");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QString::fromUtf8("application/json"));

    httpSelectedReply = httpManager.post(request,baData);
    QObject::connect(httpSelectedReply, SIGNAL(finished()),
            this, SLOT(httpSelectedFinished()));
    QObject::connect(httpSelectedReply, SIGNAL(readyRead()),
            this, SLOT(httpReadySelectedRead()));
}

void FBDialogLayerNgw::httpSelectedFinished ()
{
    Json::Value json_root;
    Json::Reader json_reader;
    bool reply_parsed = json_reader.parse(receivedJson, json_root, false);

    if (httpSelectedReply->error() == QNetworkReply::NoError)
    {
        // Get the only new parameter of the newly created layer.
        Json::Value jId = json_root["id"];
        if (jId.isInt())
        {
            strId = QString::number(jId.asInt());
            this->accept();
        }
        else
        {
            this->showMsg(tr("Unable to create layer! Reason: ")
              + tr("unable to get id of newly created layer"));
        }
    }

    else
    {
        QString msgErr = "";
        if (reply_parsed)
        {
            msgErr = json_root.toStyledString().data();
        }
        this->showMsg(tr("Unable to create layer! Reason: ") + '\n' + msgErr);
    }

    receivedJson = "";
    httpSelectedReply->deleteLater();
}


QString FBDialogLayerNgw::getSelectedNgwResource (QString &strUrl, QString &strUrlName,
    QString &strLogin, QString &strPass, int &strId, Json::Value &jsonLayerMeta)
{
    strUrl = this->strUrl; // delete the last '/' (it is not neccessery)
    strUrlName = this->strUrlName;
    strLogin = this->strLogin;
    strPass = this->strPass;
    strId = this->strId.toInt(); // TODO: should we check for conversion errors?
    jsonLayerMeta = this->jsonLayerMeta;
    QString ret = "";
    //ret = strUrl + "/resource/" + QString::number(strId) + "/geojson/";
    ret = strUrl + "/api/resource/" + QString::number(strId) + "/geojson";
    return ret;
}


void FBDialogLayerNgw::showMsg (QString msg)
{
    QMessageBox msgBox(this);
    msgBox.setText(msg);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setWindowTitle(tr("Info"));
    QMessageBox::Icon icon;
    icon = QMessageBox::Information;
    msgBox.setIcon(icon);
    msgBox.exec();
}


