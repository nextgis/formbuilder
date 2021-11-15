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

#include "ngw_gdal_io.h"

#include "core/geom_types.h"

#include "cpl_http.h"
#include "cpl_vsi.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>

using namespace Fb;
using namespace Util;


NgwGdalIo::NgwGdalIo (const QString &app_version):
    NgwIo()
{
    this->api = new NgwApi();
    this->app_version = app_version;
}

NgwGdalIo::~NgwGdalIo ()
{
    delete api;
}


void NgwGdalIo::reset ()
{
    CPLSetConfigOption("GDAL_HTTP_USERPWD", NULL);

    CPLSetConfigOption("GDAL_HTTP_USERAGENT", QString("formbuilder/%1").arg(app_version).toUtf8().data());

#ifdef Q_OS_WIN
    QString s_path = QCoreApplication::applicationDirPath() + QDir::separator() + QLatin1String("..\\share\\ssl\\certs");
    QDir path(s_path);
    CPLSetConfigOption("CURL_CA_BUNDLE", path.absoluteFilePath("cert.pem").toUtf8().data());
#endif
}


bool NgwGdalIo::getRootResources (QList<NgwResourceData> &resources,
                                  QString base_url, QString login, QString password,
                                  const std::set<NgwResourceType> &allowed_types)
{
    if (login != "" && password != "")
    {
        QString s_log_pass = login + ":" + password;
        QByteArray ba_log_pass(s_log_pass.toUtf8());
        CPLSetConfigOption("GDAL_HTTP_USERPWD", ba_log_pass.data());
    }

    return u_getChildResources(resources, base_url, 0, allowed_types);
}

bool NgwGdalIo::getResources (QList<NgwResourceData> &resources,
                              QString base_url, int resource_id,
                              const std::set<NgwResourceType> &allowed_types)
{
    return u_getChildResources(resources, base_url, resource_id, allowed_types);
}

bool NgwGdalIo::getLayerInfo (NgwLayerInfo &layer_info, QString base_url, int resource_id)
{
    GDALDatasetPtr dataset_p;
    u_openDataset(dataset_p, base_url, resource_id, true);
    if (dataset_p.data() == NULL)
    {
        error = QObject::tr("Unable to open NGW dataset via GDAL");
        return false;
    }

    OGRLayer *layer = dataset_p.data()->GetLayer(0);
    if (layer == NULL)
    {
        error = QObject::tr("Unable to open [0] layer of NGW dataset via GDAL");
        return false;
    }

    OGRFeatureDefn *layer_defn = layer->GetLayerDefn();

    layer_info.name = layer->GetName();
    layer_info.geom_type = Core::g_findGeomTypeNgw(layer->GetGeomType());
    for (int i = 0; i < layer_defn->GetFieldCount(); i++)
    {
        QString s_field_alias = QString("FIELD_%1_ALIAS").arg(i);
        QString alias = layer->GetMetadataItem(s_field_alias.toUtf8().data());

        OGRFieldDefn *field_defn = layer_defn->GetFieldDefn(i);
        layer_info.fields.append({field_defn->GetNameRef(), alias,
                                  Core::g_findFieldTypeNgw(field_defn->GetType())});
    }

    return true;
}

