/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  interface for working with NextGIS Web
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
 
#include "ngw.h"
#include "project/project_core.h"

FBDialogNgw::FBDialogNgw (QWidget *parent):
    QDialog (parent)
{
    this->setWindowTitle(tr("Establish NextGIS Web connection"));

    strUrl = "";
    strLogin = "";
    strPass = "";
    strId = "";
    strUrlName = "";

    availResTypes.append(FB_NGW_ITEMTYPE_RESOURCEGROUP);

    itemToExpand = NULL;

    QLabel* label;

    lMain = new QVBoxLayout(this);
    lMain->setSpacing(4);

    QGridLayout *lgLayout = new QGridLayout();
    lgLayout->setAlignment(Qt::AlignLeft);
    lgLayout->setSpacing(6);

    label = new QLabel(this);
    label->setText(tr("URL:")); //label->setText(tr("WebGIS name:"));
    lgLayout->addWidget(label,0,0);

    wEditUrl = new QLineEdit(this);
//QLabel *labUrl2 = new QLabel(this);
//labUrl2->setText(QString(" ") + FB_NGW_WEBGIS_SUFFIX);
    QHBoxLayout *hLayout1 = new QHBoxLayout();
    hLayout1->setSpacing(0);
    hLayout1->addWidget(wEditUrl);
//hLayout1->addWidget(labUrl2);
    lgLayout->addLayout(hLayout1,0,1);

//chbGuest = new QCheckBox(this);
//chbGuest->setChecked(true);
//chbGuest->setText(tr("Login as guest"));
//lgLayout->addWidget(chbGuest,1,1);
//connect(chbGuest,SIGNAL(clicked(bool)),this,SLOT(onCheckboxClick(bool)));

    label = new QLabel(this);
    label->setText(tr("Login:"));
    lgLayout->addWidget(label,2,0);
    wEditLogin = new QLineEdit(this);
    lgLayout->addWidget(wEditLogin,2,1);

    label = new QLabel(this);
    label->setText(tr("Password:"));
    lgLayout->addWidget(label,3,0);
    wEditPass = new QLineEdit(this);
    wEditPass->setEchoMode(QLineEdit::Password);
    lgLayout->addWidget(wEditPass,3,1);

    lMain->addLayout(lgLayout);

//this->onCheckboxClick(true);

    wButConnect = new QPushButton(this);
    wButConnect->setText(tr("Connect"));
    lMain->addWidget(wButConnect);
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

    lMain->addLayout(cancelLayout);
    lMain->addWidget(wTree);
    lMain->addWidget(wButSelect);
}


//void FBDialogProjectNgw::onCheckboxClick (bool pressed)
//{
//    wEditPass->setEnabled(!pressed);
//    wEditLogin->setEnabled(!pressed);
//}


void FBDialogNgw::onConnectClicked ()
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
//strUrlName = wEditUrl->text();
    strLogin = wEditLogin->text();
    strPass = wEditPass->text();

//if (chbGuest->isChecked()) { strLogin=""; strPass=""; }

    // Remove last '/' symbol:
    while (strUrl.endsWith("/"))
        strUrl.chop(1);

    // Add "http" prefix if needed:
//    if (!strUrl.startsWith("http://", Qt::CaseInsensitive))
//        strUrl.prepend("http://");

    // Add main url suffix:
//strUrl+=FB_NGW_WEBGIS_SUFFIX;

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
void FBDialogNgw::httpOnItemExpended (QTreeWidgetItem *treeItem)
{
    wButConnect->setEnabled(false);
    wTree->setEnabled(false);

    // Clear item to fill it from the beginning.
    QList<QTreeWidgetItem*> childrenToDel = treeItem->takeChildren();
    for (int i=0; i<childrenToDel.size(); i++)
    {
        delete childrenToDel[i];
    }

    itemToExpand = treeItem;
    wLabelStatus->setText(tr("Connecting ..."));
    wProgBar->setValue(50);

    receivedJson = "";
    QUrl url;
    int idStr = treeItem->data(0,Qt::UserRole).value<FBNgwResData>().first;
    url.setUrl(strUrl+"/resource/" + QString::number(idStr) + "/child/");
    QNetworkRequest request(url);
    httpResourceReply = httpManager.get(request);
    QObject::connect(httpResourceReply, SIGNAL(finished()),
            this, SLOT(httpResourceFinished()));
    QObject::connect(httpResourceReply, SIGNAL(readyRead()),
            this, SLOT(httpReadyResourceRead()));
}


