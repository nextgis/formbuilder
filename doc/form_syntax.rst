.. sectionauthor:: Mikhail Gusev <mikhail.gusev@nextgis.ru>

General
=======

This page describes the syntax of "form.json" file which is a part of NextGIS
Formbuilder project file (with ".ngfp" extension). The file uses the JSON
specification rules and is encoded in UTF-8.

Syntax
======

.. http:options::

   Example of the "form.json" file which represents a form with two elements for
   entering person's name:

   .. sourcecode:: json

      [
       {
          "attributes" : {
             "text" : "Person"
          },
          "type" : "text_label"
       },
       {
          "attributes" : null,
          "type" : "space"
       },
       {
          "attributes" : {
             "field" : "researcher",
             "last" : true,
             "max_string_count" : 1,
             "ngw_login" : false,
             "only_figures" : false,
             "text" : "John Smith"
          },
          "type" : "text_edit"
       }
      ]

   All elements are always arranged vertically on the form in a strict order, so
   the root of the file has an array type. Each item of the array has an object
   type and contains two pairs with the names: "attributes" and "type".

   :<json jsonobj attributes: Contains several pairs where each name can only be
                              one of the reserved attribute's keynames. The set
                              of pairs depends on the type of the element.
                              ``Note:`` if the element has no attributes the *null*
                              value is correct.
   :<json string type: Can only be one of the reserved elem's keynames.

Elements
========

.. http:options:: Button

   **Keyname:** button

   :<json string field: The name of the layer's field to which this element saves its value. ``Note:`` can be *null* if there was no field selected.
   :<json string text: Text which is displayed as button caption. Can be void string.
   :<json string value: Text which is saved when user presses the button. Can be void string.

   **Example**

   .. sourcecode:: json

      {
          "attributes" : {
             "field" : "comment",
             "text" : "Press me",
             "value" : "Magic button was pressed"
          },
          "type" : "button"
      }

.. http:options:: Checkbox

   **Keyname:** checkbox

   :<json string field: The name of the layer's field to which this element saves its value. ``Note:`` can be *null* if there was no field selected.
   :<json bool init_value: Initial value.
   :<json bool last: Whether to keep value for further sessions of data collection.
   :<json string text: Text which displays near the box of this checkbox. Can be void string.

   **Example**

   .. sourcecode:: json

      {
        "attributes" : {
           "field" : "voice_desc",
           "init_value" : false,
           "last" : false,
           "text" : "Have voice description"
        },
        "type" : "checkbox"
      }

.. http:options:: Combobox

   **Keyname:** combobox

   :<json bool allow_adding_values: Whether to allow user to add own items to
                                    this combobox during data collection.
   :<json string field: The name of the layer's field to which this element
                        saves its value. ``Note:`` can be *null* if there was no
                        field selected.
   :<json bool input_search: Whether to show all corresponding items from this
                             combobox at the time when user types some text.
   :<json bool last: Whether to keep value for further sessions of data collection.
   :<json int ngw_id: The NGW resource identifier for the NGW lookup table or -1
                      if this combobox has no such connection.
                      ``Note:`` all other NextGIS Web connection parameters are stored in "meta.json" file.
   :<json array values: The list of items for this combobox. An array of JSON
                        objects where each one has two (or three) pairs of
                        name-values. See below.
   :<json string alias: Alias (displayed value) for the item. Can not be void string.
   :<json string name: Name (inner value) for the item. Can not be void string.
   :<json bool default: Whether this item is showed as default one before user
                        enters this combobox. The value is always true.

   **Example**

   .. sourcecode:: json

       {
         "attributes" : {
            "allow_adding_values" : false,
            "field" : "tree_type",
            "input_search" : false,
            "last" : true,
            "ngw_id" : -1,
            "values" : [
               {
                  "alias" : "Pine tree",
                  "name" : "1"
               },
               {
                  "alias" : "Oak tree",
                  "default": true,
                  "name" : "2"
               },
               {
                  "alias" : "Olive tree",
                  "name" : "3"
               }
            ]
         },
         "type" : "combobox"
       }

