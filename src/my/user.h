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

#ifndef USER_H
#define USER_H

#include "apiwrapper.h"

#include <QObject>
#include <QString>
#include <QtNetworkAuth>

using namespace Nextgis::My;

namespace Nextgis
{

#ifdef _RSA_PUBLIC_KEY
const QString RSA_PUBLIC_KEY = {_RSA_PUBLIC_KEY}; // should be defined somewhere outside
#else
const QString RSA_PUBLIC_KEY {""};
#endif


/**
 * @brief The class which represents a NextGIS user in the application. The class internally
 * uses Nextgis::My::ApiWrapper for requests to the my.nextgis.com server and uses the common
 * NextGIS ini file if there is no internet connection. By default the User is not authenticated
 * and you should call startAuthentication() in order to fill User's fields with neccessery info.
 */
class User: public QObject
{
    Q_OBJECT

    public:

        User ();
        virtual ~User ();

        void setApiWrapper (ApiWrapper *pApiWrapper);
        ApiWrapper *getApiWrapperPtr () const { return m_pApiWrapper; }

        void writeToCache ();
        void readFromCache ();
        void clearCache ();

        bool verifyInfo ();
        bool verifySignature ();

        bool isAuthenticated () const;
        void startAuthentication ();

        AccountType getAccountType () const { return m_eAccountType; }
        QString getLogin () const { return m_sName; }
        QDate getSupportStartDate () const { return m_oStartDate; }
        QDate getSupportEndDate () const { return m_oEndDate; }

    signals:

        void authenticationFinished ();

    private slots:

        void u_onAuthFinished ();
        void u_onGetAccountTypeFinished ();
        void u_onGetNameFinished ();

    private:

        static QString s_toString (const QDate &oDate);
        static QString s_toString (Nextgis::My::AccountType eAccountType);
        static QDate s_toDate (const QString &sString);
        static Nextgis::My::AccountType s_toAccountType (const QString &sString);

        ApiWrapper *m_pApiWrapper;

        QString m_sName;
        Nextgis::My::AccountType m_eAccountType;
        QDate m_oStartDate;
        QDate m_oEndDate;
        QString m_sGuid;
        QString m_sSign;
};


}

#endif //USER_H



