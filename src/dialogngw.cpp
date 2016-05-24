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


FBDialogProjectNgw::~FBDialogProjectNgw()
{
}


FBDialogProjectNgw::FBDialogProjectNgw (QWidget *parent,
                                        QString lastNgwUrl, QString lastNgwLogin):
    QDialog (parent)
{
//    this->setStyleSheet("");
//    this->setStyleSheet("QWidget { color: black }");
    this->setWindowTitle(tr("Establish NextGIS Web connection ..."));

    strUrl = "";
    strLogin = "";
    strPass = "";
    strId = "";

    itemToExpand = NULL;

    QVBoxLayout *dialogLayout = new QVBoxLayout(this);
    QLabel* label;
    QHBoxLayout *hLayout;

    hLayout = new QHBoxLayout();
    hLayout->setSpacing(0);
    label = new QLabel(this);
label->setText("Web GIS name:     ");//(tr("URL:          "));
    hLayout->addWidget(label);
    wEditUrl = new QLineEdit(this);
    wEditUrl->setText(lastNgwUrl);
QLabel *labUrl2 = new QLabel(this);
labUrl2->setText(QString(" ") + FB_NGW_WEBGIS_SUFFIX);
    hLayout->addWidget(wEditUrl);
hLayout->addWidget(labUrl2);
    dialogLayout->addLayout(hLayout);

hLayout = new QHBoxLayout();
hLayout->setAlignment(Qt::AlignLeft);
chbGuest = new QCheckBox(this);
chbGuest->setChecked(true);
chbGuest->setText(tr("Login as guest"));
hLayout->addSpacing(89);
hLayout->addWidget(chbGuest);
dialogLayout->addLayout(hLayout);
connect(chbGuest,SIGNAL(clicked(bool)),this,SLOT(onCheckboxClick(bool)));

    hLayout = new QHBoxLayout();
    label = new QLabel(this);
label->setText(tr("Login:                  "));//(tr("Login:        "));
    hLayout->addWidget(label);
    wEditLogin = new QLineEdit(this);
    wEditLogin->setText(lastNgwLogin);
    hLayout->addWidget(wEditLogin);
    dialogLayout->addLayout(hLayout);

    hLayout = new QHBoxLayout();
    label = new QLabel(this);
label->setText(tr("Password:           "));//(tr("Password: "));
    hLayout->addWidget(label);
    wEditPass = new QLineEdit(this);
    wEditPass->setEchoMode(QLineEdit::Password);
    hLayout->addWidget(wEditPass);
    dialogLayout->addLayout(hLayout);

this->onCheckboxClick(true);

    wButConnect = new QPushButton(this);
    wButConnect->setText(tr("Connect"));
    dialogLayout->addWidget(wButConnect);
    QObject::connect(wButConnect, SIGNAL(clicked()),
            this, SLOT(onConnectClicked()));

    QHBoxLayout *cancelLayout = new QHBoxLayout();
    wLabelStatus = new QLabel(this);
    wLabelStatus->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    wLabelStatus->setText("");
    wProgBar = new QProgressBar(this);
    wProgBar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    wProgBar->setValue(0);
    wProgBar->setMaximum(100);
    wProgBar->setTextVisible(false);
    cancelLayout->addWidget(wLabelStatus);
    cancelLayout->addWidget(wProgBar);

    wTree = new QTreeWidget(this);
    wTree->setColumnCount(1);
    wTree->setHeaderLabel(tr("Available resources:"));
    wTree->setEnabled(false);
    QObject::connect(wTree, SIGNAL(itemExpanded(QTreeWidgetItem*)),
            this, SLOT(httpOnItemExpended(QTreeWidgetItem*)));
    QObject::connect(wTree, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
            this, SLOT(httpOnItemCollapsed(QTreeWidgetItem*)));
    QObject::connect(wTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(httpOnItemClicked(QTreeWidgetItem*,int)));

    wButSelect = new QPushButton(this);
    wButSelect->setText(tr("Select"));
    wButSelect->setEnabled(false);
    QObject::connect(wButSelect, SIGNAL(clicked()),
            this, SLOT(onSelectClicked()));

    // TODO: make actual button which can cancel http connection process.
    // For now user can close the dialog or press Connect button again, because all
    // connections are asynchronous.
    //wButCancel = new QPushButton(this);

    dialogLayout->addLayout(cancelLayout);
    dialogLayout->addWidget(wTree);
    dialogLayout->addWidget(wButSelect);
}