.. http:options:: Coordinates

   **Keyname:** coordinates

   :<json int crs: Coordinate Reference System. Currently always = 0 which
                   refers to "EPSG:4326" (WGS 84).
   :<json string field_lat: The name of the layer's field to which this element
                            saves its latitude value. ``Note:`` can be *null* if
                            there was no field selected.
   :<json string field_long: The name of the layer's field to which this element
                             saves its longitude value. ``Note:`` can be *null* if
                             there was no field selected.
   :<json int format: Format of the string. Currently always = 0 which refers to
                      "dd.dddddd".
   :<json bool hidden: Whether this element will not be shown to user on the
                       mobile device.

   **Example**

   .. sourcecode:: json

       {
         "attributes" : {
            "crs" : 0,
            "field_lat" : "lat_str",
            "field_long" : "lon_str",
            "format" : 0,
            "hidden" : false
         },
         "type" : "coordinates"
       }

.. http:options:: Counter

   **Keyname:** counter

   :<json string field: The name of the layer's field to which this element saves its value. ``Note:`` can be *null* if there was no field selected.
   :<json int increment: The value which is added to the current value each session of data collection. Range: from 1 to 65535.
   :<json int init_value: Initial value from which incrementing starts. Range: from 0 to 65535.
   :<json string prefix: Text string to add before the current counter's number. Can be void string.
   :<json string suffix: Text string to add after the current counter's number. Can be void string.

   **Example**

   .. sourcecode:: json

       {
         "attributes" : {
            "field" : "id_collector",
            "increment" : 1,
            "init_value" : 0,
            "prefix" : "XX",
            "suffix" : "NV"
         },
         "type" : "counter"
       }

.. http:options:: Date and Time

   **Keyname:** date_time

   :<json int date_type: What to save for this date-time element.
                         Possible types: 1) 1 for date, 2) 2 for time, 3) 3 for
                         date and time.
   :<json string datetime: Initial value for this date-time element. The string
                           with date is always written in the specific format.
                           Possible formats (according to types):
                              1) yyyy-MM-dd,
                              2) HH:mm:ss,
                              3) yyyy-MM-dd HH:mm:ss.
                           ``Note:`` can be *null* which means that the current
                                 date/time/date-time will be written on the
                                 mobile device.
   :<json string field: The name of the layer's field to which this element
                        saves its value. ``Note:`` can be *null* if there was no
                        field selected.
   :<json bool last: Whether to keep value for further sessions of data collection.

   **Example**

   .. sourcecode:: json

       {
         "attributes" : {
            "date_type" : 1,
            "datetime" : "2016-01-01 15:00:00",
            "field" : "time",
            "last" : false
         },
         "type" : "date_time"
       }

