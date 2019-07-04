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

#include "ngfp_reader.h"

#include "core/field_types.h"
#include "mockup/container.h"
#include "mockup/screen.h"
#include "util/json_io.h"
#include "util/zip_io.h"
#include "core/elem_registrar.h"
#include "mockup/elemview_registrar.h"
#include "core/attributes/number.h"

#include "core/attributes/datetime.h"
#include "core/attributes/doubleitems.h"
#include "core/attributes/tripleitems.h"
#include "core/attributes/depdoubleitems.h"
#include "mockup/elemviews/tabs_view.h"

#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

using namespace Fb;
using namespace Serializer;

using namespace Core;
using namespace Mockup;
using namespace Util;


QStringList NgfpReader::last_warnings;

void NgfpReader::loadNgfp (QString file_path, Project *project, Screen *screen, NgmFormView *form)
{
    if (form == nullptr || project == nullptr)
        return;

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
        metaFromJson(j_meta_doc, project);
        formFromJson(j_form_doc, form, project, screen);
    }
    catch (QString &s)
    {
        throw s;
    }
}


void NgfpReader::metaFromJson (const QJsonDocument &j_layer, Project *project)
{
    if (!j_layer.isObject())
        throw "Meta: not a proper JSON object";
    QJsonObject j_root = j_layer.object();

    QJsonValue j_fields = j_root["fields"];
    if (!j_fields.isArray())
        throw "Not found proper \"fields\" key in meta.json";

    Layer *layer = project->temp_getLayer();

    QJsonArray j_fields_arr = j_fields.toArray();
    for (int i = 0; i < j_fields_arr.size(); i++)
    {
        if (!j_fields_arr[i].isObject())
        {
            qDebug() << "Not a proper JSON object for field in meta.json";
            continue;
        }

        QJsonObject j_field = j_fields_arr[i].toObject();

        QJsonValue j_datatype = j_field["datatype"];
        if (!j_datatype.isString())
        {
            qDebug() << "Not found proper \"datatype\" key for field in meta.json";
            continue;
        }

        QJsonValue j_disp_name = j_field["display_name"];
        if (!j_disp_name.isString())
        {
            qDebug() << "Not found proper \"display_name\" key for field in meta.json";
            continue;
        }

        QJsonValue j_name = j_field["keyname"];
        if (!j_name.isString())
        {
            qDebug() << "Not found proper \"keyname\" key for field in meta.json";
            continue;
        }

        if (!layer->addField(j_name.toString(), j_disp_name.toString(),
                             g_findFieldType(j_datatype.toString())))
        {
            qDebug() << "Unable to add field from .ngfp meta.json";
            continue;
        }
    }

    QJsonValue j_ngw_con = j_root["ngw_connection"];
//    if (j_ngw_con.isObject())
//    {
//        QJsonValue j_id = j_ngw_con["id"];
//        if (!j_id.isDouble())
//            throw "Not found proper \"id\" key for ngw connection in .ngfp";
//        QJsonValue j_login = j_ngw_con["login"];
//        if (!j_login.isString())
//            throw "Not found proper \"login\" key for ngw connection in .ngfp";
//        QJsonValue j_pass = j_ngw_con["password"];
//        if (!j_pass.isString())
//            throw "Not found proper \"password\" key for ngw connection in .ngfp";
//        QJsonValue j_url = j_ngw_con["url"];
//        if (!j_url.isString())
//            throw "Not found proper \"url\" key for ngw connection in .ngfp";

//        layer->setNgwConnection({j_id.toInt(), j_login.toString(), j_pass.toString(), j_url.toString()});
//    }
//    else
    if (!j_ngw_con.isNull())
    {
        qDebug() << "A key \"ngw_connection\" must have null value in meta.json (temporary)";
    }

    QJsonValue j_name = j_root["name"];
    if (!j_name.isString())
        qDebug() << "Not found proper \"name\" key in meta.json";
    else
        layer->setName(j_name.toString());

    QJsonValue j_geom_type = j_root["geometry_type"];
    if (!j_name.isString())
        qDebug() << "Not found proper \"geometry_type\" key in meta.json";
    else
        layer->setGeomType(g_findGeomType(j_geom_type.toString()));

    layer->setSrsType(g_findSrsType("4326"));

    QJsonValue j_key_list = j_root["key_list"];
    QJsonValue j_lists = j_root["lists"];
    if (!(j_key_list.isNull() && j_lists.isNull()))
    {
        QList<QStringList> lists;
        if (!j_key_list.isString())
            qDebug() << "Not found proper \"key_list\" key in meta.json";
        if (!j_lists.isObject())
            qDebug() << "Not found proper \"lists\" key in meta.json";
        auto j_lists_obj = j_lists.toObject();
        auto list_keys = j_lists_obj.keys();
        for (auto list_key: list_keys)
        {
            auto j_arr_v = j_lists_obj[list_key];
            if (!j_arr_v.isArray())
                continue;
            auto j_arr = j_arr_v.toArray();
            QStringList list;
            list.append(list_key);
            for (auto j_arr_str: j_arr)
                list.append(j_arr_str.toString());
            lists.append(list);
        }
        layer->setLists({lists, j_key_list.toString()});
    }
}