void FBDialogProjectNgw::onCheckboxClick (bool pressed)
{
    wEditPass->setEnabled(!pressed);
    wEditLogin->setEnabled(!pressed);
}


void FBDialogProjectNgw::onConnectClicked ()
{
    wButConnect->setEnabled(false);
    wTree->setEnabled(false);
    wTree->clear();
    wLabelStatus->setText(tr("Connecting ..."));
    wProgBar->setValue(25);

    // We must clear json string, while there will be several read actions with
    // string concatination next (as replys from server returned).
    receivedJson = "";

    // These parameters are read once only when user clicks connect button.
    strUrl = wEditUrl->text();
    strLogin = wEditLogin->text();
    strPass = wEditPass->text();

if (chbGuest->isChecked()) { strLogin=""; strPass=""; }

    // Remove last '/' symbol:
    while (strUrl.endsWith("/"))
        strUrl.chop(1);

    // Add "http" prefix if needed:
    if (!strUrl.startsWith("http://",Qt::CaseInsensitive))
        strUrl.prepend("http://");

// Add main erl suffix:
strUrl+=FB_NGW_WEBGIS_SUFFIX;

    QUrl url;
    url.setUrl(strUrl+"/login");

    // NOTE: header "text/plain;charset=utf-8" does not work for the following.
    QNetworkRequest logPasRequest(url);
    QByteArray barr = QString("login=%1&password=%2").arg(strLogin).arg(strPass).toUtf8();
    logPasRequest.setHeader(QNetworkRequest::ContentTypeHeader,
                            QVariant(QString("application/x-www-form-urlencoded")));
    httpAuthReply = httpManager.post(logPasRequest,barr);
    QObject::connect(httpAuthReply, SIGNAL(finished()),
            this, SLOT(httpAuthFinished()));
    QObject::connect(httpAuthReply, SIGNAL(readyRead()),
            this, SLOT(httpReadyAuthRead()));
}


// Expand tree elem for show the child resources of the current resource.
void FBDialogProjectNgw::httpOnItemExpended (QTreeWidgetItem *treeItem)
{
    wButConnect->setEnabled(false);
    wTree->setEnabled(false);

    // Clear tree elem to fill it from the beginning.
    QList<QTreeWidgetItem*> childrenToDel = treeItem->takeChildren();
    for (int i=0; i<childrenToDel.size(); i++)
    {
        // Delete according item from the dictionary by its id.
        itemTypes.erase(childrenToDel[i]->data(0,Qt::UserRole).toInt());
        delete childrenToDel[i];
    }

    itemToExpand = treeItem;
    wLabelStatus->setText(tr("Connecting ..."));
    wProgBar->setValue(50);

    receivedJson = "";
    QUrl url;
    url.setUrl(strUrl+"/resource/" + treeItem->data(0,Qt::UserRole).toString()+ "/child/");
    QNetworkRequest request(url);
    httpResourceReply = httpManager.get(request);
    QObject::connect(httpResourceReply, SIGNAL(finished()),
            this, SLOT(httpResourceFinished()));
    QObject::connect(httpResourceReply, SIGNAL(readyRead()),
            this, SLOT(httpReadyResourceRead()));
}


void FBDialogProjectNgw::httpOnItemCollapsed(QTreeWidgetItem *treeItem)
{
    wButSelect->setEnabled(false);
}


void FBDialogProjectNgw::httpOnItemClicked(QTreeWidgetItem *treeItem, int treeItemColumn)
{
    int itemId = treeItem->data(0,Qt::UserRole).toInt();

    if (itemTypes[itemId] == FB_NGW_ITEMTYPE_VECTORLAYER
        || itemTypes[itemId] == FB_NGW_ITEMTYPE_POSTGISLAYER)
    {
        wButSelect->setEnabled(true);
    }
    else
    {
        wButSelect->setEnabled(false);
    }
}


