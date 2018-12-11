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

#include "ngfp_reader.h"

#include "core/field_types.h"
#include "mockup/container.h"
#include "util/json_io.h"
#include "util/zip_io.h"
#include "core/elem_registrar.h"
#include "mockup/elemview_registrar.h"

#include "core/attributes/datetime.h"
#include "core/attributes/doubleitems.h"
#include "core/attributes/tripleitems.h"
#include "core/attributes/depdoubleitems.h"
#include "mockup/elemviews/tabs_view.h"

#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>

using namespace Fb;
using namespace Serializer;

using namespace Core;
using namespace Mockup;
using namespace Util;


void NgfpReader::loadNgfp (QString file_path, Layer *layer, NgmFormView *form)
{
    if (!QFile::exists(file_path))
        throw QString("Unable to find %1").arg(file_path);

    std::string meta_contents;
    std::string form_contents;
    try
    {
        ZipIo::readFile(file_path, "meta.json", meta_contents);
        ZipIo::readFile(file_path, "form.json", form_contents);
    }
    catch (QString &s)
    {
        throw s;
    }

    QByteArray ba_meta_contents(meta_contents.data());
    QJsonDocument j_meta_doc = QJsonDocument::fromJson(ba_meta_contents);
    if (j_meta_doc.isNull())
        throw "meta.json is incorrect JSON file";

    QByteArray ba_form_contents(form_contents.data());
    QJsonDocument j_form_doc = QJsonDocument::fromJson(ba_form_contents);
    if (j_form_doc.isNull())
        throw "form.json is incorrect JSON file";

    last_warnings.clear();

    try
    {
        metaFromJson(j_meta_doc, layer);
        formFromJson(j_form_doc, form, layer);
    }
    catch (QString &s)
    {
        throw s;
    }
}


void NgfpReader::metaFromJson (const QJsonDocument &j_layer, Layer *layer)
{
    if (!j_layer.isObject())
        throw "Meta: not a proper JSON object";
    QJsonObject j_root = j_layer.object();

    QJsonValue j_fields = j_root["fields"];
    if (!j_fields.isArray())
        throw "Not found proper \"fields\" key in .ngfp meta.json";

    QJsonArray j_fields_arr = j_fields.toArray();
    for (int i = 0; i < j_fields_arr.size(); i++)
    {
        if (!j_fields_arr[i].isObject())
        {
            qDebug() << "Not a proper JSON object for field in .ngfp meta.json";
            continue;
        }

        QJsonObject j_field = j_fields_arr[i].toObject();

        QJsonValue j_datatype = j_field["datatype"];
        if (!j_datatype.isString())
        {
            qDebug() << "Not found proper \"datatype\" key for field in .ngfp meta.json";
            continue;
        }

        QJsonValue j_disp_name = j_field["display_name"];
        if (!j_disp_name.isString())
        {
            qDebug() << "Not found proper \"display_name\" key for field in .ngfp meta.json";
            continue;
        }

        QJsonValue j_name = j_field["keyname"];
        if (!j_name.isString())
        {
            qDebug() << "Not found proper \"keyname\" key for field in .ngfp meta.json";
            continue;
        }

        if (!layer->addField(j_name.toString(), g_findFieldType(j_datatype.toString())))
        {
            qDebug() << "Unable to add field from .ngfp meta.json";
            continue;
        }
    }

    QJsonValue j_ngw_con = j_root["ngw_connection"];
/*    if (j_ngw_con.isObject())
    {
        QJsonValue j_id = j_ngw_con["id"];
        if (!j_id.isDouble())
            throw "Not found proper \"id\" key for ngw connection in .ngfp";
        QJsonValue j_login = j_ngw_con["login"];
        if (!j_login.isString())
            throw "Not found proper \"login\" key for ngw connection in .ngfp";
        QJsonValue j_pass = j_ngw_con["password"];
        if (!j_pass.isString())
            throw "Not found proper \"password\" key for ngw connection in .ngfp";
        QJsonValue j_url = j_ngw_con["url"];
        if (!j_url.isString())
            throw "Not found proper \"url\" key for ngw connection in .ngfp";

        layer->setNgwConnection({j_id.toInt(), j_login.toString(), j_pass.toString(), j_url.toString()});
    }
    else */
    if (!j_ngw_con.isNull())
    {
        qDebug() << "Not found proper \"ngw_connection\" key in .ngfp";
    }

    QJsonValue j_name = j_root["name"];
    if (!j_name.isString())
        qDebug() << "Not found proper \"name\" key in .ngfp";
    else
        layer->setName(j_name.toString());

    QJsonValue j_geom_type = j_root["geometry_type"];
    if (!j_name.isString())
        qDebug() << "Not found proper \"geometry_type\" key in .ngfp";
    else
        layer->setGeomType(g_findGeomType(j_geom_type.toString()));

    QJsonValue j_srs = j_root["srs"];
/*
    if (!j_srs.isObject())
    {
        qDebug() << "Not found proper \"srs\" key in .ngfp";
    }
    else
    {
        QJsonValue j_srs_id = j_srs["id"];
        if (!j_srs_id.isString())
            qDebug() << "Not found proper \"id\" key for srs in .ngfp";
        else
            layer->setSrsType(g_findSrsType(j_srs_id.toString()));
    }
*/
    layer->setSrsType(g_findSrsType("4326"));
}


void NgfpReader::formFromJson (const QJsonDocument &j_form, NgmFormView *form, const Layer *layer)
{
    if (!j_form.isArray())
        throw "Form: not a proper JSON object";
    QJsonArray j_form_arr = j_form.array();
    for (int i = 0; i < j_form_arr.size(); i++)
    {
        if (!j_form_arr[i].isObject())
        {
            qDebug() << "Not a proper JSON object for element in .ngfp";
            continue;
        }
        QJsonObject j_elem = j_form_arr[i].toObject();

        QJsonValue j_type = j_elem["type"];
        if (!j_type.isString())
        {
            skipped_elements.append("No proper \"type\" key for element");
            continue;
        }

        QString elem_keyname = g_ngfpFindElemName(j_type.toString());
        auto elem_data = ElemRegistrar::get(elem_keyname);
        auto elemview_fct = ElemViewRegistrar::get(elem_keyname);
        if (elem_data == nullptr || elem_data->fct == nullptr || elemview_fct == nullptr)
        {
            skipped_elements.append(QString("Unable to create \"%1\" element. Incorrect keyname or factory").arg(elem_keyname));
            continue;
        }

        Elem *elem = elem_data->fct->create();
        cur_project.data()->addElem(elem);

        ElemView *elemview = elemview_fct->create(elem);
        cur_screen->setTopLevelElemView(top_elemview);

    }
}



