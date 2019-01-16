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

#include "ngw_default_io.h"

#include <QFile>

using namespace Fb;
using namespace Util;


/*!
 * @brief Constructor. Will take ownership of the passed NgwApi object.
 */
NgwDefaultIo::NgwDefaultIo (NgwApi *api):
    NgwIo(),
    api(api),
    ww(new WebWorker())
{
    if (api == nullptr)
        throw; // TODO: named exception
}

NgwDefaultIo::~NgwDefaultIo ()
{
    delete ww;
    delete api;
}


void NgwDefaultIo::reset ()
{
    // Recreate web worker from scratch in order to clear cookies / old replies / old errors / etc.
    delete ww;
    ww = new WebWorker();
}


bool NgwDefaultIo::getRootResources (QList<NgwResourceData> &resources,
                                     QString base_url, QString login, QString password,
                                     const std::set<NgwResourceType> &allowed_types)
{
    ww->post(api->urlAuth(base_url), api->bodyAuth(login, password), WebReqType::Authorize);
    if (ww->wasError())
    {
        error = ww->obtainError();
        return false;
    }

    ww->get(api->urlRootResources(base_url));
    if (ww->wasError())
    {
        error = ww->obtainError();
        return false;
    }

    QJsonDocument j_reply = ww->obtainJson();
    return this->parseGetResourcesReply(j_reply, resources, allowed_types);
}

bool NgwDefaultIo::getResources (QList<NgwResourceData> &resources,
                                 QString base_url, int resource_id,
                                 const std::set<NgwResourceType> &allowed_types)
{
    ww->get(api->urlResources(base_url, resource_id));
    if (ww->wasError())
    {
        error = ww->obtainError();
        return false;
    }

    QJsonDocument j_reply = ww->obtainJson();
    return this->parseGetResourcesReply(j_reply, resources, allowed_types);
}

bool NgwDefaultIo::getLayerInfo (NgwLayerInfo &layer_info, QString base_url, int resource_id)
{
    ww->get(api->urlResource(base_url, resource_id));
    if (ww->wasError())
    {
        error = ww->obtainError();
        return false;
    }

    QJsonDocument j_reply = ww->obtainJson();
    return this->parseGetVecLayerReply(j_reply, layer_info);
}

bool NgwDefaultIo::createLayer (int &new_layer_id, const NgwLayerInfo &layer_info, QString base_url,
                                int resource_group_id, QString base_url_copy_features,
                                int layer_id_copy_features)
{
    ww->post(api->urlCreateResource(base_url), api->bodyCreateLayer(layer_info, resource_group_id));
    if (ww->wasError())
    {
        error = ww->obtainError();
        return false;
    }

    QJsonDocument j_reply = ww->obtainJson();
    return this->parseCreateResourceReply(j_reply, new_layer_id);
}

bool NgwDefaultIo::createForm (int &new_form_id, QString ngfp_path, QString base_url,
                               int layer_id)
{
    QFile *file = new QFile(ngfp_path);
    if (!file->open(QIODevice::ReadOnly))
    {
        error = QObject::tr("Failed to open temporary form file");
        return false;
    }
    QByteArray file_contents = file->readAll(); // load ALL file data into memory
    file->close();

    ww->multipartPost(api->urlUploadFile(base_url), api->bodyUploadNgfp(file_contents));
    if (ww->wasError())
    {
        error = ww->obtainError();
        return false;
    }

    NgwFileInfo file_info;
    QJsonDocument j_reply = ww->obtainJson();
    bool ok = this->parseUploadNgfpReply(j_reply, file_info);
    if (!ok)
        return false;

    ww->post(api->urlCreateResource(base_url), api->bodyCreateForm(file_info, layer_id));
    if (ww->wasError())
    {
        error = ww->obtainError();
        return false;
    }

    j_reply = ww->obtainJson();
    return this->parseCreateResourceReply(j_reply, new_form_id);
}


bool NgwDefaultIo::parseGetResourcesReply (QJsonDocument &j_reply, QList<NgwResourceData> &resources,
                                        const std::set<NgwResourceType> &allowed_types)
{
    resources.clear();

    if (!j_reply.isArray())
    {
        error = err("not a JSON array");
        return false;
    }

    // Look for all resources and skip if cannot read (e.g. resource is forbidden when "resource"
    // key is JSON-null).
    for (int i = 0; i < j_reply.array().size(); i++)
    {
        QJsonValue j_arr_item = j_reply.array()[i];
        if (!j_arr_item.isObject())
        {
            error = err("JSON array item is not an object");
            continue;
        }

        QJsonValue j_res = j_arr_item.toObject()["resource"];
        if (!j_res.isObject())
        {
            error = err("no \"resource\" key in JSON array item");
            continue;
        }

        QJsonObject j_res_obj = j_res.toObject();

        QJsonValue j_cls = j_res_obj["cls"];
        if (!j_cls.isString())
        {
            error = err("no \"cls\" key in JSON object");
            continue;
        }

        NgwResourceType res_type = g_findNgwResType(j_cls.toString());
        if (allowed_types.find(res_type) == allowed_types.end())
            continue;

        QJsonValue j_id = j_res_obj["id"];
        if (!j_id.isDouble())
        {
            error = err("no \"id\" key in JSON object");
            continue;
        }

        QJsonValue j_dname = j_res_obj["display_name"];
        if (!j_dname.isString())
        {
            error = err("no \"display_name\" key in JSON object");
            continue;
        }

        QJsonValue j_children = j_res_obj["children"];
        if (!j_children.isBool())
        {
            error = err("no \"children\" key in JSON object");
            continue;
        }

        resources.append({j_id.toInt(), res_type, j_dname.toString(), j_children.toBool()});
    }

    return true;
}