// Is called when press Select button.
// Make another request to the server. In the connected httpSelectedFinished() method
// the dialogue will be closed if all was successful.
void FBDialogProjectNgw::onSelectClicked ()
{
    // TODO: show the progress bar here, while the received JSON ~ 90Mb can
    // significantly increase waiting time.

    // As in other menthods we block buttons which can cause another requests.
    wButConnect->setEnabled(false);
    wTree->setEnabled(false);

    // Set the finally selected resource ID.
    strId = wTree->currentItem()->data(0,Qt::UserRole).toString();

    // The returned JSON may not contain layer's metadata: e.g. list of fields, because
    // of structure of the GeoJSON vector format - if there is no data in a layer.
    // That's why we must get here such metadata in a NextGISWeb-specific form.
    // Do it here and not in the project creation method so not to work with http in
    // the project class/module.
    receivedJson = "";
    QUrl url;
    url.setUrl(strUrl + "/api/resource/" + strId);
    QNetworkRequest request(url);
    httpSelectedReply = httpManager.get(request);
    QObject::connect(httpSelectedReply, SIGNAL(finished()),
            this, SLOT(httpSelectedFinished()));
    QObject::connect(httpSelectedReply, SIGNAL(readyRead()),
            this, SLOT(httpReadySelectedRead()));
}


void FBDialogProjectNgw::httpReadyAuthRead ()
{
    QByteArray barr;
    barr = httpAuthReply->readAll();
}
void FBDialogProjectNgw::httpReadyRead ()
{
    QByteArray barr;
    barr = httpReply->readAll();
    receivedJson += QString(barr).toStdString();
}
void FBDialogProjectNgw::httpReadyResourceRead ()
{
    QByteArray barr;
    barr = httpResourceReply->readAll();
    receivedJson += QString(barr).toStdString();
}
void FBDialogProjectNgw::httpReadySelectedRead ()
{
    QByteArray barr;
    barr = httpSelectedReply->readAll();
    receivedJson += QString(barr).toStdString();
}


void FBDialogProjectNgw::httpAuthFinished ()
{
    if (httpAuthReply->error() == QNetworkReply::NoError)
    {
        httpAuthReply->deleteLater();
        wProgBar->setValue(50);

        QUrl url;
        url.setUrl(strUrl+"/resource/0/child/");
        //url.setUrl(QUrl(strUrl+"/resource/store/"));
        QNetworkRequest request(url);
        httpReply = httpManager.get(request);
        QObject::connect(httpReply, SIGNAL(finished()),
                this, SLOT(httpFinished()));
        QObject::connect(httpReply, SIGNAL(readyRead()),
                this, SLOT(httpReadyRead()));
    }
    else
    {
        wLabelStatus->setText(tr("Connection error"));
        wProgBar->setValue(0);
    }
    wButConnect->setEnabled(true);
    wTree->setEnabled(true);
}


void FBDialogProjectNgw::httpFinished ()
{
    QList<QTreeWidgetItem*> newItems = parseJsonReply(httpReply);
    if (!newItems.isEmpty())
    {
        wTree->insertTopLevelItems(0,newItems);
        wLabelStatus->setText(tr("Connection successful"));
        wProgBar->setValue(100);
        // Save connection settings.
        emit updateNgwSettings(wEditUrl->text(),wEditLogin->text());
    }
    else
    {
        wLabelStatus->setText("");
        wProgBar->setValue(0);
    }
    httpReply->deleteLater();
    wButConnect->setEnabled(true);
    wTree->setEnabled(true);
}


void FBDialogProjectNgw::httpResourceFinished ()
{
    QList<QTreeWidgetItem*> newItems = parseJsonReply(httpResourceReply);
    if (!newItems.isEmpty())
    {
        itemToExpand->insertChildren(0,newItems);
        wLabelStatus->setText(tr("Connection successful"));
        wProgBar->setValue(100);
    }
    else
    {
        wLabelStatus->setText("");
        wProgBar->setValue(0);
    }
    itemToExpand = NULL;
    httpResourceReply->deleteLater();
    wButConnect->setEnabled(true);
    wTree->setEnabled(true);
}