void FBDialogNgw::httpOnItemClicked(QTreeWidgetItem *treeItem, int treeItemColumn)
{
    QVariant var = treeItem->data(0,Qt::UserRole);
    FBNgwResData itemData = var.value<FBNgwResData>();
    if (itemData.second == FB_NGW_ITEMTYPE_RESOURCEGROUP
            || itemData.second == FB_NGW_ITEMTYPE_UNDEFINED)
    {
        wButSelect->setEnabled(false);
    }
    else
    {
        wButSelect->setEnabled(true);
    }
}

void FBDialogNgw::httpOnItemCollapsed(QTreeWidgetItem *treeItem)
{
    wButSelect->setEnabled(false);
}


// Is called when press Select button.
// Make another request to the server. In the connected httpSelectedFinished() method
// the dialog will be closed if all was successful.
void FBDialogNgw::onSelectClicked ()
{
    // TODO: show the progress bar here, while the received JSON ~90Mb can
    // significantly increase waiting time.

    // As in other menthods we block buttons which can cause another requests.
    wButConnect->setEnabled(false);
    wTree->setEnabled(false);

    // Set the finally selected resource ID.
    QVariant var = wTree->currentItem()->data(0,Qt::UserRole);
    FBNgwResData itemData = var.value<FBNgwResData>();
    strId = QString::number(itemData.first);

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


void FBDialogNgw::httpReadyAuthRead ()
{
    QByteArray barr;
    barr = httpAuthReply->readAll();
}
void FBDialogNgw::httpReadyRead ()
{
    QByteArray barr;
    barr = httpReply->readAll();
    receivedJson += QString(barr).toStdString();
}
void FBDialogNgw::httpReadyResourceRead ()
{
    QByteArray barr;
    barr = httpResourceReply->readAll();
    receivedJson += QString(barr).toStdString();
}
void FBDialogNgw::httpReadySelectedRead ()
{
    QByteArray barr;
    barr = httpSelectedReply->readAll();
    receivedJson += QString(barr).toStdString();
}


void FBDialogNgw::httpAuthFinished ()
{
    if (httpAuthReply->error() == QNetworkReply::NoError)
    {
        httpAuthReply->deleteLater();
        wProgBar->setValue(50);

        QUrl url;
        url.setUrl(strUrl+"/resource/0/child/");
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
        wButSelect->setEnabled(false);
    }
    wButConnect->setEnabled(true);
    wTree->setEnabled(true);
}


void FBDialogNgw::httpFinished ()
{
    QList<QTreeWidgetItem*> newItems = parseJsonReply(httpReply);
    if (!newItems.isEmpty())
    {
        wTree->insertTopLevelItems(0,newItems);
        wLabelStatus->setText(tr("Connection successful"));
        wProgBar->setValue(100);
    }
    else
    {
        // TODO: move checks of http errors here because these lines override them!
        wLabelStatus->setText("");
        wProgBar->setValue(0);
    }
    httpReply->deleteLater();
    wButConnect->setEnabled(true);
    wTree->setEnabled(true);
}


void FBDialogNgw::httpResourceFinished ()
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
        // TODO: move checks of http errors here because these lines override them!
        wLabelStatus->setText("");
        wProgBar->setValue(0);
    }
    itemToExpand = NULL;
    httpResourceReply->deleteLater();
    wButConnect->setEnabled(true);
    wTree->setEnabled(true);
}


QList<QTreeWidgetItem*> FBDialogNgw::parseJsonReply (QNetworkReply *reply)
{
    // TODO: remove http error checks from this method!

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

                    QByteArray baType = jCls.asString().data();
                    QString strType = QString::fromUtf8(baType);
                    if (availResTypes.contains(strType))
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

                        QTreeWidgetItem *treeItem;
                        treeItem = new QTreeWidgetItem();
                        FBNgwResData resData;
                        resData.first = jId.asInt();
                        if (availResTypes.contains(strType))
                            resData.second = strType;
                        else
                            resData.second = FB_NGW_ITEMTYPE_UNDEFINED;
                        treeItem->setData(0,Qt::UserRole,QVariant::fromValue(resData));
                        treeItem->setText(0,sDispName
                                          + this->getResTypeDispPrefix(resData.second));
                        newItems.append(treeItem);

                        Json::Value jChildren = jRes["children"];
                        if (!jChildren.isNull()
                                && jChildren.asString() == "true"
                                && jCls.asString() == FB_NGW_ITEMTYPE_RESOURCEGROUP)
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


QString FBDialogNgw::getResTypeDispPrefix (QString resType)
{
    QString ret = "";
    if (resType == FB_NGW_ITEMTYPE_VECTORLAYER)
        ret = " [Vector]";
    else if (resType == FB_NGW_ITEMTYPE_POSTGISLAYER)
        ret = " [PostGIS]";
    else if (resType == FB_NGW_ITEMTYPE_LOOKUPTABLE)
        ret = " [Lookup table]";
    return ret;
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


