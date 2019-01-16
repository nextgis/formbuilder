/**************************************************************************************************
 *                                                                                                *
 *    Project:  NextGIS Formbuilder                                                               *
 *    Authors:  Mikhail Gusev, gusevmihs@gmail.com                                                *
 *              Copyright (C) 2014-2019 NextGIS                                                   *
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

#include "ngw_io.h"
#include "ngw_api.h"
#include "web_worker.h"

namespace Fb
{
namespace Util
{


/*!
 * @brief This is default implementation of NgwIo which internally uses QtNetwork classes for actual
 * accessing NextGIS Web. The constructor requires NgwApi object of some API version. This object
 * will be deleted in destructor.
 */
class NgwDefaultIo: public NgwIo
{
    public:

     explicit NgwDefaultIo (NgwApi *api);
     virtual ~NgwDefaultIo ();

     inline NgwApi *getNgwApi () const { return api; }
     inline WebWorker *getWebWorker () const { return ww; }

     virtual void reset ();
     virtual QString getLastError () override { return error; }

     virtual bool getRootResources (QList<NgwResourceData> &resources,
                                    QString base_url, QString login, QString password,
                                    const std::set<NgwResourceType> &allowed_types) override;
     virtual bool getResources (QList<NgwResourceData> &resources,
                                QString base_url, int resource_id,
                                const std::set<NgwResourceType> &allowed_types) override;
     virtual bool getLayerInfo (NgwLayerInfo &layer_info, QString base_url,
                                int resource_id) override;
     virtual bool createLayer (int &new_layer_id, const NgwLayerInfo &layer_info, QString base_url,
                               int resource_group_id = 0, QString base_url_copy_features = "",
                               int layer_id_copy_features = -1) override;
     virtual bool createForm (int &new_form_id, QString ngfp_path, QString base_url,
                              int layer_id) override;

    protected:

     bool parseGetResourcesReply (QJsonDocument &j_reply, QList<NgwResourceData> &resources,
                               const std::set<NgwResourceType> &allowed_types);
     bool parseGetVecLayerReply (QJsonDocument &j_reply, NgwLayerInfo &layer_info);
     bool parseCreateResourceReply (QJsonDocument &j_reply, int &new_resource_id);
     bool parseUploadNgfpReply (QJsonDocument &j_reply, NgwFileInfo &file_info);

     inline QString err (const QString &text) {return QString("Reply: %1").arg(text);}

     QString error;
     NgwApi *api;
     WebWorker *ww;
};


}
}



