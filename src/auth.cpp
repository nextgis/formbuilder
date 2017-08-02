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
 
#include "auth.h"

using namespace Nextgis;


User::User ()
{
    m_bIsAuthorized = false;
    m_eAccountType = AccountType::Undefined;
    m_eCurrentRequest = Request::Undefined;

    m_pAuth = new QOAuth2AuthorizationCodeFlow();
    m_pAuth->setScope(OAUTH2_USER_INFO_SCOPE);
    connect(m_pAuth,&QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,&QDesktopServices::openUrl);
    m_pAuth->setAuthorizationUrl(OAUTH2_AUTH_URL);
    m_pAuth->setClientIdentifier(OAUTH2_CLIENT_ID);
    m_pAuth->setAccessTokenUrl(OAUTH2_ACCESSTOKEN_URL);
    m_pAuth->setClientIdentifierSharedKey("");
    m_pAuth->setReplyHandler(new QOAuthHttpServerReplyHandler(OAUTH2_REDIRECT_PORT));
    connect(m_pAuth, &QOAuth2AuthorizationCodeFlow::granted, this, &User::onOAuth2Granted);
}

User::~User ()
{
}


/**
 * @brief Start OAuth2 authentication.
 * @see User::authFinished
 */
void User::startAuthentication ()
{
    if (m_bIsAuthorized)
        return;
    m_pAuth->grant();
}


// Slot. After the end of the authorization process.
void User::onOAuth2Granted ()
{  
    // TODO: how to check errors?

    m_bIsAuthorized = true;
    this->startGetSupportInfo();
}


// Slot. Common actions for all replies: read reply data.
void User::onReplyReadyRead ()
{
    QByteArray ba;
    ba = m_pReply->readAll();
    m_baReply += ba;
}


// Slot. Common actions for all replies: on reply finished.
void User::onReplyFinished ()
{
    if (m_pReply->error() == QNetworkReply::NoError)
    {
        // GET support info request.
        if (m_eCurrentRequest == Request::GetSuppportInfo)
        {
            // If we have a {} JSON for the user it says that he uses the "Free" plan.
            QJsonDocument jDoc = QJsonDocument::fromJson(m_baReply);
            if (!jDoc.isNull() && jDoc.isObject()) // && !jDoc.isEmpty())
            {
                QJsonObject jReply = jDoc.object();
                if (jReply.isEmpty())
                    m_eAccountType = AccountType::NotSupported;
                else
                    m_eAccountType = AccountType::Supported;
            }
        }

        // TODO: process other requests.
        // ...
    }
    else
    {
        m_sLastReplyError = QString::fromUtf8(m_pReply->errorString().toUtf8().data());
    }

    m_eCurrentRequest = Request::Undefined;
    m_pReply->deleteLater();
    emit authFinished();
}


// Send the GET request to obtain support information about this user.
void User::startGetSupportInfo ()
{
    if (!m_bIsAuthorized)
        return;

    m_eCurrentRequest = Request::GetSuppportInfo;
    m_sLastReplyError.clear();

    m_pReply = m_pAuth->get(SUPPORT_INFO_URL);
    connect(m_pReply, &QNetworkReply::finished, this, &User::onReplyFinished);
    connect(m_pReply, &QNetworkReply::readyRead, this, &User::onReplyReadyRead);
}



