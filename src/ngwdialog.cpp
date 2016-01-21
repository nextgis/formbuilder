/******************************************************************************
 *
 * Name:     ngwdialog.cpp
 * Project:  NextGIS Form Builder
 * Purpose:  Dialog for accessing NextGIS Web.
 * Author:   NextGIS
 *
 ******************************************************************************
 * Copyright (c) 2014, 2015 NextGIS
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#include "fb.h"


FBNgwDialog::FBNgwDialog(FB *parent): QDialog(parent)
{
    this->setStyleSheet("QWidget { color: black }");

    strUrl = "";
    strLogin = "";
    strPass = "";
    strId = "";

    itemToExpand = NULL;

    QVBoxLayout *dialogLayout = new QVBoxLayout(this);
    this->setWindowTitle(tr("Установите соединение с NextGIS Web"));
    QLabel* label;
    QHBoxLayout *hLayout;

    hLayout = new QHBoxLayout();
    label = new QLabel(this);
    label->setText(tr("URL: "));
    hLayout->addWidget(label);
    wEditUrl = new QLineEdit(this);
    wEditUrl->setText(parent->strLastNgwUrl);
    hLayout->addWidget(wEditUrl);
    dialogLayout->addLayout(hLayout);

     hLayout = new QHBoxLayout();
     label = new QLabel(this);
     label->setText(tr("Логин: "));
     hLayout->addWidget(label);
     wEditLogin = new QLineEdit(this);
     wEditLogin->setText(parent->strLastNgwLogin);
     hLayout->addWidget(wEditLogin);
     dialogLayout->addLayout(hLayout);

      hLayout = new QHBoxLayout();
      label = new QLabel(this);
      label->setText(tr("Пароль: "));
      hLayout->addWidget(label);
      wEditPass = new QLineEdit(this);
      wEditPass->setEchoMode(QLineEdit::Password);
      hLayout->addWidget(wEditPass);
      dialogLayout->addLayout(hLayout);

      wButConnect = new QPushButton(this);
      wButConnect->setText(tr("Соединить"));
      dialogLayout->addWidget(wButConnect);
      connect(wButConnect, SIGNAL(clicked()), this, SLOT(onConnectClicked()));

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
    wTree->setHeaderLabel(tr("Доступные ресурсы:"));
    wTree->setEnabled(false);
    connect(wTree, SIGNAL(itemExpanded(QTreeWidgetItem*)),
            this, SLOT(httpOnItemExpended(QTreeWidgetItem*)));
    connect(wTree, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
            this, SLOT(httpOnItemCollapsed(QTreeWidgetItem*)));
    connect(wTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(httpOnItemClicked(QTreeWidgetItem*,int)));

    wButSelect = new QPushButton(this);
    wButSelect->setText(tr("Выбрать"));
    wButSelect->setEnabled(false);
    connect(wButSelect, SIGNAL(clicked()),
            this, SLOT(onSelectClicked()));
    // Раньше завершение работы диалога висело на этой кнопке. Теперь он завершается
    // в слоте, связанным с приёмом последнего ответа с http-сервера.

    dialogLayout->addLayout(cancelLayout);
    dialogLayout->addWidget(wTree);
    dialogLayout->addWidget(wButSelect);
}


void FBNgwDialog::onConnectClicked ()
{
    wButConnect->setEnabled(false);
    wTree->setEnabled(false);
    wTree->clear();
    wLabelStatus->setText(tr("Идёт соединение..."));
    wProgBar->setValue(25);

    // Эти параметры, заданные пользователем, считываются только при нажатии на эту кнопку.
    strUrl = wEditUrl->text();
    strLogin = wEditLogin->text();
    strPass = wEditPass->text();

    // Обнуляем строку, т.к. дальше будет не единственное считывание, а несколько
    // считываний с конкатенацией (по мере прихода ответа с сервера).
    receivedJson = "";

    QUrl url;
    #ifdef FB_NGW_API_VERS
    url.setUrl(strUrl+"/login");
    #else
    //...
    #endif
    QNetworkRequest logPasRequest(url);
    QByteArray barr = QString("login=%1&password=%2").arg(strLogin).arg(strPass).toUtf8();
    // Хедер: "text/plain;charset=utf-8" не работает!
    logPasRequest.setHeader(QNetworkRequest::ContentTypeHeader,
                            QVariant(QString("application/x-www-form-urlencoded")));
    httpAuthReply = httpManager.post(logPasRequest,barr);
    connect(httpAuthReply, SIGNAL(finished()),
            this, SLOT(httpAuthFinished()));
    connect(httpAuthReply, SIGNAL(readyRead()),
            this, SLOT(httpReadyAuthRead()));
}


// Раскрыть элемент дерева для просмотра дочерних ресурсов данного ресурса.
void FBNgwDialog::httpOnItemExpended (QTreeWidgetItem *treeItem)
{
    wButConnect->setEnabled(false);
    wTree->setEnabled(false);

    // Очищаем весь элемент дерева, чтобы затем наполнить его заного.
    QList<QTreeWidgetItem*> childrenToDel = treeItem->takeChildren();
    for (int i=0; i<childrenToDel.size(); i++)
    {
        // Удаляем из словаря соотвествующий итем по ключу - его айдишнику.
        itemTypes.erase(childrenToDel[i]->data(0,Qt::UserRole).toInt());
        delete childrenToDel[i];
    }

    itemToExpand = treeItem;
    wLabelStatus->setText(tr("Идёт соединение..."));
    wProgBar->setValue(50);

    receivedJson = "";

    // Отправляем запрос на выдачу дочерних ресурсов для данного ресурса.
    QUrl url;
    #ifdef FB_NGW_API_VERS
    url.setUrl(strUrl+"/resource/" + treeItem->data(0,Qt::UserRole).toString()+ "/child/");
    #else
    // ...
    #endif
    QNetworkRequest request(url);
    httpResourceReply = httpManager.get(request);
    connect(httpResourceReply, SIGNAL(finished()),
            this, SLOT(httpResourceFinished()));
    connect(httpResourceReply, SIGNAL(readyRead()),
            this, SLOT(httpReadyResourceRead()));
}


void FBNgwDialog::httpOnItemCollapsed(QTreeWidgetItem *treeItem)
{
    wButSelect->setEnabled(false);
}


void FBNgwDialog::httpOnItemClicked(QTreeWidgetItem *treeItem, int treeItemColumn)
{
    int itemId = treeItem->data(0,Qt::UserRole).toInt();

    if (itemTypes[itemId] == FB_NGW_ITEM_TYPE_VECTORLAYER
        || itemTypes[itemId] == FB_NGW_ITEM_TYPE_POSTGISLAYER)
    {
        wButSelect->setEnabled(true);
    }
    else
    {
        wButSelect->setEnabled(false);
    }
}


// Вызывается при нажатии на кнопку "Выбрать".
// Делает итоговые действия по выбору ресурса на NextGIS Web сервере, делая ещё один
// запрос к http серверу. В методе по обработке этого запроса диалог завершает работу
// (если всё было успешно).
void FBNgwDialog::onSelectClicked ()
{
    // Так же как и везде перед выполенением http запроса - блокируем кнопки,
    // которые могут вызвать другие запросы.
    wButConnect->setEnabled(false);
    wTree->setEnabled(false);

    // Назначаем итоговый выбранный Id ресурса. Остальные параметры уже назначены.
    strId = wTree->currentItem()->data(0,Qt::UserRole).toString();

    // Из-за особенностей формата GeoJSON, а именно из-за того, что если в нём
    // нет самих данных - в возвращаемом с сервера NGW JSON-е может не
    // содержаться ни список полей, ни другая информация об ИД. Поэтому
    // необходимо достать всё это другим способом.
    // Делаем это тут, формируя нужный json данных. А в классе проекта
    // в специальном методе инициализации NGW-проекта будем только обращаться
    // к выбранному ИД через GDAL.

    receivedJson = "";

    // Делаем http запрос к серверу, чтобы получить нужные метаданные.
    QUrl url;
    #ifdef FB_NGW_API_VERS
    url.setUrl(strUrl + "/api/resource/" + strId);
    #else
    //...
    #endif
    QNetworkRequest request(url);
    httpSelectedReply = httpManager.get(request);
    connect(httpSelectedReply, SIGNAL(finished()),
            this, SLOT(httpSelectedFinished()));
    connect(httpSelectedReply, SIGNAL(readyRead()),
            this, SLOT(httpReadySelectedRead()));
}


void FBNgwDialog::httpReadyAuthRead ()
{
    QByteArray barr;
    barr = httpAuthReply->readAll();
}
void FBNgwDialog::httpReadyRead ()
{
    QByteArray barr;
    barr = httpReply->readAll();
    receivedJson += QString(barr).toStdString();
}
void FBNgwDialog::httpReadyResourceRead ()
{
    QByteArray barr;
    barr = httpResourceReply->readAll();
    receivedJson += QString(barr).toStdString();
}
void FBNgwDialog::httpReadySelectedRead ()
{
    QByteArray barr;
    barr = httpSelectedReply->readAll();
    receivedJson += QString(barr).toStdString();
}


void FBNgwDialog::httpAuthFinished ()
{
    if (httpAuthReply->error() == QNetworkReply::NoError)
    {
        httpAuthReply->deleteLater();
        wProgBar->setValue(50);

        QUrl url;
        #ifdef FB_NGW_API_VERS
        url.setUrl(strUrl+"/resource/0/child/");
        #else
        // ...
        //url.setUrl(QUrl(strUrl+"/resource/store/"));
        #endif
        QNetworkRequest request(url);
        httpReply = httpManager.get(request);
        connect(httpReply, SIGNAL(finished()),
                this, SLOT(httpFinished()));
        connect(httpReply, SIGNAL(readyRead()),
                this, SLOT(httpReadyRead()));
    }
    else
    {
        wLabelStatus->setText(tr("Ошибка соединения"));
        wProgBar->setValue(0);
    }
    wButConnect->setEnabled(true);
    wTree->setEnabled(true);
}


void FBNgwDialog::httpFinished ()
{
    QList<QTreeWidgetItem*> newItems = parseJsonReply(httpReply);
    if (!newItems.isEmpty())
    {
        wTree->insertTopLevelItems(0,newItems);
        wLabelStatus->setText(tr("Соединение успешно"));
        wProgBar->setValue(100);
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


void FBNgwDialog::httpResourceFinished ()
{
    QList<QTreeWidgetItem*> newItems = parseJsonReply(httpResourceReply);
    if (!newItems.isEmpty())
    {
        itemToExpand->insertChildren(0,newItems);
        wLabelStatus->setText(tr("Соединение успешно"));
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


void FBNgwDialog::httpSelectedFinished ()
{
    if (httpSelectedReply->error() == QNetworkReply::NoError)
    {
        // Парсим полученный json и заполняем оставшуюся информацию
        // о выбранном ИД - метаданные.
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
                jsonMeta[FB_JSON_META_FIELDS] = json_new_fields;

                jsonMeta[FB_JSON_META_GEOMETRY_TYPE]
                        = json_root["vector_layer"]["geometry_type"];

                jsonMeta[FB_JSON_META_SRS]
                        = json_root["vector_layer"]["srs"];

                // Завершаем работу диалога.
                wLabelStatus->setText(tr("Соединение успешно"));
                httpSelectedReply->deleteLater();

                // Сохраняем настройки подключения.
                // Можно преобразовать т.к. FB передавался в параметрах конструктора.
                static_cast<FB*>(this->parent())->strLastNgwUrl = wEditUrl->text();
                static_cast<FB*>(this->parent())->strLastNgwLogin = wEditLogin->text();

                this->accept();

                return; // TODO: нужен возврат здесь?
            }
            else
            {
                //"Не удалось считать описание полей из источника данных "
                wLabelStatus->setText(tr("Ошибка чтения JSON"));
            }
        }
        else
        {
            //"Не удалось считать описание полей из источника данных "
            wLabelStatus->setText(tr("Ошибка чтения JSON"));
        }
    }
    else
    {
        wLabelStatus->setText(tr("Ошибка соединения"));
    }

    receivedJson = "";
    httpSelectedReply->deleteLater();
}



QList<QTreeWidgetItem*> FBNgwDialog::parseJsonReply (QNetworkReply *reply)
{
    QList<QTreeWidgetItem*> newItems;

    if (reply->error() == QNetworkReply::NoError)
    {
        if (!receivedJson.empty())
        {
            Json::Value jResources; // will contains the root value after parsing.
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
                            // fromStdString() использует ASCII преобразование, поэтому:
                            sDispName = QString::fromUtf8(jDispName.asString().data());
                        }

                        int itemType = FB_NGW_ITEM_TYPE_UNDEFINED;
                        QString res_type = "";
                        if (jCls.asString() == "postgis_layer")
                        {
                            res_type = " [PostGIS]";
                            itemType = FB_NGW_ITEM_TYPE_POSTGISLAYER;
                        }
                        else if (jCls.asString() == "vector_layer")
                        {
                            res_type = " [Vector]";
                            itemType = FB_NGW_ITEM_TYPE_VECTORLAYER;
                        }
                        else if (jCls.asString() == "resource_group")
                        {
                            itemType = FB_NGW_ITEM_TYPE_RESOURCEGROUP;
                        }

                        QTreeWidgetItem *treeItem;
                        treeItem = new QTreeWidgetItem();
                        treeItem->setText(0,sDispName + res_type);
                        treeItem->setData(0,Qt::UserRole,jId.asInt());
                        newItems.append(treeItem);

                        // Добавляем в словарь id и тип итема, чтобы потом
                        // можно была разрешить/запретить выбор этого итема в качестве
                        // источника данных при работе с NGW-диалогом.
                        itemTypes.insert(std::make_pair(jId.asInt(),itemType));

                        Json::Value jChildren = jRes["children"];
                        if (!jChildren.isNull()
                                && jChildren.asString() == "true"
                                && jCls.asString() == "resource_group")
                        {
                            // Для того, чтобы в дальнейшем можно было раскрыть этот
                            // список.
                            treeItem->addChild(new QTreeWidgetItem(QStringList("to_delete")));
                        }
                    }
                }
            }
            else
            {
                wLabelStatus->setText(tr("Ошибка соединения"));
                wProgBar->setValue(0);
            }

        }
        else
        {
            wLabelStatus->setText(tr("Ошибка соединения"));
            wProgBar->setValue(0);
        }
    }
    else
    {
        wLabelStatus->setText(tr("Ошибка соединения"));
        wProgBar->setValue(0);
    }

    return newItems;
}


//void FBNgwDialog::HttpCancel()
//{
//
//}


//void FBNgwDialog::HttpSslErrors(QNetworkReply*,const QList<QSslError> &errors)
//{
//
//}


// Достать итоговый полный путь к json-ИД, выбранному пользователем, который можно
// передать в метод создания GDALDataset, а так же дополнительные параметры подключения
// к выбранному ресурсу NextGIS Web.
// Метод должен вызываться, когда диалог завершился по нажатию кнопки "Выбрать".
QString FBNgwDialog::selectedNgwResource (QString &strUrl, QString &strLogin, QString &strPass,
                                          QString &strId, Json::Value &jsonMeta)
{
    strUrl = this->strUrl; // Удалять последний '/', если он там был - не обязательно.
    strLogin = this->strLogin;
    strPass = this->strPass;
    strId = this->strId;
    jsonMeta = this->jsonMeta;

    QString ret = "";
    #ifdef FB_NGW_API_VERS
    ret = strUrl + "/resource/" + strId + "/geojson/";
    #else
    //...
    #endif
    return ret;
}

