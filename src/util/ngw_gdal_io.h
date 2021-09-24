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

#include "util/ngw_io.h"
#include "util/gdal_dataset_deleter.h"
#include "util/ngw_api.h"

#include "ogrsf_frmts.h"

namespace Fb
{
namespace Util
{


enum class NgwFormErr
{
    Ok,
    NoForm,
    Err
};


class NgwGdalIo: public NgwIo
{
    public:

     NgwGdalIo (const QString &app_version = "");
     virtual ~NgwGdalIo () override;

     virtual void reset () override;
     virtual QString getLastError () override { return error; }
     virtual QString getLastDeatiledError () override { return detailed_error; }

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
     virtual bool createFile (int &new_form_id, QString upload_reply, QString base_url,
                              int layer_id, QString res_type, QString disp_name) override;

     virtual QList<QPair<int, QString>> getCollectorUsers (QString base_url);
     virtual bool createCollectorUser (QString base_url, QString email);
     virtual int createResource (QString base_url, const QJsonObject &body);
     virtual int createSimpleBasemap (QString base_url, int group_id, QString basemap_name);
     virtual int createSimpleCollectorProject (QString base_url, int group_id, int collector_user_id,
                                               QString user, QString password,
                                               QString proj_name, QString proj_description,
                                                int layer_id, QString layer_name,
                                                int basemap_id, QString basemap_name);
     virtual int createSimpleWebmap (QString base_url, int group_id, QString webmap_name, int style_id, QString layer_name);

     virtual NgwFormErr downloadForm (QString base_url, int layer_id, QString file_path);

     QString getUrlResourcePage (QString base_url, int resource_id);
     QString getUrlVersion (QString base_url);

    protected:

     bool u_getChildResources (QList<NgwResourceData> &resources,
                               QString base_url, int resource_id,
                               const std::set<NgwResourceType> &allowed_types);

     void u_openDataset (GDALDatasetPtr &dataset_p, QString base_url, int resource_id, bool read_only);

     NgwApi *api;

     QString error;
     QString detailed_error;

     QString app_version;
};


}
}



