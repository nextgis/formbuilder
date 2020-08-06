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

#include "ngfp_writer.h"

#include "core/field_types.h"
#include "mockup/container.h"
#include "util/json_io.h"
#include "util/zip_io.h"

#include "core/attributes/datetime.h"
#include "core/attributes/doubleitems.h"
#include "core/attributes/tripleitems.h"
#include "core/attributes/depdoubleitems.h"
#include "core/attributes/globaltextenum.h"
#include "mockup/elemviews/tabs_view.h"

#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QTemporaryDir>

using namespace Fb;
using namespace Serializer;

using namespace Core;
using namespace Mockup;
using namespace Util;


void NgfpWriter::saveNgfp (QString file_path, const Layer *layer, const NgmFormView *form,
                           const QStringList &tr_lang_keys, const QMap<QString, QStringList> &tr_values)
{
    QTemporaryDir temp_dir;
    if (!temp_dir.isValid())
        throw QObject::tr("Unable to create temp directory for saving file");

    QString ngfp_temp_file_path = temp_dir.path() + "/ngfp.zip";

    QString meta_file_name = "meta.json";
    QString meta_file_path = temp_dir.path() + "/" + meta_file_name;
    QString form_file_name = "form.json";
    QString form_file_path = temp_dir.path() + "/" + form_file_name;
    QString data_file_name = "data.geojson";
    QString data_file_path = temp_dir.path() + "/" + data_file_name;

    QJsonDocument j_doc_meta = metaToJson(layer, tr_lang_keys, tr_values);
    QJsonDocument j_doc_form = formToJson(form, layer);
    QJsonDocument j_doc_data;

    try
    {
        JsonIo::write(meta_file_path, j_doc_meta);
        JsonIo::write(form_file_path, j_doc_form);
        JsonIo::write(data_file_path, j_doc_data);
        QList<ZipFileInfo> files = {{meta_file_path, meta_file_name},
                                    {form_file_path, form_file_name},
                                    {data_file_path, data_file_name}};
        ZipIo::writeFiles(ngfp_temp_file_path, files);
    }
    catch (QString &s)
    {
        throw s;
    }

    if (QFile::exists(file_path) && !QFile::remove(file_path))
        throw QObject::tr("Unable to remove old project file with the same name as the saved one");
    if (!QFile::copy(ngfp_temp_file_path, file_path))
        throw QObject::tr("Unable to copy final project file from the temporary directory to the "
                          "selected one");
}


QJsonDocument NgfpWriter::metaToJson (const Layer *layer, const QStringList &tr_lang_keys,
                                      const QMap<QString, QStringList> &tr_values)
{
    QJsonObject j_root;

    QJsonArray j_fields;
    auto fields = layer->getFields();
    for (int i = 0; i < fields.size(); i++)
    {
        QJsonObject j_field;
        j_field["datatype"] = g_getFieldTypeNgwName(fields[i]->getType());
        j_field["keyname"] = fields[i]->getName();
        j_field["display_name"] = fields[i]->getAlias();
        j_fields.append(j_field);
    }

    QJsonValue j_ngw_con; // null
//    auto &ngw_con = layer->getNgwConnection();
//    if (ngw_con.resource_id != -1)
//    {
//        QJsonObject j_obj;
//        j_obj["id"] = ngw_con.resource_id;
//        j_obj["login"] = ngw_con.login;
//        j_obj["password"] = "";//ngw_con.password;
//        j_obj["url"] = "";//ngw_con.base_url;
//        j_ngw_con = j_obj;
//    }

    QJsonObject j_srs;
    j_srs["id"] = g_getSrsTypeNgwNumber(layer->getSrsType());

    QJsonArray j_translations;
    if (tr_lang_keys.size() > 0)
    {
        auto phrase_keys = tr_values.keys();
        for (auto &phrase_key: phrase_keys)
        {
            QJsonObject j_translation;
            j_translation["key"] = phrase_key;
            for (int i = 0; i < tr_lang_keys.size(); i++)
                j_translation[tr_lang_keys[i]] = tr_values[phrase_key][i];
            j_translations.append(j_translation);
        }
    }

    QJsonValue j_lists; // null by default
    QJsonValue j_key_list;
    auto lists_pair = layer->getLists();
    auto lists = lists_pair.first;
    auto key_list = lists_pair.second;
    if (lists.size() != 0 && lists[0].size() != 0)
    {
        j_key_list = key_list;
        QJsonObject j_lists_obj;
        for (int i = 0; i < lists.size(); i++)
        {
            QJsonArray j_list_array;
            for (int j = 1; j < lists[i].size(); j++)
                j_list_array.append(lists[i][j]);
            j_lists_obj[lists[i][0]] = j_list_array;
        }
        j_lists = j_lists_obj;
    }

    j_root["fields"] = j_fields;
    j_root["name"] = layer->getName();
    j_root["geometry_type"] = g_getGeomTypeNgwName(layer->getGeomType());
    j_root["key_list"] = j_key_list;
    j_root["lists"] = j_lists;
    j_root["ngw_connection"] = j_ngw_con;
    j_root["srs"] = j_srs;
    j_root["translations"] = j_translations;
    j_root["version"] = QString::number(FB_NGFP_VERSION, 'f', 1);

    QJsonDocument j_doc(j_root);
    return j_doc;
}


