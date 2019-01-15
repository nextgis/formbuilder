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

#include "attr_registrar.h"
#include "fieldslot_registrar.h"
#include "elem_registrar.h"

// Include here all element factories:
#include "elements/label.h"
#include "elements/voidspace.h"
#include "elements/textedit.h"
#include "elements/checkbox.h"
#include "elements/combobox.h"
#include "elements/depcomboboxes.h"
#include "elements/splitcombobox.h"
#include "elements/radiogroup.h"
#include "elements/datetimepicker.h"
#include "elements/coordinates.h"
#include "elements/counter.h"
#include "elements/dmeter.h"
#include "elements/photo.h"
#include "elements/signfield.h"
#include "elements/ngmform.h"
#include "elements/tabs.h"

namespace Fb
{
namespace Core
{


inline void g_registerAttributes ()
{
    AttrRegistrar::add("keep_last",
    {
       QObject::tr("Keep last value"),
       QObject::tr("Whether to keep last entered value for this element during data collecting "
                   "process")
    });

    AttrRegistrar::add("init_text",
    {
       QObject::tr("Initial text"),
       QObject::tr("Initial text")
    });

    AttrRegistrar::add("init_val",
    {
       QObject::tr("Initial value"),
       QObject::tr("Initial value")
    });

    AttrRegistrar::add("text",
    {
       QObject::tr("Text"),
       QObject::tr("Displayed text")
    });

    AttrRegistrar::add("items",
    {
       QObject::tr("Items"),
       QObject::tr("A list of items")
    });

//    AttrRegistrar::add("is_visible",
//    {
//       QObject::tr("Whether this element is visible to user. An initial value of this element "
//                   "will be written to the layer's field if hidden")
//    });

//    AttrRegistrar::add("is_disabled",
//    {
//       QObject::tr("Is disabled"),
//       QObject::tr("Whether user can interact with this element. An initial value of this "
//                   "element will be written to the layer's field if disabled")
//    });

    AttrRegistrar::add("only_nums",
    {
       QObject::tr("Only numbers"),
       QObject::tr("This element can accept only numbers")
    });

    AttrRegistrar::add("ngw_login",
    {
       QObject::tr("NextGIS Web login"),
       QObject::tr("The value in this text edit will be replaced with user's login if the "
                   "connection with NextGIS Web is established during the data collecting process")
    });

    AttrRegistrar::add("ngid_login",
    {
       QObject::tr("NextGIS ID login"),
       QObject::tr("The value in this text edit will be replaced with user's NextGIS ID login")
    });

    AttrRegistrar::add("max_lines",
    {
       QObject::tr("Max. lines count"),
       QObject::tr("Maximum number of lines for this text edit")
    });

    AttrRegistrar::add("own_items",
    {
        QObject::tr("Own items"),
        QObject::tr("User can add own items to this combobox")
    });

    AttrRegistrar::add("search_items",
    {
        QObject::tr("Input with search"),
        QObject::tr("Whether to show or not corresponding items during typing in this combobox")
    });

    AttrRegistrar::add("radio_items",
    {
       QObject::tr("Items"),
       QObject::tr("A list of items (must have at least two items)")
    });

    AttrRegistrar::add("caption_left",
    {
        QObject::tr("Caption (left)"),
        QObject::tr("Static text which will be placed above this element (at left)")
    });

    AttrRegistrar::add("caption_right",
    {
        QObject::tr("Caption (right)"),
        QObject::tr("Static text which will be placed above this element (at right)")
    });

    AttrRegistrar::add("date_type",
    {
       QObject::tr("Type"),
       QObject::tr("Type")
    });

    AttrRegistrar::add("init_date",
    {
        QObject::tr("Initial value"),
        QObject::tr("Initial value")
    });

    AttrRegistrar::add("date_is_cur",
    {
       QObject::tr("Is current"),
       QObject::tr("Automatically assigns current value during the data collecting process. "
                   "Otherwise user will be able to pick it manually")
    });

    AttrRegistrar::add("srs",
    {
       QObject::tr("CRS"),
       QObject::tr("Coordinate Reference System")
    });

    AttrRegistrar::add("srs_format",
    {
       QObject::tr("Format"),
       QObject::tr("The format of the string which will be written to the layer's field(s)")
    });

    AttrRegistrar::add("coords_is_hidden",
    {
       QObject::tr("Is hidden"),
       QObject::tr("Do not show this element. The coordinates will be written to the layer's "
                   "field(s) anyway")
    });

    AttrRegistrar::add("max_photos",
    {
       QObject::tr("Max. photo count"),
       QObject::tr("Maximum number of photos")
    });

    AttrRegistrar::add("count_init",
    {
       QObject::tr("Initial value"),
       QObject::tr("Initial value from which incrementing starts")
    });

    AttrRegistrar::add("count_incr",
    {
       QObject::tr("Increment"),
       QObject::tr("The value which is added to the current counter's value each time user "
                   "collects data about an object")
    });

    AttrRegistrar::add("count_pref",
    {
       QObject::tr("Prefix"),
       QObject::tr("A text which is added before the counter's value. If this text is not void "
                   "the counter writes its value as a string")
    });

    AttrRegistrar::add("count_suff",
    {
       QObject::tr("Suffix"),
       QObject::tr("A text which is added after the counter's value. If this text is not void "
                   "the counter writes its value as a string")
    });

    AttrRegistrar::add("count_pref_list",
    {
       QObject::tr("Prefix from list"),
       QObject::tr("Prefix from list")
    });

    AttrRegistrar::add("count_suff_list",
    {
       QObject::tr("Suffix from list"),
       QObject::tr("Suffix from list")
    });

    AttrRegistrar::add("cur_page",
    {
       QObject::tr("Current page"),
       QObject::tr("Current page")
    });

    AttrRegistrar::add("page_hdrs",
    {
       QObject::tr("Headers"),
       QObject::tr("A list of header strings")
    });
}


inline void g_registerFieldSlots ()
{
    FieldSlotRegistrar::add("field_common",
    {
       QObject::tr("Field"),
       QObject::tr("Layer's field where this element puts its value")
    });

    FieldSlotRegistrar::add("field_int01",
    {
        QObject::tr("Field"),
        QObject::tr("Layer's field where this element puts its value (integer, 0 or 1)")
    });

    FieldSlotRegistrar::add("field_timestamp",
    {
        QObject::tr("Field"),
        QObject::tr("Layer's field where this element puts its value (long integer timestamp)")
    });

    FieldSlotRegistrar::add("field_top_combo",
    {
        QObject::tr("Field (top list)"),
        QObject::tr("Layer's field where the top combobox puts its value (string)")
    });

    FieldSlotRegistrar::add("field_bot_combo",
    {
        QObject::tr("Field (bottom list)"),
        QObject::tr("Layer's field where the bottom combobox puts its value (string)")
    });

    FieldSlotRegistrar::add("field_lat",
    {
        QObject::tr("Field (lattitude)"),
        QObject::tr("Layer's field where this element puts its lattitude (string)")
    });

    FieldSlotRegistrar::add("field_lon",
    {
        QObject::tr("Field (longitude)"),
        QObject::tr("Layer's field where this element puts its longitude (string)")
    });
}


inline void g_registerElements ()
{
    QString key_name;

    key_name = "label";
    ElemRegistrar::add(key_name,
    {
       ElemCategory::Decoration,
       false, false,
       new LabelFct(key_name),
       "label.svg", "",
       QObject::tr("Label"),
       QObject::tr("An element which displays static text"),
    });

    key_name = "voidspace";
    ElemRegistrar::add(key_name,
    {
       ElemCategory::Decoration,
       false, false,
       new VoidSpaceFct(key_name),
       "void_space.svg", "",
       QObject::tr("Void space"),
       QObject::tr("Void space for making indents"),
    });

    key_name = "textedit";
    ElemRegistrar::add(key_name,
    {
       ElemCategory::Input,
       false, false,
       new TextEditFct(key_name),
       "text_edit.svg", "",
       QObject::tr("Text edit"),
       QObject::tr("An element for editing simple text")
    });

    key_name = "checkbox";
    ElemRegistrar::add(key_name,
    {
       ElemCategory::Input,
       false, false,
       new CheckBoxFct(key_name),
       "check_box.svg", "",
       QObject::tr("Checkbox"),
       QObject::tr("An element which allows user to pick from two values: true or false")
    });

    key_name = "radiogroup";
    ElemRegistrar::add(key_name,
    {
       ElemCategory::Input,
       false, false,
       new RadioGroupFct(key_name),
       "radio_group.svg", "",
       QObject::tr("Radiogroup"),
       QObject::tr("A list with predefined items which allows the selection of only one value")
    });

    key_name = "combobox";
    ElemRegistrar::add(key_name,
    {
       ElemCategory::Input,
       false, false,
       new ComboBoxFct(key_name),
       "combo_box.svg", "",
       QObject::tr("Combobox"),
       QObject::tr("A drop-down list with predefined items which allows the selection of only "
                   "one value")
    });

    key_name = "depcomboboxes";
    ElemRegistrar::add(key_name,
    {
       ElemCategory::Input,
       false, true,
       new DepComboBoxesFct(key_name),
       "dep_combo_boxes.svg", "dep_combo_boxes_dis.svg",
       QObject::tr("Dependent comboboxes"),
       QObject::tr("A pair of drop-down lists with predefined items. The item list of the "
                   "dependent combobox depends on the items of the main combobox")
    });

    key_name = "splitcombobox";
    ElemRegistrar::add(key_name,
    {
       ElemCategory::Input,
       false, true,
       new SplitComboBoxFct(key_name),
       "split_combo_box.svg", "split_combo_box_dis.svg",
       QObject::tr("Splitted combobox"),
       QObject::tr("A drop-down list with predefined items splitted by two parts. Each item has "
                   "an inner value for saving and a pair of displayed values. E.g. the collector "
                   "will be able to see the same item but in different languages")
    });

    key_name = "datetimepicker";
    ElemRegistrar::add(key_name,
    {
       ElemCategory::Input,
       false, false,
       new DateTimePickerFct(key_name),
       "date_time.svg", "",
       QObject::tr("Date & time"),
       QObject::tr("An element which allows to pick a date, time or date&time value")
    });

    key_name = "coords";
    ElemRegistrar::add(key_name,
    {
       ElemCategory::Specific,
       false, true,
       new CoordinatesFct(key_name),
       "coords.svg", "coords_dis.svg",
       QObject::tr("Coordinates"),
       QObject::tr("An element which automatically saves current geographical coordinates in text"
                   "format")
    });

    key_name = "counter";
    ElemRegistrar::add(key_name,
    {
       ElemCategory::Specific,
       false, true,
       new CounterFct(key_name),
       "counter.svg", "counter_dis.svg",
       QObject::tr("Counter"),
       QObject::tr("An element which automatically adds values based on some predefined format")
    });

    key_name = "dmeter";
    ElemRegistrar::add(key_name,
    {
       ElemCategory::Specific,
       false, true,
       new DmeterFct(key_name),
       "distance_meter.svg", "distance_meter_dis.svg",
       QObject::tr("Distance meter"),
       QObject::tr("This element automatically measures distance between current position of the "
                   "point and current position of the surveyor")
    });

    key_name = "signfield";
    ElemRegistrar::add(key_name,
    {
       ElemCategory::Specific,
       false, true,
       new SignFieldFct(key_name),
       "sign_field.svg", "sign_field_dis.svg",
       QObject::tr("Signature field"),
       QObject::tr("A field for signature")
    });

    key_name = "photo";
    ElemRegistrar::add(key_name,
    {
       ElemCategory::Specific,
       false, true,
       new PhotoFct(key_name),
       "photo.svg", "photo_dis.svg",
       QObject::tr("Photo"),
       QObject::tr("An element which allows to take photos with embedded camera or to add them "
                   "from the gallery")
    });

    key_name = "tabs";
    ElemRegistrar::add(key_name,
    {
       ElemCategory::Grouping,
       false, true,
       new TabsFct(key_name),
       "tabs.svg", "tabs_dis.svg",
       QObject::tr("Tabs"),
       QObject::tr("Tabs grouping for other elements")
    });

    key_name = "ngmform";
    ElemRegistrar::add(key_name,
    {
       ElemCategory::AppWidget,
       true, false,
       new NgmFormFct(key_name),
       "", "",
       QObject::tr(""),
       QObject::tr("")
    });
}


inline void g_registerAll ()
{
    g_registerAttributes();
    g_registerFieldSlots();
    g_registerElements();
}


}
}



