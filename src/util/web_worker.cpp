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

#include "web_worker.h"

#include <QEventLoop>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkCookieJar>
#include <QHttpMultiPart>

using namespace Fb;
using namespace Util;


WebWorker::WebWorker ():
    QObject(),
    cur_request(WebReqType::NoRequest),
    net_manager(new QNetworkAccessManager())
{
//    qDebug() << "Compiled against SSL library: " << QSslSocket::sslLibraryBuildVersionString();
//    qDebug() << "Using SSL library: " << QSslSocket::sslLibraryVersionString();

    // WARNING. setRedirectPolicy() disables correct NGW authorization (only gueset authorization
    // stays untouched). TODO: fix this.
//    net_manager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy); // since Qt 5.9
}

WebWorker::~WebWorker ()
{
    delete net_manager;
}


void WebWorker::startPost (QString url, QByteArray body, WebReqType req_type)
{
    error_text.clear();
    // TODO: clear j_reply here.

    cur_request = req_type;

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QVariant(QString("application/x-www-form-urlencoded")));
//    request.setHeader(QNetworkRequest::ContentTypeHeader,
//                      QVariant(QString("text/plain;charset=utf-8")));
    net_reply = net_manager->post(request, body);
    connect(net_reply, &QNetworkReply::finished, this, &WebWorker::onReplyFinished);
    connect(net_reply, &QNetworkReply::readyRead, this, &WebWorker::onReplyReadyRead);
}

void WebWorker::startMultipartPost (QString url, QHttpMultiPart *multipart)
{
    error_text.clear();
    // TODO: clear j_reply here.

    cur_request = WebReqType::Json;

    QNetworkRequest request(url);
    net_reply = net_manager->post(request, multipart);
    multipart->setParent(net_reply); // delete the multipart with the reply
    connect(net_reply, &QNetworkReply::finished, this, &WebWorker::onReplyFinished);
    connect(net_reply, &QNetworkReply::readyRead, this, &WebWorker::onReplyReadyRead);
}

void WebWorker::startGet (QString url, WebReqType req_type)
{
    error_text.clear();
    // TODO: clear j_reply here.

    cur_request = req_type;

    QNetworkRequest request(url);
    net_reply = net_manager->get(request);
    connect(net_reply, &QNetworkReply::finished, this, &WebWorker::onReplyFinished);
    connect(net_reply, &QNetworkReply::readyRead, this, &WebWorker::onReplyReadyRead);
}


void WebWorker::post (QString url, QByteArray body, WebReqType req_type)
{
    auto f = std::bind(&WebWorker::startPost, this, url, body, req_type);
    this->callSync(f);
}

void WebWorker::multipartPost (QString url, QHttpMultiPart *multipart)
{
    auto f = std::bind(&WebWorker::startMultipartPost, this, url, multipart);
    this->callSync(f);
}

void WebWorker::get (QString url, WebReqType req_type)
{
    auto f = std::bind(&WebWorker::startGet, this, url, req_type);
    this->callSync(f);
}


void WebWorker::callSync (std::function<void(void)> f)
{
    // TODO: timeout?

    QEventLoop event_loop;
    connect(this, &WebWorker::requestFinished, &event_loop, &QEventLoop::quit);
    f();
    event_loop.exec();
}


void WebWorker::onReplyReadyRead ()
{
    QByteArray ba;
    ba = net_reply->readAll();
    ba_reply += ba;
}

void WebWorker::onReplyFinished ()
{
    QJsonDocument j_doc = QJsonDocument::fromJson(ba_reply);

    if (net_reply->error() == QNetworkReply::NoError)
    {
        if (cur_request == WebReqType::Json)
        {
            if (!j_doc.isNull())
                j_reply = j_doc;
            else
                error_text = "Incorrect reply from server: not a JSON received";
        }
    }
//    else if () // e.g. 301 redirects
//    {
//    }
    else
    {
//        if (j_doc.object()["message"].isString())
            error_text = QString("[Server] %1").arg(j_doc.object()["message"].toString());
//        else
            error_text += QString("\n[QNetworkReply] %1").arg(QString::fromUtf8(net_reply->errorString().toUtf8().data()));
    }

    cur_request = WebReqType::NoRequest;
    ba_reply.clear();
    net_reply->deleteLater();

    emit requestFinished();
}





