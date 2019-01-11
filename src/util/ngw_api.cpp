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

#include "ngw_api.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonValueRef>

using namespace Fb;
using namespace Util;


QString NgwApi::urlAuth (QString base_url)
{
    return QString("%1/login").arg(base_url);
}

QString NgwApi::urlRootResources (QString base_url)
{
    return this->urlResources(base_url, 0);
}

QString NgwApi::urlResources (QString base_url, int resource_id)
{
    //return QString("%1/resource/%2/child/").arg(base_url).arg(resource_id);
    return QString("%1/api/resource/?parent=%2").arg(base_url).arg(resource_id);
}

QString NgwApi::urlResource (QString base_url, int resource_id)
{
    return QString("%1/api/resource/%2").arg(base_url).arg(resource_id);
}

QString NgwApi::urlResourcePage (QString base_url, int resource_id)
{
    return QString("%1/resource/%2").arg(base_url).arg(resource_id);
}

QString NgwApi::urlCreateResource (QString base_url)
{
    return QString("%1/api/resource/").arg(base_url);
}

QString NgwApi::urlUploadFile (QString base_url)
{
    return QString("%1/api/component/file_upload/upload").arg(base_url);
}

QString NgwApi::urlDownloadNgfp (QString base_url, int resource_id)
{
    return QString("%1/api/resource/%2/ngfp").arg(base_url).arg(resource_id);
}


QByteArray NgwApi::bodyAuth (QString login, QString password)
{
    return QString("login=%1&password=%2").arg(login).arg(password).toUtf8();
}

QByteArray NgwApi::bodyCreateLayer (const NgwLayerInfo &layer_info, int resource_group_id)
{
    QJsonObject j_resource;
    j_resource["cls"] = "vector_layer";
    j_resource["parent"] = QJsonObject({{"id", resource_group_id}});
    j_resource["display_name"] = layer_info.name;
    j_resource["keyname"] = QJsonValue();
    j_resource["description"] = QJsonValue();

    QJsonObject j_resmeta;
    j_resmeta["items"] = QJsonObject();

    QJsonObject j_vector_layer;
    j_vector_layer["srs"] = QJsonObject({{"id", 3857}});
    j_vector_layer["geometry_type"] = layer_info.geom_type;
    QJsonArray j_fields;
    for (auto &tup: layer_info.fields)
        j_fields.append(QJsonObject({{"keyname", std::get<0>(tup)},
                                     {"display_name", std::get<1>(tup)},
                                     {"datatype", std::get<2>(tup)}}));
    j_vector_layer["fields"] = j_fields;

    QJsonObject j_obj;
    j_obj["resource"] = j_resource;
    j_obj["resmeta"] = j_resmeta;
    j_obj["vector_layer"] = j_vector_layer;

    QJsonDocument j_doc(j_obj);
    return j_doc.toJson();
}

QByteArray NgwApi::bodyCreateForm (const NgwFileInfo &file_info, int layer_id)
{
    QJsonObject j_resource;
    j_resource["cls"] = "formbuilder_form";
    j_resource["parent"] = QJsonObject({{"id", layer_id}});
    j_resource["display_name"] = "Form";
    j_resource["keyname"] = QJsonValue();
    j_resource["description"] = QJsonValue();

    QJsonObject j_resmeta;
    j_resmeta["items"] = QJsonObject();

    QJsonObject j_file_upload;
    j_file_upload["id"] = file_info.id;
    j_file_upload["mime_type"] = file_info.mime_type;
    j_file_upload["name"] = file_info.name;
    j_file_upload["size"] = file_info.size;
    QJsonObject j_fb_form;
    j_fb_form["file_upload"] = j_file_upload;

    QJsonObject j_obj;
    j_obj["resource"] = j_resource;
    j_obj["resmeta"] = j_resmeta;
    j_obj["formbuilder_form"] = j_fb_form;

    QJsonDocument j_doc(j_obj);
    return j_doc.toJson();
}

QHttpMultiPart *NgwApi::bodyUploadNgfp (const QByteArray &file_contents)
{
    QHttpMultiPart *multipart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart part;
    part.setHeader(QNetworkRequest::ContentDispositionHeader,
                   QVariant("form-data; name=\"file\"; filename=\"form.ngfp\""));
    part.setHeader(QNetworkRequest::ContentTypeHeader,
                   QVariant("application/octet-stream"));
    part.setBody(file_contents); // TODO: setBodyDevice() for performance.
    multipart->append(part);

    return multipart;
}