bool NgwGdalIo::createLayer (int &new_layer_id, const NgwLayerInfo &layer_info, QString base_url,
                             int resource_group_id, QString base_url_copy_features, int layer_id_copy_features)
{
    GDALDatasetPtr dataset_p;
    u_openDataset(dataset_p, base_url, resource_group_id, false);
    if (dataset_p.data() == NULL)
    {
        error = QObject::tr("Unable to open NGW dataset via GDAL");
        return false;
    }

    QByteArray ba_name(layer_info.name.toUtf8());

    OGRwkbGeometryType geom_type = Core::g_findGeomTypeGdal(layer_info.geom_type);

    OGRSpatialReference *srs = new OGRSpatialReference();
    srs->importFromEPSG(3857);
    #if GDAL_VERSION_MAJOR >= 3
    srs->SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
    #endif

    char **papszOptions = NULL;
    papszOptions = CSLAddString(papszOptions, "OVERWRITE=NO");
    OGRLayer *layer = dataset_p.data()->CreateLayer(ba_name.data(), srs, geom_type, papszOptions);
    CSLDestroy(papszOptions);
    OSRRelease(srs);

    if (layer == NULL)
    {
        error = QObject::tr("Unable to create NGW layer via GDAL");
        error += QString("\nGDAL error: %1").arg(CPLGetLastErrorMsg());
        return false;
    }

    for (int i = 0; i < layer_info.fields.size(); i++)
    {
        QByteArray ba_field_name = std::get<0>(layer_info.fields[i]).toUtf8();
        OGRFieldType field_type = Core::g_findFieldTypeGdal(std::get<2>(layer_info.fields[i]));

        OGRFieldDefn field_defn(ba_field_name.data(), field_type);
        layer->CreateField(&field_defn);

        // TEMP. There may be another way to create field aliases via GDAL in future.
        QString s_field_alias = QString("FIELD_%1_ALIAS").arg(i);
        layer->SetMetadataItem(s_field_alias.toUtf8().data(),
                               std::get<1>(layer_info.fields[i]).toUtf8().data());

        // TODO: delete layer if the creation of field was not successfull? What if at the time
        // between creation and this deletion some other layer have been created outside? How to
        // remove layer in GDAL not by layer index & how to properly get index of a layer in
        // GDALDataset?
    }

    layer->SyncToDisk(); // otherwise NGW id of created resource will not be assigned to the new layer

    auto metadata = layer->GetMetadata("");
    QString s_new_layer_id = {CSLFetchNameValue(metadata, "id")};
    new_layer_id = s_new_layer_id.toInt();

    if (base_url_copy_features != "" && layer_id_copy_features != -1)
    {
        // TODO (important): here we need own credentioals for dataset_p2 !

        GDALDatasetPtr dataset_p2;
        u_openDataset(dataset_p2, base_url_copy_features.toUtf8().data(), layer_id_copy_features, true);
        if (dataset_p2.data() == NULL)
        {
            // TODO: warning
            error = QObject::tr("Unable to open NGW dataset to copy features from");
            //return false;
        }
        else
        {
            OGRLayer *layer2 = dataset_p2.data()->GetLayer(0);
            if (layer2 == NULL)
            {
                // TODO: warning
                error = QObject::tr("Unable to open NGW layer to copy features from");
                //return false;
            }
            else
            {
                layer2->ResetReading();
                OGRFeature *feature;
                while ((feature = layer2->GetNextFeature()) != NULL)
                {
                    layer->CreateFeature(feature);
                    OGRFeature::DestroyFeature(feature);
                }

                layer->SyncToDisk();
            }
        }
    }

    return true;
}

