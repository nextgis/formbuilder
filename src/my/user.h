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
 */
class User: public QObject
{
    Q_OBJECT

    public:

        User ();
        virtual ~User ();

        void setApiWrapper (ApiWrapper *pApiWrapper);

        bool isAuthenticated () const;
        void startAuthentication ();

        QString getLastError () const { return m_pApiWrapper->obtainLastError(); }
        AccountType getAccountType () const { return m_eAccountType; }
        QString getLogin () const { return m_sName; }

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
};


}
}

#endif //USER_H