.. http:options:: Double combobox

   **Keyname:** double_combobox

   :<json string field_level1: The name of the layer's field to which the first (main) combobox saves its value. ``Note:`` can be *null* if there was no field selected.
   :<json string field_level2: The name of the layer's field to which the second (dependant) combobox saves its value. ``Note:`` can be *null* if there was no field selected.
   :<json bool last: Whether to keep values for further sessions of data collection.
   :<json array values: The list of items for the first (main) combobox. An array of JSON objects where each one has three (or four) pairs of name-values. See below. ``Note:`` if user had not defined any items for the first (main) combobox - the stub item is pasted with "alias":"--" and "name":"-1".
   :<json string alias: Alias (displayed value) for the item.
   :<json string name: Name (inner value) for the item.
   :<json bool default: Whether this item is showed as default one before user enters the first (main) combobox. The value is always true.
   :<json array values: The list of items for the second (dependant) combobox. An array of JSON objects where each one has two (or three) pairs of name-values. See below.
   :<json string alias: Alias (displayed value) for the item.
   :<json string name: Name (inner value) for the item.
   :<json bool default: Whether this item is showed as default one before user enters the second (dependant) combobox. The value is always true.

   **Example**

   .. sourcecode:: json

       {
         "attributes" : {
            "field_level1" : "state",
            "field_level2" : "city",
            "last" : true,
            "values" : [
               {
                  "alias" : "California",
                  "name" : "CA",
                  "values" : [
                     {
                        "alias" : "Los Angeles",
                        "name" : "1"
                     },
                     {
                        "alias" : "San Francisco",
                        "name" : "2"
                     },
                     {
                        "alias" : "Sacramento",
                        "default" : true,
                        "name" : "3"
                     }
                  ]
               },
               {
                  "alias" : "New York",
                  "name" : "NY",
                  "values" : [
                     {
                        "alias" : "Albany",
                        "default" : true,
                        "name" : "4"
                     },
                     {
                        "alias" : "New York",
                        "name" : "5"
                     }
                  ]
               },
               {
                  "alias" : "Oregon",
                  "name" : "OR",
                  "values" : [
                     {
                        "alias" : "--",
                        "name" : "-1"
                     }
                  ]
               }
            ]
         },
         "type" : "double_combobox"
       }

.. http:options:: Photo

   **Keyname:** photo

   :<json int gallery_size: The maximum amount of photos user can make. Range: from 1 to 5.

   **Example**

   .. sourcecode:: json

       {
         "attributes" : {
            "gallery_size" : 1
         },
         "type" : "photo"
       }

.. http:options:: Radiogroup

   **Keyname:** radio_group

   :<json string field: The name of the layer's field to which this element saves its value. ``Note:`` can be *null* if there was no field selected.
   :<json bool last: Whether to keep value for further sessions of data collection.
   :<json array values: The list of items for this combobox. An array of JSON objects where each one has two (or three) pairs of name-values. See below. ``Note:`` the array must have at least two items and one of them is always default.
   :<json string alias: Alias (displayed value) for the item. Can not be void string.
   :<json string name: Name (inner value) for the item. Can not be void string.
   :<json bool default: Whether this item is showed as default one before user enters this combobox. The value is always true.

   **Example**

   .. sourcecode:: json

       {
         "attributes" : {
            "field" : "tree_type",
            "last" : false,
            "values" : [
               {
                  "alias" : "Pine tree",
                  "default" : true,
                  "name" : "1"
               },
               {
                  "alias" : "Oak tree",
                  "name" : "2"
               }
            ]
         },
         "type" : "radio_group"
       }

.. http:options:: Signature

   **Keyname:** signature

   **Example**

   .. sourcecode:: json

       {
         "attributes" : null,
         "type" : "signature"
       }

.. http:options:: Space

   **Keyname:** space

   **Example**

   .. sourcecode:: json

       {
         "attributes" : null,
         "type" : "space"
       }

.. http:options:: Text

   **Keyname:** text_label

   :<json string text: Text which displays in this text label. Can be void string.

   **Example**

   .. sourcecode:: json

       {
         "attributes" : {
            "text" : "Biotope"
         },
         "type" : "text_label"
       }

.. http:options:: Text edit

   **Keyname:** text_edit

   :<json string field: The name of the layer's field to which this element saves its value.
                        ``Note:`` can be *null* if there was no field selected.
   :<json bool last: Whether to keep value for further sessions of data collection.
   :<json int max_string_count: How much strings (not symbols!) can be entered to this text edit. Range: from 1 to 65535.
   :<json bool ngw_login: Whether to replace the value of this text edit with NGW login.
   :<json bool only_figures: Whether to allow only numbers for user input to this text edit.
   :<json string text: Initial text which displays in this text edit before user enters it. Can be void string.

   **Example**

   .. sourcecode:: json

       {
         "attributes" : {
            "field" : "researcher",
            "last" : true,
            "max_string_count" : 1,
            "ngw_login" : false,
            "only_figures" : false,
            "text" : "Researcher"
         },
         "type" : "text_edit"
       }