bool NgwGdalIo::createFile (int &new_form_id, QString upload_reply, QString base_url,
                            int layer_id, QString res_type, QString disp_name)
{
    detailed_error = "";
    QString debug_info = QString("\nupload_reply = %1 \nbase_url = %2 \nlayer_id = %3 \nres_type = %4 \ndisp_name = %5")
            .arg(upload_reply).arg(base_url).arg(layer_id).arg(res_type).arg(disp_name);

    QJsonDocument j_reply = QJsonDocument::fromJson(upload_reply.toUtf8());
    if (!j_reply.isObject())
    {
        error = "Unable to upload file to NGW";
        detailed_error = QString("!j_reply.isObject(). ") + debug_info;
        return false;
    }
    QJsonValue j_up_meta = j_reply.object()["upload_meta"];
    if (!j_up_meta.isArray() || j_up_meta.toArray().size() == 0)
    {
        error = "Unable to upload file to NGW";
        detailed_error = QString("!j_up_meta.isArray() || j_up_meta.toArray().size() == 0. ") + debug_info;
        return false;
    }
    QJsonValue j_arr_item = j_up_meta.toArray()[0];
    if (!j_arr_item.isObject())
    {
        error = "Unable to upload file to NGW";
        detailed_error = QString("!j_arr_item.isObject(). ") + debug_info;
        return false;
    }
    QJsonValue j_id = j_arr_item.toObject()["id"];
    if (!j_id.isString())
    {
        error = "Unable to upload file to NGW";
        detailed_error = QString("!j_id.isString(). ") + debug_info;
        return false;
    }

    NgwFileInfo file_info;
    file_info.id = j_id.toString();
    file_info.mime_type = j_arr_item.toObject()["mime_type"].toString();
    file_info.name = j_arr_item.toObject()["name"].toString();
    file_info.size = j_arr_item.toObject()["size"].toInt();

    QString s_url = api->urlCreateResource(base_url);

    QByteArray ba_body = api->bodyCreateFile(file_info, layer_id, res_type, disp_name);

    std::string payload = ba_body.toStdString();
    std::string payload_int = "POSTFIELDS=" + payload;

    char **papszHTTPOptions = NULL;
    papszHTTPOptions = CSLAddString(papszHTTPOptions, "CUSTOMREQUEST=POST");
    papszHTTPOptions = CSLAddString(papszHTTPOptions, payload_int.c_str());
    papszHTTPOptions = CSLAddString(papszHTTPOptions, "HEADERS=Content-Type: application/json\r\nAccept: */*");

    CPLJSONDocument cplj_reply;
    bool ok = cplj_reply.LoadUrl(s_url.toStdString(), papszHTTPOptions);
    CSLDestroy(papszHTTPOptions);
    if (!ok)
    {
        error = "Unable to create form for layer in NGW";
        detailed_error = QString("!cplj_reply.LoadUrl(). ") + debug_info;
        return false;
    }
    CPLJSONObject cplj_root = cplj_reply.GetRoot();
    if (!cplj_root.IsValid())
    {
        error = "Unable to create form for layer in NGW";
        detailed_error = QString("!cplj_root.IsValid(). ") + debug_info;
        return false;
    }

    CPLJSONObject cplj_id = cplj_root.GetObj("id");
    if (!cplj_id.IsValid())
    {
        error = "Unable to create form for layer in NGW";
        detailed_error = QString("!cplj_id.IsValid(). ") + debug_info;
        return false;
    }

    new_form_id = cplj_id.ToInteger();

    return true;
}


QList<QPair<int, QString>> NgwGdalIo::getCollectorUsers (QString base_url)
{
    QList<QPair<int, QString>> users;

    QString url = base_url + "/collector/user";
    QString headers = "Content-Type: application/json\r\nAccept: */*";

    CPLStringList options;
    options.AddNameValue("HEADERS", headers.toStdString().c_str());
    options.AddNameValue("CUSTOMREQUEST", "GET");

    CPLHTTPResult *result = CPLHTTPFetch(url.toStdString().c_str(), options);
    if (result->nStatus != 0 || result->pszErrBuf != nullptr)
    {
        error = QString("CPLHTTPFetch() failed. Info: \nnStatus = %1 \npszErrBuf = %2 \nGDAL error = %3")
            .arg(result->nStatus).arg(result->pszErrBuf == nullptr ? "" : result->pszErrBuf).arg(CPLGetLastErrorMsg());

        CPLHTTPDestroyResult(result);
        return users;
    }

    try
    {
        QByteArray res_data(reinterpret_cast<const char*>(result->pabyData), result->nDataLen);
        QJsonDocument res_doc = QJsonDocument::fromJson(res_data);
        QJsonArray j_arr = res_doc.array();
        for (int i = 0; i < j_arr.size(); i++)
        {
            QJsonValue j_val = j_arr[i];
            QJsonObject j_obj = j_val.toObject();
            users.append({j_obj["id"].toInt(), j_obj["email"].toString()});
        }
    }
    catch (...)
    {
        error = "Failed to extract user infos from returned JSON object";
    }

    CPLHTTPDestroyResult(result);
    return users;
}

