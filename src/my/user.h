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

namespace Nextgis
{
namespace My
{


/**
 * @brief The class which represents a my.nextgis.com user in the application. The class internally
 * uses Nextgis::My::ApiWrapper for requests to the my.nextgis.com server.
 * @details Usage: create an instance of this class and call startAuthentication(). After the end of
 * the authentication process (authenticationFinished() signal emitted) the instance will contain
 * all nesessary information about the user.
 */
class User: public QObject
{
    Q_OBJECT

    public:

        User ();
        virtual ~User ();

        void setApiWrapper (ApiWrapper *pApiWrapper);
        ApiWrapper *getApiWrapperPtr () const { return m_pApiWrapper; }

        void setAuthCallbackHtml (QString sFilePath);

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

        ApiWrapper *m_pApiWrapper;

        Nextgis::My::AccountType m_eAccountType;
        QString m_sName;
        QDate m_oStartDate;
        QDate m_oEndDate;
        QString m_sGuid;
        QString m_sSign;
};


}
}

#endif //USER_H