QJsonDocument NgfpWriter::formToJson (const NgmFormView *form, const Layer *layer)
{
    if (form == nullptr || layer == nullptr)
        return QJsonDocument();

    QJsonArray j_root;

    const Container *container = form->getContainer(); // only one vertical container in NgmFormView
    j_root = containerToJson(container, layer);

    QJsonDocument j_doc(j_root);
    return j_doc;
}


QJsonArray NgfpWriter::containerToJson (const Container *container, const Layer *layer)
{
    QJsonArray j_arr;

    QLayout *lay = container->layout();
    for (int i = 0; i < lay->count(); i++)
    {
        QLayoutItem *item = lay->itemAt(i);
        if (item == nullptr)
        {
            //qDebug
            continue;
        }

        ElemView *elemview = qobject_cast<ElemView*>(item->widget());
        if (elemview == nullptr)
        {
            //qDebug
            continue;
        }

        QJsonValue j_elemview = elemViewToJson(elemview, layer);
        modifySpecificElemView(j_elemview, elemview, layer); // some magic required by .ngfp format

        j_arr.append(j_elemview);
    }

    return j_arr;
}


QJsonValue NgfpWriter::elemViewToJson (const ElemView *elemview, const Layer* layer)
{
    if (elemview == nullptr)
        return QJsonValue(); // null

    const QString &elem_name = elemview->getKeyName();
    const Elem *elem = elemview->getElem();

    QJsonObject j_elem;
    QJsonObject j_attrs;

    auto attrs = elem->getAttrs();
    auto field_slots = elem->getFieldSlots().keys();

    QString elem_ngfp_key = NGFP_ELEM_KEYS.value(elem_name, "");
    if (elem_ngfp_key == "")
    {
        //qDebug
        return QJsonValue(); // null
    }

    for (auto &attr: attrs)
    {
        QString attr_name = attr->getKeyName();
        QString ngfp_key = NGFP_ATTR_KEYS.value(attr_name, "");
        if (ngfp_key == "")
        {
            //qDebug()
            continue;
        }
        j_attrs[ngfp_key] = attrToJson(attr);
    }

    for (auto &field_slot: field_slots)
    {
        QString ngfp_key = NGFP_FIELD_KEYS.value(field_slot, "");
        if (ngfp_key == "")
        {
            //qDebug
            continue;
        }
        j_attrs[ngfp_key] = fieldSlotToJson(layer, elem, field_slot);
    }

    j_elem["type"] = elem_ngfp_key;
    if (j_attrs.isEmpty())
        j_elem["attributes"] = QJsonValue(); // null if no attributes / fields
    else
        j_elem["attributes"] = j_attrs;

    return j_elem;
}


