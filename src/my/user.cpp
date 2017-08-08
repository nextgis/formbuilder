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
 
#include "user.h"

using namespace Nextgis::My;


/**
 * @brief Constructor.
 */
User::User ():
    m_eAccountType(AccountType::Undefined),
    m_sName("")
{
    m_pApiWrapper = new ApiWrapper();
    connect(m_pApiWrapper, &ApiWrapper::authFinished, this, &User::u_onAuthFinished);
}

/**
 * @brief Destructor.
 */
User::~User ()
{
    delete m_pApiWrapper;
}


/**
 * @brief Reset the ApiWrapper with the passed one. Note: the class takes ownership of the passed
 * object.
 * @details With this method you can change the support of other API versions for this class.
 */
void User::setApiWrapper (ApiWrapper *pApiWrapper)
{
    if (pApiWrapper == nullptr)
        return;
    delete m_pApiWrapper;
    m_pApiWrapper = pApiWrapper;
}


/**
 * @brief ...
 */
void User::setAuthCallbackHtml (QString sFilePath)
{
    m_pApiWrapper->setCallbackHtml(sFilePath);
}


/**
 * @brief Return true if this user is correctly authenticated.
 */
bool User::isAuthenticated () const
{
    if (m_eAccountType == AccountType::Undefined || m_sName == "")
        return false;
    return true;
}


/**
 * @brief Start authentication process for this user. The authentication process itself consists of
 * some steps except the standard OAuth2 steps during which some additional http requests are sent
 * to the server. After the end of the successful authentication the User class instance will
 * contain the appropriate information of the user which is signed in for my.nextgis.com.
 * @see User::authenticationFinished
 */
void User::startAuthentication ()
{
    // If all fields for the correctly authenticated user are set we will not start another
    // authentication.
    if (this->isAuthenticated())
        return;

    disconnect(m_pApiWrapper, &ApiWrapper::requestFinished, 0, 0);
    m_pApiWrapper->startAuthentication();
}

// Authentication step 1 of 3: ...
void User::u_onAuthFinished ()
{
    if (!m_pApiWrapper->isAuthenticated())
    {

        emit authenticationFinished();
        return;
    }

    connect(m_pApiWrapper, &ApiWrapper::requestFinished, this, &User::u_onGetAccountTypeFinished);
    m_pApiWrapper->startGetSupportInfo();
}

// Authentication step 2 of 3: ...
void User::u_onGetAccountTypeFinished ()
{
    if (!m_pApiWrapper->obtainLastError().isEmpty())
    {

        emit authenticationFinished();
        return;
    }

    m_eAccountType = m_pApiWrapper->obtainAccountType();

    if (m_eAccountType == AccountType::Supported)
    {
        m_oStartDate = m_pApiWrapper->obtainDate(DateType::Start);
        m_oEndDate = m_pApiWrapper->obtainDate(DateType::End);
    }

    disconnect(m_pApiWrapper, &ApiWrapper::requestFinished, 0, 0);
    connect(m_pApiWrapper, &ApiWrapper::requestFinished, this, &User::u_onGetNameFinished);
    m_pApiWrapper->startGetUserInfo();
}

// Authentication step 3 of 3: ...
void User::u_onGetNameFinished ()
{
    if (!m_pApiWrapper->obtainLastError().isEmpty())
    {

        emit authenticationFinished();
        return;
    }

    m_sName = m_pApiWrapper->obtainLogin();

    emit authenticationFinished();
}