void NgfpReader::formFromJson (const QJsonDocument &j_form, NgmFormView *form, Project *project, Screen *screen)
{
    if (!j_form.isArray())
        throw "Form: not a proper JSON object";
    QJsonArray j_form_arr = j_form.array();

    containerFromJson(j_form_arr, const_cast<Container*>(form->getContainer()), project, screen);
}


void NgfpReader::containerFromJson (const QJsonArray &j_container, Container *container, Project *project, Screen *screen)
{
    for (int i = 0; i < j_container.size(); i++)
    {
        if (!j_container[i].isObject())
        {
            last_warnings.append("Not a proper JSON object for element in .ngfp");
            continue;
        }
        QJsonValue j_elemview = j_container[i];

        QJsonValue j_type = j_elemview.toObject()["type"];
        if (!j_type.isString())
        {
            last_warnings.append(QString("No proper \"type\" key for element %1").arg(i));
            continue;
        }

        QString elem_keyname = g_ngfpFindElemName(j_type.toString());
        auto elem_data = ElemRegistrar::get(elem_keyname);
        auto elemview_fct = ElemViewRegistrar::get(elem_keyname);
        if (elem_data == nullptr || elem_data->fct == nullptr || elemview_fct == nullptr)
        {
            last_warnings.append(QString("Unable to create \"%1\" element. Incorrect keyname or "
                                         "factory").arg(elem_keyname));
            continue;
        }

        Elem *elem = elem_data->fct->create();
        ElemView *elemview = elemview_fct->create(elem);
        QObject::connect(elemview, &ElemView::needToRemoveElem,
                         project, &Core::Project::onNeedToRemoveElem);

        elemViewFromJson(j_elemview.toObject(), elemview, project);
        modifySpecificElemView(j_elemview, elemview, project, screen);

        elem->behave(); // because attribute values have been just loaded from file

        project->addElem(elem);
        screen->space_showed = container->getLastSpace();
        screen->endMoveNewElemView(elemview);

        elemview->updateBindingStyle(project->temp_getLayer()->getFields());
    }
}

void NgfpReader::elemViewFromJson (const QJsonObject &j_elemview, ElemView *elemview, Project *project)
{
    Elem *elem = const_cast<Elem*>(elemview->getElem());
    QString elem_keyname = elem->getKeyName();

    QJsonObject j_attrs = j_elemview.value("attributes").toObject();

    auto attrs = elem->getAttrs();
    for (auto &attr: attrs)
    {
        QString attr_fb_keyname = attr->getKeyName();
        QString attr_ngfp_keyname = NGFP_ATTR_KEYS.value(attr_fb_keyname, "");

        QJsonValue j_attr = j_attrs.value(attr_ngfp_keyname);
        if (j_attr.isUndefined())
        {
            last_warnings.append(QString("A key \"%1\" was not found in form.json for the \"%2\" "
                                         "elem. The default value is used")
                                 .arg(attr_fb_keyname).arg(elem_keyname));
            continue;
        }

        attrFromJson(j_attr, attr);
    }

    elem->behave(); // in order to bind fields correctly next

    auto field_slots = elem->getFieldSlots().keys();
    for (auto &field_slot: field_slots)
    {
        QString fs_ngfp_keyname = NGFP_FIELD_KEYS.value(field_slot, "");

        QJsonValue j_attr = j_attrs.value(fs_ngfp_keyname);
        if (j_attr.isUndefined())
        {
            last_warnings.append(QString("A key \"%1\" was not found in form.json for the \"%2\" "
                                         "elem. No field binding is used instead")
                                 .arg(fs_ngfp_keyname).arg(elem_keyname));
            continue;
        }

        fieldSlotFromJson(j_attr.toString(""), project, elem, field_slot);
    }
}

