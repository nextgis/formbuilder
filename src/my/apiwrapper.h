/***************************************************************************************************
 *    Project:  NextGIS common libraries
 *    Author:   Mikhail Gusev, gusevmihs@gmail.com
 ***************************************************************************************************
 *    Copyright (C) 2017 NextGIS
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

#ifndef APIWRAPPER_H
#define APIWRAPPER_H

#include <QObject>
#include <QtNetworkAuth>

namespace Nextgis
{
namespace My
{


const quint16 OA2_REDIRECT_PORT = 8090;
const QUrl OA2_AUTH_URL {"https://my.nextgis.com/oauth2/authorize/"};
const QUrl OA2_ACCESS_TOKEN_URL {"https://my.nextgis.com/oauth2/token/"};
const QString OA2_USER_INFO_SCOPE {"user_info.read"};
#ifdef _OAUTH2_CLIENT_ID
const QString OA2_CLIENT_ID = {_OAUTH2_CLIENT_ID}; // should be defined somewhere outside
#else
const QString OA2_CLIENT_ID {""};
#endif

enum class AccountType
{
    Undefined,
    NotSupported,
    Supported
};

enum class DateType
{
    Start,
    End
};


/**
 * @brief my.nextgis.com API wrapper of version 1.0.
 * @details Using: call some startGetXxx() method and wait for the requestFinished signal. After
 * that you can call some obtainXxx() method to get the required information from the JSON reply.
 * DEVELOPERS: inherit this class to support other API versions.
 */
class ApiWrapper: public QObject
{
    Q_OBJECT

    public:

        ApiWrapper ();
        virtual ~ApiWrapper ();

        void setCallbackHtml (QString sFilePath);

        void setLastAccessToken (QString sLastAccessToken) { m_sLastAccessToken = sLastAccessToken; }
        void setLastRefreshToken (QString sLastRefreshToken) { m_sLastRefreshToken = sLastRefreshToken; }
        QString getLastAccessToken () { return m_sLastAccessToken; }
        QString getLastRefreshToken () { return m_sLastRefreshToken; }

        virtual void startAuthentication ();

        void startGet (QString sUrl);
        void startGetSupportInfo ();
        void startGetUserInfo ();

        QString obtainLastError () const { return m_sLastError; }
        QString obtainString (QString sJsonKey) const;

        virtual AccountType obtainAccountType () const;
        virtual QDate obtainDate (DateType eDateType) const;
        virtual QString obtainGuid () const;
        virtual QString obtainSign () const;
        virtual QString obtainLogin () const;

        bool isAuthenticated () const { return m_bIsAuthenticated; }

    signals:

        void authFinished ();
        void requestFinished ();

    protected:

        QString toReplyError (QString sMessage) const;

        QString m_sSupportInfoUrl;
        QString m_sUserInfoUrl;

        mutable QString m_sLastError;

    private slots:

        void onOAuth2Finished ();
        void onTokensReceived (const QVariantMap &mapTokens);

        void onReplyReadyRead ();
        void onReplyFinished ();

    private:

        QOAuth2AuthorizationCodeFlow *m_pAuthCodeFlow;
        QOAuthHttpServerReplyHandler *m_pAuthReplyHandler;

        bool m_bIsAuthenticated;

        QByteArray m_baReply;
        QNetworkReply *m_pReply;
        QJsonObject m_jReply;

        QString m_sLastAccessToken;
        QString m_sLastRefreshToken;
};


}
}

#endif //APIWRAPPER_H