QJsonValue NgfpWriter::attrToJson (const Attr *attr)
{
    if (attr == nullptr)
        return QJsonValue(); // null

    QVariant var = attr->getValueAsVar();
    AttrInputType input_type = attr->getInputType();

    if (input_type == AttrInputType::Boolean)
    {
        return var.toBool();
    }

    else if (input_type == AttrInputType::Number)
    {
        return var.toDouble();
    }

    else if (input_type == AttrInputType::String)
    {
        return var.toString();
    }

    else if (input_type == AttrInputType::StringList ||
             input_type == AttrInputType::PageList)
    {
        QJsonArray j_arr;
        const QStringList &strs = var.toStringList();
        for (auto &str: strs)
            j_arr.append(str);
        return j_arr;
    }

    else if (input_type == AttrInputType::DateTime)
    {
        return var.toDateTime().toString(FB_NGFP_DATETIME_FORMAT_DT);
    }

    else if (input_type == AttrInputType::Enum)
    {
        return var.toInt();
    }

    else if (input_type == AttrInputType::GlobalTextEnum)
    {
        return var.toString();
    }

    else if (input_type == AttrInputType::DoubleItems)
    {
        QJsonArray j_arr;
        const auto &value = var.value<DoubleItemsValue>();
        if (value.inners.size() == 0)
        {
            return QJsonValue(); // null
        }
        else
        {
            for (int i = 0; i < value.inners.size(); i++)
            {
                QJsonObject j_item;

                j_item["name"] = value.inners[i];
                j_item["alias"] = value.outers[i];
                if (value.def_index == i)
                    j_item["default"] = true;

                j_arr.append(j_item);
            }
        }
        return j_arr;
    }

    else if (input_type == AttrInputType::TripleItems)
    {
        QJsonArray j_arr;
        const auto &value = var.value<TripleItemsValue>();
        if (value.inners.size() == 0)
        {
            return QJsonValue(); // null
        }
        else
        {
            for (int i = 0; i < value.inners.size(); i++)
            {
                QJsonObject j_item;

                j_item["name"] = value.inners[i];
                j_item["alias"] = value.outers_left[i];
                j_item["alias2"] = value.outers_right[i];
                if (value.def_index == i)
                    j_item["default"] = true;

                j_arr.append(j_item);
            }
        }
        return j_arr;
    }

    else if (input_type == AttrInputType::DepDoubleItems)
    {
        QJsonArray j_arr;
        const auto &value = var.value<DepDoubleItemsValue>();
        for (int i = 0; i < value.main.inners.size(); i++)
        {
            QJsonObject j_item;

            j_item["name"] = value.main.inners[i];
            j_item["alias"] = value.main.outers[i];
            if (value.main.def_index == i)
                j_item["default"] = true;

            QJsonArray j_arr2;
            if (value.deps[i].inners.size() == 0)
            {
                QJsonObject j_item2;
                j_item2["name"] = "-1";
                j_item2["alias"] = "--";
                j_arr2.append(j_item2);
            }
            else
            {
                for (int j = 0; j < value.deps[i].inners.size(); j++)
                {
                    QJsonObject j_item2;
                    j_item2["name"] = value.deps[i].inners[j];
                    j_item2["alias"] = value.deps[i].outers[j];
                    if (value.deps[i].def_index == j)
                        j_item2["default"] = true;
                    j_arr2.append(j_item2);
                }
            }
            j_item["values"] = j_arr2;

            j_arr.append(j_item);
        }
        return j_arr;
    }

    return QJsonValue(); // null
}


QJsonValue NgfpWriter::fieldSlotToJson (const Layer *layer, const Elem *elem, QString field_slot)
{
    auto fields = layer->getFields();
    for (auto field: fields)
    {
        if (field->getElem() == elem && field->getSlot() == field_slot)
            return field->getName();
    }
    return QJsonValue(); // null: the slot is not connected to a field
}