bool NgwGdalIo::createCollectorUser (QString base_url, QString email)
{
    QString url = base_url + "/collector/user/new";
    QByteArray payload = QString("{\"descr\":\"\",\"email\":\"%1\"}").arg(email).toUtf8();

    CPLStringList options;
    options.AddNameValue("CUSTOMREQUEST", "PUT");
    options.AddNameValue("HEADERS", "Content-Type: multipart/form-data\r\nAccept: */*");
    options.AddNameValue("FORM_KEY_0", "collector_user");
    options.AddNameValue("FORM_VALUE_0", payload.toStdString().c_str());
    options.AddNameValue("FORM_ITEM_COUNT", "1");

    CPLHTTPResult *result = CPLHTTPFetch(url.toStdString().c_str(), options);
    if (result->nStatus != 0 || result->pszErrBuf != nullptr)
    {
        error = QString("CPLHTTPFetch() failed. Info: \nnStatus = %1 \npszErrBuf = %2 \nGDAL error = %3")
            .arg(result->nStatus).arg(result->pszErrBuf == nullptr ? "" : result->pszErrBuf).arg(CPLGetLastErrorMsg());

        CPLHTTPDestroyResult(result);
        return false;
    }

    bool ok = false;
    try
    {
        QByteArray res_data(reinterpret_cast<const char*>(result->pabyData), result->nDataLen);
        QJsonDocument res_doc = QJsonDocument::fromJson(res_data);
        QJsonObject j_obj = res_doc.object();
        ok = j_obj["result"].toBool();
    }
    catch (...)
    {
        error = "Failed to extract True status from returned JSON object";
    }

    CPLHTTPDestroyResult(result);
    return ok;
}

int NgwGdalIo::createResource (QString base_url, const QJsonObject &body)
{
    QJsonDocument j_doc(body);
    QByteArray payload = j_doc.toJson();
    QString url = api->urlCreateResource(base_url);
    QString headers = "Content-Type: application/json\r\nAccept: */*";

    CPLStringList options;
    options.AddNameValue("HEADERS", headers.toStdString().c_str());
    options.AddNameValue("CUSTOMREQUEST", "POST");
    options.AddNameValue("POSTFIELDS", payload.toStdString().c_str());

    CPLHTTPResult *result = CPLHTTPFetch(url.toStdString().c_str(), options);
    if (result->nStatus != 0 || result->pszErrBuf != nullptr)
    {
        error = QString("CPLHTTPFetch() failed. Info: \nnStatus = %1 \npszErrBuf = %2 \nGDAL error = %3")
            .arg(result->nStatus).arg(result->pszErrBuf == nullptr ? "" : result->pszErrBuf).arg(CPLGetLastErrorMsg());

        CPLHTTPDestroyResult(result);
        return -1;
    }

    int id = -1;
    try
    {
        QByteArray res_data(reinterpret_cast<const char*>(result->pabyData), result->nDataLen);
        QJsonDocument res_doc = QJsonDocument::fromJson(res_data);
        QJsonObject res_obj = res_doc.object();
        id = res_obj["id"].toInt();
    }
    catch (...)
    {
        error = "Failed to extract id from returned JSON object";
    }

    CPLHTTPDestroyResult(result);
    return id;
}


int NgwGdalIo::createSimpleBasemap (QString base_url, int group_id, QString basemap_name)
{
    QJsonObject j_resource;
    j_resource["cls"] = "basemap_layer";
    j_resource["parent"] = QJsonObject({{"id", group_id}});
    j_resource["display_name"] = basemap_name;
    j_resource["keyname"] = QJsonValue();
    j_resource["description"] = QJsonValue();

    QJsonObject j_basemap;
    j_basemap["url"] = "http://c.tile.openstreetmap.org/{z}/{x}/{y}.png";
    j_basemap["qms"] = QJsonValue();

    QJsonObject j_resmeta;
    j_resmeta["items"] = QJsonObject();

    QJsonObject j_obj;
    j_obj["resource"] = j_resource;
    j_obj["basemap_layer"] = j_basemap;
    j_obj["resmeta"] = j_resmeta;

    return createResource(base_url, j_obj);
}

