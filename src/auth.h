/***************************************************************************************************
 *    Project:  NextGIS Formbuilder
 *    Author:   Mikhail Gusev, gusevmihs@gmail.com
 ***************************************************************************************************
 *    Copyright (C) 2014-2017 NextGIS
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
 **************************************************************************************************/

#ifndef AUTH_H
#define AUTH_H

#include <QObject>
#include <QString>
#include <QtNetworkAuth>


namespace Nextgis
{

const quint16 OAUTH2_REDIRECT_PORT = 8090;
const QUrl OAUTH2_AUTH_URL {"https://my.nextgis.com/oauth2/authorize/"};
const QUrl OAUTH2_ACCESSTOKEN_URL {"https://my.nextgis.com/oauth2/token/"};
const QString OAUTH2_USER_INFO_SCOPE {"user_info.read"};
#ifdef _OAUTH2_CLIENT_ID
const QString OAUTH2_CLIENT_ID = {_OAUTH2_CLIENT_ID}; // should be defined somewhere outside
#else
const QString OAUTH2_CLIENT_ID {""};
#endif

const QUrl SUPPORT_INFO_URL {"https://my.nextgis.com/api/v1/support_info/"};
const QUrl USER_INFO_URL {"https://my.nextgis.com/api/v1/user_info/"};


enum class AccountType
{
    Undefined,
    NotSupported,
    Supported
};

enum class Request
{
    Undefined,
    GetSuppportInfo,
    GetUserInfo
};


/**
 * @brief A class which represents a user in the NextGIS authenticaion system.
 */
class User: public QObject
{
    Q_OBJECT

public:
    User ();
    ~User ();
public:
    void startAuthentication ();
    QString getLastReplyError () const { return m_sLastReplyError; }
    AccountType getAccountType () const { return m_eAccountType; }

signals:
    void authFinished ();

private slots:
    void onOAuth2Granted ();
    void onReplyReadyRead ();
    void onReplyFinished ();
private:
    void startGetSupportInfo ();
private:
    QOAuth2AuthorizationCodeFlow *m_pAuth;
    QByteArray m_baReply;
    QNetworkReply *m_pReply;
    QString m_sLastReplyError;
    Nextgis::Request m_eCurrentRequest;
    bool m_bIsAuthorized;
    Nextgis::AccountType m_eAccountType;
    QString m_sUserName;
};

}

#endif //AUTH_H
