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

#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <functional>

namespace Fb
{
namespace Util
{


enum class WebReqType
{
    NoRequest,
    Authorize,
    Json
};


/*!
 * @brief A wrapper around QNetworkAccessManager which provides synchronous and asynchronous
 * versions of common HTTP methods with JSON replies.
 */
class WebWorker: public QObject
{
    Q_OBJECT

    public:

     WebWorker ();
     virtual ~WebWorker ();

     // Async.
     void startPost (QString url, QByteArray body, WebReqType req_type = WebReqType::Json);
     void startMultipartPost (QString url, QHttpMultiPart *multipart);
     void startGet (QString url, WebReqType req_type = WebReqType::Json);

     // Sync.
     void post (QString url, QByteArray body, WebReqType req_type = WebReqType::Json);
     void multipartPost (QString url, QHttpMultiPart *multipart);
     void get (QString url, WebReqType req_type = WebReqType::Json);

     // Results.
     const QJsonDocument &obtainJson () const { return j_reply; }
     const QString &obtainError () const { return error_text; }
     bool wasError () const { return error_text == "" ? false : true; }

    signals:

     void requestFinished ();

    protected:

     void callSync (std::function<void(void)> f);

     mutable QString error_text;
     QJsonDocument j_reply;

    private slots:

     void onReplyReadyRead ();
     void onReplyFinished ();

    private:

     QNetworkAccessManager *net_manager;
     QByteArray ba_reply;
     QNetworkReply *net_reply;

     WebReqType cur_request;
};


}
}