int NgwGdalIo::createSimpleCollectorProject (QString base_url, int group_id, int collector_user_id,
                                             QString user, QString password,
                                             QString proj_name, QString proj_description,
                                             int layer_id, QString layer_name,
                                             int basemap_id, QString basemap_name)
{
    QJsonObject j_resource;
    j_resource["cls"] = "collector_project";
    j_resource["parent"] = QJsonObject({{"id", group_id}});
    j_resource["display_name"] = proj_name;
    j_resource["keyname"] = QJsonValue();
    j_resource["description"] = proj_description;

    QJsonArray j_users;
    j_users.append(collector_user_id);
    QJsonObject j_basemap;
    j_basemap["display_name"] = basemap_name;
    j_basemap["editable"] = false;
    j_basemap["item_type"] = "item";
    j_basemap["lifetime"] = 1440;
    j_basemap["max_zoom"] = 25;
    j_basemap["min_zoom"] = 0;
    j_basemap["resource_id"] = basemap_id;
    j_basemap["syncable"] = false;
    j_basemap["visible"] = true;
    j_basemap["children"] = QJsonArray();
    QJsonObject j_layer;
    j_layer["display_name"] = layer_name;
    j_layer["editable"] = true;
    j_layer["item_type"] = "item";
    j_layer["lifetime"] = 1440;
    j_layer["max_zoom"] = 25;
    j_layer["min_zoom"] = 0;
    j_layer["resource_id"] = layer_id;
    j_layer["syncable"] = true;
    j_layer["visible"] = true;
    j_layer["children"] = QJsonArray();
    QJsonArray j_def_elems;
    j_def_elems.append(j_layer);
    j_def_elems.append(j_basemap);
    QJsonObject j_elems;
    j_elems["item_type"] = "root";
    j_elems["children"] = j_def_elems;
    QJsonObject j_collector;
    j_collector["screen"] = "list";
    j_collector["username"] = user;
    j_collector["password"] = password;
    j_collector["root_item"] = j_elems;
    j_collector["collector_users"] = j_users;

    QJsonObject j_resmeta;
    j_resmeta["items"] = QJsonObject();

    QJsonObject j_obj;
    j_obj["resource"] = j_resource;
    j_obj["collector_project"] = j_collector;
    j_obj["resmeta"] = j_resmeta;

    return createResource(base_url, j_obj);
}

int NgwGdalIo::createSimpleWebmap (QString base_url, int group_id, QString webmap_name, int style_id, QString layer_name)
{
    QJsonObject j_resource;
    j_resource["cls"] = "webmap";
    j_resource["parent"] = QJsonObject({{"id", group_id}});
    j_resource["display_name"] = webmap_name;
    j_resource["keyname"] = QJsonValue();
    j_resource["description"] = QJsonValue();

    QJsonObject j_item;
    j_item["layer_enabled"] = true;
    j_item["layer_adapter"] = "tile";
    j_item["display_name"] = layer_name;
    j_item["layer_style_id"] = style_id;
    j_item["item_type"] = "layer";
    QJsonArray j_items;
    j_items.append(j_item);
    QJsonObject j_rootitem;
    j_rootitem["item_type"] = "root";
    j_rootitem["children"] = j_items;
    QJsonObject j_webmap;
    j_webmap["root_item"] = j_rootitem;

    QJsonObject j_obj;
    j_obj["resource"] = j_resource;
    j_obj["webmap"] = j_webmap;

    return createResource(base_url, j_obj);
}


NgwFormErr NgwGdalIo::downloadForm (QString base_url, int layer_id, QString file_path)
{
    // Get layer's child resources in order to get form id.
    QList<NgwResourceData> resources;
    bool ok = u_getChildResources(resources, base_url, layer_id, {NgwResourceType::FormbuilderForm});
    if (!ok || resources.size() != 1)
    {
        error = QObject::tr("Unable to get form id for the selected layer");
        return NgwFormErr::NoForm;
    }

    // Download form's file.
    QString s_url = api->urlDownloadNgfp(base_url, resources[0].id);
    CPLHTTPResult *http_result = CPLHTTPFetch(s_url.toUtf8().data(), NULL);
    if (http_result == NULL)
    {
        error = "Unable to download NGW form via GDAL";
        return NgwFormErr::Err;
    }

    auto file = VSIFOpenL(file_path.toUtf8().data(), "wb");
    if (file == NULL)
    {
        error = "Unable to open temp file for writing form";
        CPLHTTPDestroyResult(http_result);
        return NgwFormErr::Err;
    }
    auto ret = VSIFWriteL(http_result->pabyData, 1, http_result->nDataAlloc, file);
    if (ret == 0)
    {
        error = "Unable to write downloaded form file";
        VSIFCloseL(file);
        CPLHTTPDestroyResult(http_result);
        return NgwFormErr::Err;
    }

    VSIFCloseL(file);
    CPLHTTPDestroyResult(http_result);

    return NgwFormErr::Ok;
}


