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

#include <QObject>
#include <QMap>

#define FB_NGFP_VERSION 2.2

namespace Fb
{
namespace Serializer
{


const QMap<QString, QString> NGFP_FIELD_KEYS =
{
    {{"field_common"},     {"field"}},
    {{"field_int01"},      {"field"}},
    {{"field_timestamp"},  {"field"}},
    {{"field_top_combo"},  {"field_level1"}},
    {{"field_bot_combo"},  {"field_level2"}},
    {{"field_lat"},        {"field_lat"}},
    {{"field_lon"},        {"field_long"}}
};

inline QString g_ngfpFindFieldSlotName (QString ngfp_key)
{
    for (auto &key: NGFP_FIELD_KEYS.keys())
        if (ngfp_key == NGFP_FIELD_KEYS.value(key))
            return key;
    return "";
}


const QMap<QString, QString> NGFP_ATTR_KEYS =
{
    {{"keep_last"},        {"last"}},
    {{"text"},             {"text"}},
    {{"caption_left"},     {"label1"}},
    {{"caption_right"},    {"label2"}},
    {{"only_nums"},        {"only_figures"}},
    {{"text"},             {"text"}},
    {{"init_text"},        {"text"}},
    {{"init_val"},         {"init_value"}},
    {{"date_type"},        {"date_type"}},
    {{"init_date"},        {"datetime"}},
    {{"srs"},              {"crs"}},
    {{"srs_format"},       {"format"}},
    {{"items"},            {"values"}},
    {{"max_lines"},        {"max_string_count"}},
    {{"ngw_login"},        {"ngw_login"}},
    {{"ngid_login"},       {"ngid_login"}},
    {{"search_items"},     {"input_search"}},
    {{"own_items"},        {"allow_adding_values"}},
    {{"radio_items"},      {"values"}},
    {{"max_photos"},       {"gallery_size"}},
    {{"coords_is_hidden"}, {"hidden"}},
    {{"count_init"},       {"init_value"}},
    {{"count_incr"},       {"increment"}},
    {{"count_pref"},       {"prefix"}},
    {{"count_suff"},       {"suffix"}},

    // The following attributes will be skipped. See NgfpSerializer::modifySpecificElemView()
    //{{"date_is_cur"},      {""}},
    //{{"page_hdrs"},        {""}},
    //{{"cur_page"},         {""}},
};

inline QString g_ngfpFindAttrName (QString ngfp_key)
{
    for (auto &key: NGFP_ATTR_KEYS.keys())
        if (ngfp_key == NGFP_ATTR_KEYS.value(key))
            return key;
    return "";
}


const QMap<QString, QString> NGFP_ELEM_KEYS =
{
    {{"label"},            {"text_label"}},
    {{"voidspace"},        {"space"}},
    {{"textedit"},         {"text_edit"}},
    {{"checkbox"},         {"checkbox"}},
    {{"radiogroup"},       {"radio_group"}},
    {{"combobox"},         {"combobox"}},
    {{"depcomboboxes"},    {"double_combobox"}},
    {{"splitcombobox"},    {"split_combobox"}},
    {{"datetimepicker"},   {"date_time"}},
    {{"coords"},           {"coordinates"}},
    {{"counter"},          {"counter"}},
    {{"signfield"},        {"signature"}},
    {{"photo"},            {"photo"}},
    {{"dmeter"},           {"distance"}},
    {{"tabs"},             {"tabs"}},
};

inline QString g_ngfpFindElemName (QString ngfp_key)
{
    for (auto &key: NGFP_ELEM_KEYS.keys())
        if (ngfp_key == NGFP_ELEM_KEYS.value(key))
            return key;
    return "";
}


}
}