void NgfpReader::attrFromJson (const QJsonValue &j_attr, Attr *attr)
{
    AttrInputType input_type = attr->getInputType();
    QVariant var;

    if (input_type == AttrInputType::Boolean)
    {
        var = j_attr.toBool();
    }

    else if (input_type == AttrInputType::Number)
    {
        var = j_attr.toInt();
    }

    else if (input_type == AttrInputType::String)
    {
        var = j_attr.toString();
    }

    else if (input_type == AttrInputType::StringList ||
             input_type == AttrInputType::PageList)
    {
        QStringList list;
        QJsonArray j_arr = j_attr.toArray(QJsonArray());
        for (int i = 0; i < j_arr.size(); i++)
            list.append(j_arr[i].toString());
        var = list;
    }

    else if (input_type == AttrInputType::DateTime)
    {
        var = QDateTime::fromString(j_attr.toString(), FB_NGFP_DATETIME_FORMAT_DT);
    }

    else if (input_type == AttrInputType::Enum)
    {
        var = j_attr.toInt();
    }

    else if (input_type == AttrInputType::GlobalTextEnum)
    {
        if (j_attr.isNull())
            var = "";
        else
            var = j_attr.toString();
    }

    else if (input_type == AttrInputType::DoubleItems)
    {
        DoubleItemsValue value;
        QJsonArray j_arr = j_attr.toArray(QJsonArray());
        value.def_index = -1;
        for (int i = 0; i < j_arr.size(); i++)
        {
            QJsonValue j_arr_item = j_arr[i];
            value.inners.append(j_arr_item.toObject()["name"].toString());
            value.outers.append(j_arr_item.toObject()["alias"].toString());
            if (!j_arr_item.toObject().value("default").isUndefined())
                value.def_index = i;
        }
        var = QVariant::fromValue<DoubleItemsValue>(value);
    }

    else if (input_type == AttrInputType::TripleItems)
    {
        TripleItemsValue value;
        QJsonArray j_arr = j_attr.toArray(QJsonArray());
        value.def_index = -1;
        for (int i = 0; i < j_arr.size(); i++)
        {
            QJsonValue j_arr_item = j_arr[i];
            value.inners.append(j_arr_item.toObject()["name"].toString());
            value.outers_left.append(j_arr_item.toObject()["alias"].toString());
            value.outers_right.append(j_arr_item.toObject()["alias2"].toString());
            if (!j_arr_item.toObject().value("default").isUndefined())
                value.def_index = i;
        }
        var = QVariant::fromValue<TripleItemsValue>(value);
    }

    else if (input_type == AttrInputType::DepDoubleItems)
    {
        DepDoubleItemsValue value;
        QJsonArray j_arr = j_attr.toArray(QJsonArray());
        value.main.def_index = -1;
        for (int i = 0; i < j_arr.size(); i++)
        {
            QJsonValue j_arr_item = j_arr[i];
            value.main.inners.append(j_arr_item.toObject()["name"].toString());
            value.main.outers.append(j_arr_item.toObject()["alias"].toString());
            if (!j_arr_item.toObject().value("default").isUndefined())
                value.main.def_index = i;
            value.deps.append(DoubleItemsValue());
            QJsonValue j_arr2_val = j_arr_item.toObject()["values"];
            QJsonArray j_arr2 = j_arr2_val.toArray(QJsonArray());
            if (!(j_arr2.size() == 1 &&
                j_arr2[0].toObject()["name"].toString() == "-1" &&
                j_arr2[0].toObject()["alias"].toString() == "--"))
            {
                value.deps.last().def_index = -1;
                for (int i2 = 0; i2 < j_arr2.size(); i2++)
                {
                    QJsonValue j_arr_item2 = j_arr2[i2];
                    value.deps.last().inners.append(j_arr_item2.toObject()["name"].toString());
                    value.deps.last().outers.append(j_arr_item2.toObject()["alias"].toString());
                    if (!j_arr_item2.toObject().value("default").isUndefined())
                        value.deps.last().def_index = i2;
                }
            }
        }
        var = QVariant::fromValue<DepDoubleItemsValue>(value);
    }

    attr->setValueAsVar(var);
}

