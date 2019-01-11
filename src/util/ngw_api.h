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

#include "util/ngw_types.h"

#include <QString>
#include <QList>
#include <QHttpMultiPart>

namespace Fb
{
namespace Util
{


/*!
 * @brief NgwApi contains NextGIS Web API methods. Reimplement them in derived classes to support
 * other versions of API.
 */
class NgwApi
{
    public:

     NgwApi () = default;
     virtual ~NgwApi () = default;

     virtual QString urlAuth (QString base_url);
     virtual QString urlRootResources (QString base_url);
     virtual QString urlResources (QString base_url, int resource_id);
     virtual QString urlResource (QString base_url, int resource_id);
     virtual QString urlResourcePage (QString base_url, int resource_id);
     virtual QString urlCreateResource (QString base_url);
     virtual QString urlUploadFile (QString base_url);
     virtual QString urlDownloadNgfp (QString base_url, int resource_id);

     virtual QByteArray bodyAuth (QString login, QString password);
     virtual QByteArray bodyCreateLayer (const NgwLayerInfo &layer_info, int resource_group_id);
     virtual QByteArray bodyCreateForm (const NgwFileInfo &file_info, int layer_id);
     virtual QHttpMultiPart *bodyUploadNgfp (const QByteArray &file_contents);
};


}
}



