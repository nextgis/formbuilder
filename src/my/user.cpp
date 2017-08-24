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

#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/x509.h>
//#include <openssl/evp_pkey.h>

using namespace Nextgis;
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
 * @brief Write user settings to the file.
 */
void User::writeToCache ()
{
    QSettings s(QSettings::IniFormat, QSettings::UserScope, "NextGIS", "User");
    s.setValue("user_id", m_sGuid);
    s.setValue("sign", m_sSign);
    s.setValue("start_date", s_toString(m_oStartDate));
    s.setValue("end_date", s_toString(m_oEndDate));
    s.setValue("supported", s_toString(m_eAccountType));
    s.setValue("user_name", m_sName);
}

/**
 * @brief Read file with user last saved settings. Can be read invalid. Check this outside.
 * @see User::hasValidInfo
 */
void User::readFromCache ()
{
    QSettings s(QSettings::IniFormat, QSettings::UserScope, "NextGIS", "User");
    m_sGuid = s.value("user_id", "").toString();
    m_sSign = s.value("sign", "").toString();
    m_oStartDate = s_toDate(s.value("start_date", "").toString());
    m_oEndDate = s_toDate(s.value("end_date", "").toString());
    m_eAccountType = s_toAccountType(s.value("supported", "").toString());
    m_sName = s.value("user_name", "").toString();
}

/**
 * @brief Clears the file with user settings. Can be called when user is logged out.
 */
void User::clearCache ()
{
    QSettings s(QSettings::IniFormat, QSettings::UserScope, "NextGIS", "User");
    s.setValue("user_id", "");
    s.setValue("sign", "");
    s.setValue("start_date", "");
    s.setValue("end_date", "");
    s.setValue("supported", "");
    s.setValue("user_name", "");
}


/**
 * @brief Returns true if this User has a correct set of info fields.
 */
bool User::verifyInfo ()
{
    if (m_eAccountType == AccountType::Undefined || m_sGuid == "")
        return false; // the ini file at least must have these fields

    if (m_eAccountType == AccountType::Supported &&
        (!m_oStartDate.isValid() || !m_oEndDate.isValid()))
        return false; // if the user is "supported" it must have the dates correctly set

    return true;
}


/**
 * @brief Verify User info with RSA signature.
 */
bool User::verifySignature ()
{
    QString sMessage = m_sGuid + s_toString(m_oStartDate) + s_toString(m_oEndDate)
            + s_toString(m_eAccountType);

    QByteArray baMessage = sMessage.toUtf8();
    QByteArray baSignature = QByteArray::fromBase64(m_sSign.toUtf8());
    QByteArray baRsaPublicKey = RSA_PUBLIC_KEY.toUtf8();

    auto warn = []()
    {
        unsigned long err;
        do
        {
            err = ERR_get_error();
            char *str = (char*)malloc(1024);
            ERR_error_string(err, str);
            qWarning()<<QString(str);
        }
        while(err);
    };

    BIO* b = BIO_new(BIO_s_mem());
    if (b == NULL) { warn(); return false; }

    BIO_write(b, baRsaPublicKey.data(), baRsaPublicKey.size());

    EVP_PKEY* evpkey = PEM_read_bio_PUBKEY(b, NULL, NULL, NULL);
    if (evpkey == NULL) { warn(); BIO_free(b); return false; }

    const EVP_MD* dgst = EVP_get_digestbyname("sha256");
    if (dgst == NULL) { warn(); EVP_PKEY_free(evpkey); BIO_free(b); return false; }

    EVP_MD_CTX* ctx = NULL;
    ctx = EVP_MD_CTX_create();
    if (ctx == NULL) { warn(); EVP_PKEY_free(evpkey); BIO_free(b); return false; }

    EVP_VerifyInit(ctx, dgst);
    EVP_VerifyUpdate(ctx, (unsigned char*)(baMessage.data()), baMessage.size());

    int ret = EVP_VerifyFinal(ctx, (unsigned char*)(baSignature.data()), baSignature.size(), evpkey);

    EVP_MD_CTX_destroy(ctx);
    EVP_PKEY_free(evpkey);
    BIO_free(b);

    return ret;
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
    m_oStartDate = m_pApiWrapper->obtainDate(DateType::Start); // can be obtained invalid
    m_oEndDate = m_pApiWrapper->obtainDate(DateType::End);
    m_sGuid = m_pApiWrapper->obtainGuid();
    m_sSign = m_pApiWrapper->obtainSign();

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

    disconnect(m_pApiWrapper, &ApiWrapper::requestFinished, 0, 0);

    emit authenticationFinished();
}


QString User::s_toString (const QDate &oDate)
{
    return oDate.toString("yyyy-MM-dd");
}

QString User::s_toString (Nextgis::My::AccountType eAccountType)
{
    if (eAccountType == AccountType::Supported)
        return "true";
    if (eAccountType == AccountType::NotSupported)
        return "false";
    return "";
}

QDate User::s_toDate (const QString &sString)
{
    return QDate::fromString(sString, "yyyy-MM-dd"); // can be returned invalid
}

AccountType User::s_toAccountType (const QString &sString)
{
    if (sString == "true")
        return AccountType::Supported;
    if (sString == "false")
        return AccountType::NotSupported;
    return AccountType::Undefined;
}