void FBDialogProjectNgw::httpSelectedFinished ()
{
    if (httpSelectedReply->error() == QNetworkReply::NoError)
    {
        // Get the last needed information (metada) about NGW layer.
        // We do not check its correctness while it is done py project class at the
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



QList<QTreeWidgetItem*> FBDialogProjectNgw::parseJsonReply (QNetworkReply *reply)
{
    QList<QTreeWidgetItem*> newItems;

    if (reply->error() == QNetworkReply::NoError)
    {
        if (!receivedJson.empty())
        {
            Json::Value jResources; // will contain the root value after parsing
            Json::Reader jReader;
            if (jReader.parse(receivedJson, jResources, false))
            {
                for (int index = 0; index < jResources.size(); ++index)
                {
                    Json::Value jResource = jResources[index];
                    if (jResource.isNull())
                        continue;

                    Json::Value jRes = jResource["resource"];
                    if (jRes.isNull())
                        continue;

                    Json::Value jCls = jRes["cls"];
                    if (jCls.isNull())
                        continue;

                    if (jCls.asString() == "resource_group" ||
                            jCls.asString() == "postgis_layer" ||
                            jCls.asString() == "vector_layer")
                    {
                        Json::Value jId = jRes["id"];
                        if (jId.isNull())
                            continue;

                        QString sDispName;
                        Json::Value jDispName = jRes["display_name"];
                        if (jDispName.isNull())
                        {
                            sDispName = QString("<resource-no-name>");
                        }
                        else
                        {
                            // fromStdString() uses ASCII conversion, so:
                            sDispName = QString::fromUtf8(jDispName.asString().data());
                        }

                        int itemType = FB_NGW_ITEMTYPE_UNDEFINED;
                        QString res_type = "";
                        if (jCls.asString() == "postgis_layer")
                        {
                            res_type = " [PostGIS]";
                            itemType = FB_NGW_ITEMTYPE_POSTGISLAYER;
                        }
                        else if (jCls.asString() == "vector_layer")
                        {
                            res_type = " [Vector]";
                            itemType = FB_NGW_ITEMTYPE_VECTORLAYER;
                        }
                        else if (jCls.asString() == "resource_group")
                        {
                            itemType = FB_NGW_ITEMTYPE_RESOURCEGROUP;
                        }

                        QTreeWidgetItem *treeItem;
                        treeItem = new QTreeWidgetItem();
                        treeItem->setText(0,sDispName + res_type);
                        treeItem->setData(0,Qt::UserRole,jId.asInt());
                        newItems.append(treeItem);

                        // Add ID and item type to the special dictionary, so later we
                        // can allow/deny this item selection as the dataset.
                        itemTypes.insert(std::make_pair(jId.asInt(),itemType));

                        Json::Value jChildren = jRes["children"];
                        if (!jChildren.isNull()
                                && jChildren.asString() == "true"
                                && jCls.asString() == "resource_group")
                        {
                            // So later we can expand this list.
                            treeItem->addChild(new QTreeWidgetItem(
                                                   QStringList("to_delete")));
                        }
                    }
                }
            }
            else
            {
                wLabelStatus->setText(tr("Connection error"));
                wProgBar->setValue(0);
            }

        }
        else
        {
            wLabelStatus->setText(tr("Connection error"));
            wProgBar->setValue(0);
        }
    }
    else
    {
        wLabelStatus->setText(tr("Connection error"));
        wProgBar->setValue(0);
    }

    return newItems;
}


//void FBDialogProjectNgw::HttpCancel()
//{
//
//}


//void FBDialogProjectNgw::HttpSslErrors(QNetworkReply*,
//                                       const QList<QSslError> &errors)
//{
//
//}


// Get the full path to JSON dataset (GeoJSON layer) which was selected by user. The
// returned string can be passed to GDALDataset creation. Also the additional
// NGW connection parameters are returned via method parameters.
// WARNING. Call this method only after the dialogue returns its end code.
QString FBDialogProjectNgw::getSelectedNgwResource (QString &strUrl, QString &strLogin,
                       QString &strPass, int &strId, Json::Value &jsonLayerMeta)
{
    strUrl = this->strUrl; // delete the last '/' (it is not neccessery)
    strLogin = this->strLogin;
    strPass = this->strPass;
    strId = this->strId.toInt(); // TODO: should we check for conversion errors?
    jsonLayerMeta = this->jsonLayerMeta;

    QString ret = "";
    ret = strUrl + "/resource/" + QString::number(strId) + "/geojson/";
    return ret;
}