void NgfpReader::fieldSlotFromJson (QString field_name, Project *project, Elem *elem, QString slot_name)
{
    if (field_name == "")
        return;

    Layer *layer = project->temp_getLayer();

    Field *field = layer->getField(field_name);
    if (field == nullptr)
    {
        last_warnings.append(QString("A field named \"%1\" was found in element, but was not found"
                                     "in a layer. This element will have no binding with any field")
                             .arg(field_name));
        return;
    }

    field->bind(elem, slot_name);
}


// Some magic required by .ngfp format.
void NgfpReader::modifySpecificElemView (const QJsonValue &j_elemview, ElemView *elemview, Project *project, Screen *screen)
{
    auto modifyTabs = [&]()
    {
        auto attrs = elemview->getElem()->getAttrs();

        QJsonArray j_pages = j_elemview.toObject()["pages"].toArray();

        TabsView *tabsview = qobject_cast<TabsView*>(elemview);
        if (tabsview == nullptr)
            return;

        int cur_page = 0;
        QStringList page_hdrs;

        tabsview->removeLastPage();
        tabsview->removeLastPage();

        for (int i = 0; i < j_pages.size(); i++)
        {
            page_hdrs.append(j_pages[i].toObject()["caption"].toString());

            if (!j_pages[i].toObject().value("default").isUndefined())
                cur_page = i;

            tabsview->appendPage();

            containerFromJson(j_pages[i].toObject()["elements"].toArray(),
                    tabsview->getAllContainers().last(), project, screen); // recursion
        }

        Attr* attr_page_hdrs = attrs.value("page_hdrs");
        attr_page_hdrs->setValueAsVar(page_hdrs);
        tabsview->headers = page_hdrs;

        Attr* attr_cur_page = attrs.value("cur_page");
        static_cast<Number*>(attr_cur_page)->setMax(page_hdrs.size());
        static_cast<Number*>(attr_cur_page)->setValue(cur_page + 1); // because this attr values starts from 1
        tabsview->cur_container_index = cur_page;
    };


    auto modifyDateTimePicker = [&]()
    {
        auto attrs = elemview->getElem()->getAttrs();
        QJsonObject j_attrs = j_elemview.toObject()["attributes"].toObject();
        if (j_attrs["datetime"].isNull())
        {
            attrs["date_is_cur"]->setValueAsVar(true);
        }
        else
        {
            attrs["date_is_cur"]->setValueAsVar(false);

            if (j_attrs["date_type"].toInt() == 0)
                attrs["init_date"]->setValueAsVar(QDateTime::fromString(j_attrs["datetime"].toString(), FB_NGFP_DATETIME_FORMAT_D));
            else if (j_attrs["date_type"].toInt() == 1)
                attrs["init_date"]->setValueAsVar(QDateTime::fromString(j_attrs["datetime"].toString(), FB_NGFP_DATETIME_FORMAT_T));
            else
                attrs["init_date"]->setValueAsVar(QDateTime::fromString(j_attrs["datetime"].toString(), FB_NGFP_DATETIME_FORMAT_DT));
        }
    };


    auto modifyComboBox = [&]()
    {
        // nothing to do here for reading
    };


    auto modifyCounter = [&]()
    {
        // nothing to do here for reading
    };


    auto modifyTextEdit = [&]()
    {
        // nothing to do here for reading
    };


    if (!j_elemview.isObject())
        return;
    if (elemview == nullptr)
        return;

    typedef std::function<void()> ModifierCallback;
    const QString &elem_name = elemview->getKeyName();
    const QMap<QString, ModifierCallback> modifiers =
    {
        {{"tabs"},           {modifyTabs}},
        {{"datetimepicker"}, {modifyDateTimePicker}},
        {{"combobox"},       {modifyComboBox}},
        {{"counter"},        {modifyCounter}},
        {{"textedit"},       {modifyTextEdit}}
    };

    if (!modifiers.contains(elem_name))
        return;

    modifiers.value(elem_name)(); // call function
}