QString NgwGdalIo::getUrlResourcePage (QString base_url, int resource_id)
{
    return QString("%1/resource/%2").arg(base_url).arg(resource_id);
}

QString NgwGdalIo::getUrlVersion (QString base_url)
{
    return QString("%1/api/component/pyramid/pkg_version").arg(base_url);
}


bool NgwGdalIo::u_getChildResources (QList<NgwResourceData> &resources,
                                     QString base_url, int resource_id,
                                     const std::set<NgwResourceType> &allowed_types)
{
    QString s_url = api->urlResources(base_url, resource_id);

    char **papszOptions = NULL;
    papszOptions = CSLAddString(papszOptions, "JSON_DEPTH=100"); // need for "webmap" resource cause a structure of all folders is returned
    CPLJSONDocument j_doc;
    bool ok = j_doc.LoadUrl(s_url.toStdString(), papszOptions);
    CSLDestroy(papszOptions);
    if (!ok)
    {
        error = QObject::tr("Unable fetch JSON via CPLJSONDocument");
        return false;
    }

    CPLJSONObject j_root = j_doc.GetRoot();
    if (!j_root.IsValid())
    {
        error = QObject::tr("Incorrect JSON structure");
        return false;
    }

    CPLJSONArray j_root_arr = j_root.ToArray();
    if (!j_root_arr.IsValid())
    {
        error = QObject::tr("Incorrect JSON structure");
        return false;
    }

    resources.clear();

    for (int i = 0; i < j_root_arr.Size(); i++)
    {
        CPLJSONObject j_root_arr_item = j_root_arr[i];
        if (!j_root_arr_item.IsValid())
            continue;

        CPLJSONObject j_res_obj = j_root_arr_item.GetObj("resource");
        if (!j_res_obj.IsValid())
            continue;

        CPLJSONObject j_cls = j_res_obj["cls"];
        std::string s_res_type = j_cls.ToString();
        if (s_res_type == "")
            continue;
        auto res_type = g_findNgwResType(QString::fromStdString(s_res_type));
        if (allowed_types.find(res_type) == allowed_types.end())
            continue;

        CPLJSONObject j_id = j_res_obj["id"];
        int id = j_id.ToInteger(-1);
        if (id == -1)
            continue;

        CPLJSONObject j_children = j_res_obj["children"];
        bool children = j_children.ToBool();
        if (j_children.GetType() != CPLJSONObject::Type::Boolean)
            continue;

        CPLJSONObject j_disp_name = j_res_obj["display_name"];
        std::string disp_name = j_disp_name.ToString();
        if (j_disp_name.GetType() != CPLJSONObject::Type::String)
            continue;

        // Make another request in order to check that vector layer has a form
        bool has_form = false;
//        if (children && res_type == NgwResourceType::VectorLayer)
//        {
//            QList<NgwResourceData> resources;
//            bool ok = u_getChildResources(resources, base_url, id, {NgwResourceType::FormbuilderForm});
//            if (ok && resources.size() == 1)
//                has_form = true;
//        }

        NgwResourceData res_data;
        res_data.id = id;
        res_data.cls = res_type;
        res_data.children = children;
        res_data.display_name = QString::fromStdString(disp_name);
        res_data.has_form = has_form;

        resources.append(res_data);
    }

    return true;
}


void NgwGdalIo::u_openDataset (GDALDatasetPtr &dataset_p, QString base_url, int resource_id, bool read_only)
{
    QString s_url = QString("NGW:%1/resource/%2").arg(base_url).arg(resource_id);
    QByteArray ba_url(s_url.toUtf8());

    char **allowed_drivers = NULL;
    allowed_drivers = CSLAddString(allowed_drivers, "NGW");
    unsigned int open_flags;
    if (read_only)
        open_flags = GDAL_OF_VECTOR | GDAL_OF_READONLY;
    else
        open_flags = GDAL_OF_VECTOR | GDAL_OF_UPDATE;

    dataset_p.reset((GDALDataset*)GDALOpenEx(ba_url.data(),
              open_flags, allowed_drivers, NULL, NULL));

    CSLDestroy(allowed_drivers);
}



