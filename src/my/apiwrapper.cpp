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
 
#include "apiwrapper.h"

using namespace Nextgis::My;


/**
 * @brief Constructor.
 */
ApiWrapper::ApiWrapper ():
    m_bIsAuthenticated(false),
    m_sSupportInfoUrl("https://my.nextgis.com/api/v1/support_info/"),
    m_sUserInfoUrl("https://my.nextgis.com/api/v1/user_info/"),
    m_sLastAccessToken(""),
    m_sLastRefreshToken("")
{
    m_pAuthCodeFlow = new QOAuth2AuthorizationCodeFlow();
    m_pAuthReplyHandler = new QOAuthHttpServerReplyHandler(OA2_REDIRECT_PORT);

    m_pAuthReplyHandler->setCallbackText(tr("You have successfully signed in to NextGIS "
                                            "Formbuilder. You may now return back to the app."));
    connect(m_pAuthReplyHandler, &QOAuthHttpServerReplyHandler::tokensReceived, this,
            &ApiWrapper::onTokensReceived);

    m_pAuthCodeFlow->setScope(OA2_USER_INFO_SCOPE);
    connect(m_pAuthCodeFlow, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
            &QDesktopServices::openUrl);
    m_pAuthCodeFlow->setAuthorizationUrl(OA2_AUTH_URL);
    m_pAuthCodeFlow->setClientIdentifier(OA2_CLIENT_ID);
    m_pAuthCodeFlow->setAccessTokenUrl(OA2_ACCESS_TOKEN_URL);
    m_pAuthCodeFlow->setClientIdentifierSharedKey("");
    m_pAuthCodeFlow->setReplyHandler(m_pAuthReplyHandler);
}

/**
 * @brief Destructor.
 */
ApiWrapper::~ApiWrapper ()
{
    delete m_pAuthReplyHandler;
    delete m_pAuthCodeFlow;
}


/**
 * @brief ...
 */
void ApiWrapper::setCallbackHtml (QString sFilePath)
{
    QFile file;

    file.setFileName(sFilePath);
    bool ok = file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!ok)
        return;

    QByteArray ba = file.readAll();
    file.close();

    // Note: here was a strange bug with the html string which is passed to the
    // QOAuthHttpServerReplyHandler. Though the string is read from the resource to the QString
    // absolutely correctly the html code which is shown in the browser after the successful
    // authentication is shown cutted (only when it contains some non-latin characters).
    // Is seems that this is fixed at least in Qt 5.9.1.

    m_pAuthReplyHandler->setCallbackText(QString::fromUtf8(ba));
}


/**
 * @brief Start OAuth2 authentication. After the end of the auth process this class will hold the
 * session an will be able to send authorized requests to this server. When the authentication
 * process ends the authFinished signal is emitted - this is for catching outside.
 */
void ApiWrapper::startAuthentication ()
{
    if (m_bIsAuthenticated)
        return;

    // First-time authorization with browser.
    if (m_sLastAccessToken == "")
    {
        connect(m_pAuthCodeFlow, &QOAuth2AuthorizationCodeFlow::granted,
        [=]()
        {
            this->onOAuth2Finished();
            disconnect(m_pAuthCodeFlow, &QOAuth2AuthorizationCodeFlow::granted, 0, 0);
        });

        m_pAuthCodeFlow->grant();
    }

    // Non-browser authorization.
    else
    {
        m_pAuthCodeFlow->setToken(m_sLastAccessToken);
        m_pAuthCodeFlow->setRefreshToken(m_sLastRefreshToken);

//        if (m_pAuthCodeFlow->expirationAt() >= )
//        {
            // This is the only(?) way how to find out that there was a succesful refresh operation.
            // But there is a bug in Qt 5.9.1: no emition of the according signal is done for the
            // refresh operation (only when grant() is called before this).
            connect(m_pAuthCodeFlow, &QOAuth2AuthorizationCodeFlow::statusChanged,
            [=]()
            {
                this->onOAuth2Finished();
                disconnect(m_pAuthCodeFlow, &QOAuth2AuthorizationCodeFlow::statusChanged, 0, 0);
            });

            // TODO: fix the following when it will be fixed in Qt (5.10 or earlier?).
            // See this file: https://github.com/qt/qtnetworkauth/blob/5.9/src/oauth/qoauth2authorizationcodeflow.cpp#L308
            QString s = OA2_CLIENT_ID;
            s.replace("\"","");
            m_pAuthCodeFlow->setModifyParametersFunction(
            [&](QAbstractOAuth::Stage stage, QVariantMap *parameters)
            {
                //parameters->insert("refresh_token", m_sLastRefreshToken);
                parameters->insert("client_id", s);
            });

            // Note: the QOAuth2AuthorizationCodeFlow::statusChanged will signalize about the
            // ending of the refresh process.
            m_pAuthCodeFlow->refreshAccessToken();
//        }
    }
}


/**
 * @brief Start GET request.
 */
void ApiWrapper::startGet (QString sUrl)
{
    m_sLastError.clear();
    // TODO: clear m_jReply JSON object here.

    m_pReply = m_pAuthCodeFlow->get(sUrl);
    connect(m_pReply, &QNetworkReply::finished, this, &ApiWrapper::onReplyFinished);
    connect(m_pReply, &QNetworkReply::readyRead, this, &ApiWrapper::onReplyReadyRead);
}