bool NgwDefaultIo::parseGetVecLayerReply (QJsonDocument &j_reply, NgwLayerInfo &layer_info)
{
    layer_info.fields.clear();

    if (!j_reply.isObject())
    {
        error = err("not a JSON object");
        return false;
    }

    QJsonObject j_reply_obj = j_reply.object();

    QJsonValue j_resource = j_reply_obj["resource"];
    if (!j_resource.isObject())
    {
        error = err("no \"j_resource\" key in JSON object");
        return false;
    }

    QJsonValue j_disp_name = j_resource.toObject()["display_name"];
    if (!j_disp_name.isString())
    {
        error = err("no \"display_name\" key in JSON object");
        return false;
    }

    QJsonValue j_vec_layer = j_reply_obj["vector_layer"];
    if (!j_vec_layer.isObject())
    {
        error = err("no \"vector_layer\" key in JSON object");
        return false;
    }

    QJsonValue j_geom_type = j_vec_layer.toObject()["geometry_type"];
    if (!j_geom_type.isString())
    {
        error = err("no \"geometry_type\" key in JSON object");
        return false;
    }

    QJsonValue j_f_layer = j_reply_obj["feature_layer"];
    if (!j_f_layer.isObject())
    {
        error = err("no \"feature_layer\" key in JSON object");
        return false;
    }

    QJsonValue j_fields = j_f_layer.toObject()["fields"];
    if (!j_fields.isArray())
    {
        error = err("value by \"fields\" key is not a JSON array");
        return false;
    }

    QJsonArray j_array = j_fields.toArray();
    for (int i = 0; i < j_array.size(); i++)
    {
        QJsonValue j_arr_item = j_array[i];
        if (!j_arr_item.isObject())
        {
            error = err("JSON array item in \"fields\" is not an object");
            return false;
        }

        QJsonValue j_keyname = j_arr_item.toObject()["keyname"];
        if (!j_keyname.isString())
        {
            error = err("no \"keyname\" key in JSON object");
            return false;
        }

        QJsonValue j_dispname = j_arr_item.toObject()["display_name"];
        if (!j_dispname.isString())
        {
            j_dispname = j_keyname.toString();
        }

        QJsonValue j_datatype = j_arr_item.toObject()["datatype"];
        if (!j_datatype.isString())
        {
            error = err("no \"datatype\" key in JSON object");
            return false;
        }

        layer_info.fields.append({j_keyname.toString(),j_dispname.toString(),j_datatype.toString()});
    }

    layer_info.name = j_disp_name.toString();
    layer_info.geom_type = j_geom_type.toString();

    return true;
}

bool NgwDefaultIo::parseCreateResourceReply (QJsonDocument &j_reply, int &new_resource_id)
{
    if (!j_reply.isObject())
    {
        error = err("not a JSON object");
        return false;
    }

    QJsonValue j_id = j_reply.object()["id"];
    if (!j_id.isDouble())
    {
        error = err("no \"id\" key in JSON object");
        return false;
    }

    new_resource_id = j_id.toInt();

    return true;
}

bool NgwDefaultIo::parseUploadNgfpReply (QJsonDocument &j_reply, NgwFileInfo &file_info)
{
    if (!j_reply.isObject())
    {
        error = err("not a JSON object");
        return false;
    }

    QJsonValue j_up_meta = j_reply.object()["upload_meta"];
    if (!j_up_meta.isArray() || j_up_meta.toArray().size() == 0)
    {
        error = err("\"upload_meta\" not a JSON array or a void array");
        return false;
    }

    QJsonValue j_arr_item = j_up_meta.toArray()[0];
    if (!j_arr_item.isObject())
    {
        error = err("\"upload_meta\" array item is not a JSON object");
        return false;
    }

    QJsonValue j_id = j_arr_item.toObject()["id"];
    if (!j_id.isString())
    {
        error = err("no \"id\" key in JSON object");
        return false;
    }

    file_info.id = j_id.toString();
    file_info.mime_type = j_arr_item.toObject()["mime_type"].toString();
    file_info.name = j_arr_item.toObject()["name"].toString();
    file_info.size = j_arr_item.toObject()["size"].toInt();

    return true;
}