// Some magic required by .ngfp format.
void NgfpWriter::modifySpecificElemView (QJsonValue &j_elemview, const ElemView *elemview,
                                             const Layer *layer)
{
    auto modifyTabs = [&]()
    {
        const Elem *elem = elemview->getElem();
        auto attrs = elem->getAttrs();
        QJsonObject j_new_obj = j_elemview.toObject();
        QJsonObject j_attrs = j_new_obj["attributes"].toObject();

        const TabsView *tabsview = qobject_cast<const TabsView*>(elemview);
        if (tabsview == nullptr)
            return;

        Core::Attr *attr_headers = attrs.value("page_hdrs");
        QStringList headers = attr_headers->getValueAsVar().toStringList();

        Core::Attr *attr_cur_page = attrs.value("cur_page");
        int cur_index = attr_cur_page->getValueAsVar().toInt() - 1; // cur page index starts from 1

        QJsonArray j_pages_arr;

        auto containers = tabsview->getAllContainers();
        for (int i = 0; i < containers.size(); i++)
        {
            QJsonObject j_page;

            j_page["elements"] = containerToJson(containers[i], layer); // recursion
            j_page["caption"] = headers[i];
            if (i == cur_index)
                j_page["default"] = true;

            j_pages_arr.append(j_page);
        }

        j_new_obj["pages"] = j_pages_arr;

        j_new_obj["attributes"] = j_attrs;
        j_elemview = j_new_obj;
    };


    auto modifyDateTimePicker = [&]()
    {
        const Elem *elem = elemview->getElem();
        auto attrs = elem->getAttrs();
        QJsonObject j_new_obj = j_elemview.toObject();
        QJsonObject j_attrs = j_new_obj["attributes"].toObject();

        Core::Attr *attr_date_is_cur = attrs.value("date_is_cur");
        if (attr_date_is_cur->getValueAsVar().toBool() == true)
        {
            j_attrs["datetime"] = QJsonValue(); // null
        }
        else
        {
            Core::Attr *attr_date_type = attrs.value("date_type");
            Core::Attr *attr_date_init = attrs.value("init_date");

            int date_type = attr_date_type->getValueAsVar().toInt();
            QString format;
            switch (date_type)
            {
                case 0: format = FB_NGFP_DATETIME_FORMAT_D; break;
                case 1: format = FB_NGFP_DATETIME_FORMAT_T; break;
                default: format = FB_NGFP_DATETIME_FORMAT_DT; break;
            }

            QDateTime date = attr_date_init->getValueAsVar().toDateTime();
            j_attrs["datetime"] = date.toString(format);
        }

        j_new_obj["attributes"] = j_attrs;
        j_elemview = j_new_obj;
    };


    auto modifyComboBox = [&]()
    {
        QJsonObject j_new_obj = j_elemview.toObject();
        QJsonObject j_attrs = j_new_obj["attributes"].toObject();

        j_attrs["ngw_id"] = -1;

        j_new_obj["attributes"] = j_attrs;
        j_elemview = j_new_obj;
    };


    auto modifyCounter = [&]()
    {
        const Elem *elem = elemview->getElem();
        auto attrs = elem->getAttrs();
        QString sel_pref_list = attrs.value("count_pref_list")->getValueAsVar().toString();
        QString sel_suff_list = attrs.value("count_suff_list")->getValueAsVar().toString();

        QJsonObject j_new_obj = j_elemview.toObject();
        QJsonObject j_attrs = j_new_obj["attributes"].toObject();

        if (sel_pref_list != "")
            j_attrs["prefix_from_list"] = sel_pref_list;
        else
            j_attrs["prefix_from_list"] = QJsonValue(); // null. Could be if no prefix from list was selected or the selection is incorrect (due to changes in list columns)

        if (sel_suff_list != "")
            j_attrs["suffix_from_list"] = sel_suff_list;
        else
            j_attrs["suffix_from_list"] = QJsonValue();

        j_new_obj["attributes"] = j_attrs;
        j_elemview = j_new_obj;
    };


    auto modifyTextEdit = [&]()
    {
        const Elem *elem = elemview->getElem();
        auto attrs = elem->getAttrs();
        QJsonObject j_new_obj = j_elemview.toObject();
        QJsonObject j_attrs = j_new_obj["attributes"].toObject();
        Core::Attr *attr_ngw_login = attrs.value("ngw_login");
        Core::Attr *attr_ngid_login = attrs.value("ngid_login");

        if (attr_ngw_login->getValueAsVar().toBool() == true ||
            attr_ngid_login->getValueAsVar().toBool() == true)
            j_attrs["text"] = QJsonValue(); // null

        j_new_obj["attributes"] = j_attrs;
        j_elemview = j_new_obj;
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