/**
 * @brief ...
 */
void ApiWrapper::startGetSupportInfo ()
{
    this->startGet(m_sSupportInfoUrl);
}

/**
 * @brief ...
 */
void ApiWrapper::startGetUserInfo ()
{
    this->startGet(m_sUserInfoUrl);
}


/**
 * @brief ...
 */
QString ApiWrapper::obtainString (QString sJsonKey) const
{
    if (m_jReply.isEmpty())
    {
        m_sLastError = tr("A void JSON reply");
        return "";
    }

    QJsonValue jStr = m_jReply.value(sJsonKey);
    if (!jStr.isString())
    {
        m_sLastError = tr("Cannot find a valid \"") + sJsonKey + tr("\" element in a JSON reply");
        return "";
    }

    return jStr.toString();
}


/**
 * @brief Obtain user account type from the JSON reply.
 */
AccountType ApiWrapper::obtainAccountType () const
{
    if (m_jReply.isEmpty())
    {
        m_sLastError = tr("A void JSON reply");
        return AccountType::Undefined;
    }

    QJsonValue jSupported = m_jReply.value("supported");
    if (!jSupported.isBool())
    {
        m_sLastError = tr("Cannot find \"supported\" boolean value in a JSON reply");
        return AccountType::Undefined;
    }

    return (jSupported.toBool()) ? AccountType::Supported : AccountType::NotSupported;
}


/**
 * @brief Obtain the start or end support dates from the JSON reply.
 */
QDate ApiWrapper::obtainDate (DateType eDateType) const
{
    QDate oDate;

    if (m_jReply.isEmpty())
    {
        m_sLastError = tr("A void JSON reply");
        return oDate;
    }

    QString sDateKey = "";
    if (eDateType == DateType::Start)
        sDateKey = "start_date";
    else if (eDateType == DateType::End)
        sDateKey = "end_date";

    QJsonValue jDate = m_jReply.value(sDateKey);
    if (!jDate.isString())
    {
        m_sLastError = tr("Cannot find a valid \"") + sDateKey + tr("\" element in a JSON reply");
        return oDate;
    }

    oDate = QDate::fromString(jDate.toString(), "yyyy-MM-dd");
    if (!oDate.isValid())
    {
        m_sLastError = tr("Unable to read the valid\"") + sDateKey + tr("\" date in a JSON reply");
        return oDate;
    }

    return oDate;
}


/**
 * @brief Obtain ...
 */
QString ApiWrapper::obtainGuid () const
{
    return this->obtainString("nextgis_guid");
}


/**
 * @brief Obtain ...
 */
QString ApiWrapper::obtainSign () const
{
    return this->obtainString("sign");
}


/**
 * @brief Obtain user name from the JSON reply.
 */
QString ApiWrapper::obtainLogin () const
{
    return this->obtainString("username");
}


/// ...
QString ApiWrapper::toReplyError (QString sMessage) const
{
    return QString(tr("[HTTP reply] ")) + sMessage;
}


// Connect/call this both for "grant" and "refresh" processes. See also onTokensReceived() where the
// new/refreshed tokens are assigned.
void ApiWrapper::onOAuth2Finished ()
{
    // TODO: how also to check unsuccessful oauth2 result? How to check errors?
    // See QAbstractOAuth::Error ?
    // See QAbstractOAuth2::error signal ?
    // See QOAuth2AuthorizationCodeFlowPrivate::_q_accessTokenRequestFinished()
    if (m_pAuthCodeFlow->status() == QAbstractOAuth::Status::Granted)
    {
        m_bIsAuthenticated = true;
    }
    else
    {
        m_sLastError = tr("OAuth2 process finished with a non-granted status");
        m_bIsAuthenticated = false;
    }

    emit authFinished();
}


// Update tokens: both access and refresh.
void ApiWrapper::onTokensReceived (const QVariantMap &mapTokens)
{
    QVariant vAccessToken = mapTokens["access_token"];
    QVariant vRefreshToken = mapTokens["refresh_token"];
    m_sLastAccessToken = vAccessToken.toString();
    m_sLastRefreshToken = vRefreshToken.toString();
}


// ...
void ApiWrapper::onReplyReadyRead ()
{
    QByteArray ba;
    ba = m_pReply->readAll();
    m_baReply += ba;
}

// ...
void ApiWrapper::onReplyFinished ()
{
    if (m_pReply->error() == QNetworkReply::NoError)
    {
        QJsonDocument jDoc = QJsonDocument::fromJson(m_baReply);
        if (!jDoc.isNull() && jDoc.isObject()) // && !jDoc.isEmpty())
            m_jReply = jDoc.object();
        else
            m_sLastError = tr("Incorrect reply from server: not a JSON received");
    }
    else
    {
        m_sLastError = this->toReplyError(QString::fromUtf8(m_pReply->errorString().toUtf8().data()));
    }

    m_baReply.clear();
    m_pReply->deleteLater();
    emit requestFinished();
}



