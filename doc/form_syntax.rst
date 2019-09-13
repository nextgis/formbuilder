.. sectionauthor:: Mikhail Gusev <mikhail.gusev@nextgis.ru>

General
=======

This page describes the syntax of "form.json" file which is a part of NextGIS
Formbuilder project file (with ".ngfp" extension). The file uses the JSON
specification rules and is encoded in UTF-8.

Syntax
======

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
         "ngid_login" : false,
         "ngw_login" : false,
         "only_figures" : false,
         "text" : "John Smith"
      },
      "type" : "text_edit"
   }
  ]

All elements are always arranged vertically on the form in a strict order, so
the root of the file is an array of objects.

:<json jsonobj attributes: Contains several pairs where each name can only be one of the reserved attribute's keynames. The set of pairs depends on the type of the element. ``Note:`` if the element has no attributes the *null* value is correct.
:<json string type: Can only be one of the reserved elem's keynames.

Elements
========

Checkbox
---------

**Keyname**

checkbox

**Attributes**

* json string field: The name of the layer's field to which this element saves its value. Can be *null* if there was no field selected.
* json bool init_value: Initial value: on/off.
* json bool last: Whether to keep value for further sessions of data collection.
* json string text: Text which displays near the box of this checkbox. Can be void string.

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

Combobox
---------

**Keyname**

combobox

**Attributes**

* json bool allow_adding_values: Whether to allow user to add own items to this combobox during data collection.
* json string field: The name of the layer's field to which this element saves its value. Can be *null* if there was no field selected.
* json bool input_search: Whether to show all corresponding items from this combobox at the time when user types some text.
* json bool last: Whether to keep value for further sessions of data collection.
* json int ngw_id: Currently always = -1.
* json array values: The list of items for this combobox. An array of JSON objects where each one has two (or three) pairs of name-values. See below.
* json string alias: Alias (displayed value) for the item. Can not be void string.
* json string name: Name (inner value) for the item. Can not be void string.
* json bool default: If exists: whether this item is showed as default one before user enters this combobox. The value is always true.

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

Coordinates
------------

**Keyname**

coordinates

**Attributes**

* json int crs: Coordinate Reference System. Currently always = 0 which refers to "EPSG:4326" (WGS 84).
* json string field_lat: The name of the layer's field to which this element saves its latitude value. Can be *null* if there was no field selected.
* json string field_long: The name of the layer's field to which this element saves its longitude value. Can be *null* if there was no field selected.
* json int format: Format of the string. Currently always = 0 which refers to "dd.dddddd".
* json bool hidden: Whether this element will not be shown to user on the mobile device.

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

Counter
--------

**Keyname**

counter

**Attributes**

* json string field: The name of the layer's field to which this element saves its value. ``Note:`` can be *null* if there was no field selected.
* json int increment: The value which is added to the current value each session of data collection. Range: from 1 to 65535.
* json int init_value: Initial value from which incrementing starts. Range: from 0 to 65535.
* json string prefix: Text string to add before the current counter's number. Can be void string.
* json string suffix: Text string to add after the current counter's number. Can be void string.
* json string prefix_from_list: Currently unused and is always null.
* json string suffix_from_list: Currently unused and is always null.

**Example**

.. sourcecode:: json

    {
      "attributes" : {
         "field" : "id_collector",
         "increment" : 1,
         "init_value" : 0,
         "prefix" : "XX",
         "prefix_from_list": null,
         "suffix" : "NV",
         "suffix_from_list": null
      },
      "type" : "counter"
    }

Date & Time
------------

**Keyname**

date_time

**Attributes**

* json int date_type: What to save for this date-time element. Possible types: 1) 1 for date, 2) 2 for time, 3) 3 for date and time.
* json string datetime: Initial value for this date-time element. The string with date is always written in the specific format. Possible formats (according to types): 1) yyyy-MM-dd, 2) HH:mm:ss, 3) yyyy-MM-dd HH:mm:ss. ``Note:`` can be *null* which means that the current date/time/date-time will be written on the mobile device.
* json string field: The name of the layer's field to which this element saves its value. Can be *null* if there was no field selected.
* json bool last: Whether to keep value for further sessions of data collection.

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

Distance meter
---------------

**Keyname**

distance

**Attributes**

* json string field: The name of the layer's field to which this element saves its value. Can be *null* if there was no field selected.

**Example**

.. sourcecode:: json

   {
       "attributes": {
           "field": "dist_to_target"
       },
       "type": "distance"
   }

Double combobox
----------------

**Keyname**

double_combobox

**Attributes**

* json string field_level1: The name of the layer's field to which the first (main) combobox saves its value. Can be *null* if there was no field selected.
* json string field_level2: The name of the layer's field to which the second (dependant) combobox saves its value. Can be *null* if there was no field selected.
* json bool last: Whether to keep values for further sessions of data collection.
* json array values: The list of items for the first (main) combobox. An array of JSON objects where each one has three (or four) pairs of name-values. See below. ``Note:`` if user had not defined any items for the first (main) combobox - the stub item is pasted with "alias":"--" and "name":"-1".
* json string alias: Alias (displayed value) for the item.
* json string name: Name (inner value) for the item.
* json bool default: If exists: whether this item is showed as default one before user enters the first (main) combobox. The value is always true.
* json array values: The list of items for the second (dependant) combobox. An array of JSON objects where each one has two (or three) pairs of name-values.

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

Label
------

**Keyname**

text_label

**Attributes**

* json string text: Text which displays in this text label. Can be a void string.

**Example**

.. sourcecode:: json

   {
     "attributes" : {
        "text" : "Biotope"
     },
     "type" : "text_label"
   }

Photo
------

**Keyname**

photo

**Attributes**

* json int gallery_size: The maximum amount of photos user can make. Range: from 1 to 5.

**Example**

.. sourcecode:: json

    {
      "attributes" : {
         "gallery_size" : 1
      },
      "type" : "photo"
    }

Radiogroup
-----------

**Keyname**

radio_group

**Attributes**

* json string field: The name of the layer's field to which this element saves its value. Can be *null* if there was no field selected.
* json bool last: Whether to keep value for further sessions of data collection.
* json array values: The list of items for this combobox. An array of JSON objects where each one has two (or three) pairs of name-values. See below. ``Note:`` the array must have at least two items and one of them is always default.
* json string alias: Alias (displayed value) for the item. Can not be void string.
* json string name: Name (inner value) for the item. Can not be void string.
* json bool default: If exists: whether this item is showed as default one before user enters this radiogroup. The value is always true.

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

Signature
----------

**Keyname**

signature

**Attributes**

No attributes

**Example**

.. sourcecode:: json

    {
      "attributes" : null,
      "type" : "signature"
    }

Space
------

**Keyname**

space

**Attributes**

No attributes

**Example**

.. sourcecode:: json

    {
      "attributes" : null,
      "type" : "space"
    }

Splitted combobox
------------------

**Keyname**

split_combobox

**Attributes**

* json string field: The name of the layer's field to which this element saves its value. Can be *null* if there was no field selected.
* json bool last: Whether to keep value for further sessions of data collection.
* json array values: The list of items for this combobox.
* json string label1: Left caption above the combobox.
* json string label2: Right caption above the combobox.
* json string alias: Left alias (displayed value) for the item. Can not be void string.
* json string alias2: Right alias (displayed value) for the item. Can not be void string.
* json string name: Name (inner value) for the item. Can not be void string.
* json bool default: If exists: whether this item is showed as default one before user enters this combobox. The value is always true.

**Example**

.. sourcecode:: json

   {
       "attributes": {
           "field": "tree_type",
           "label1": "Latin name",
           "label2": "Italian name",
           "last": false,
           "values": [
               {
                   "alias": "Viburnum lantana",
                   "alias2": "Viburno",
                   "default": true,
                   "name": "1472"
               },
               {
                   "alias": "Buxus sempervirens",
                   "alias2": "Bosso",
                   "name": "6741"
               },
               {
                   "alias": "Prunus persica",
                   "alias2": "Pesco",
                   "name": "1128"
               }
           ]
       },
       "type": "split_combobox"
   }

Tabs
-----

**Keyname**

tabs

**Attributes**

No attributes

**Other keys**

* json array pages: An array of objects where each has the following keys: "caption", "elements" and optionally "default".
* json string caption: A text which is displayed in the header of the page.
* json array elements: An array of objects similar to the main form array of objects.
* json bool default: If this key exists and is equal to true than this page is default-showed to the user.

**Example**

.. sourcecode:: json

   {
       "attributes": {
       },
       "pages": [
           {
               "caption": "TAB 1",
               "elements": [
                   {
                       "attributes": {
                           "date_type": 2,
                           "datetime": null,
                           "field": "field_1",
                           "last": false
                       },
                       "type": "date_time"
                   },
                   {
                       "attributes": {
                           "field": "field_2"
                       },
                       "type": "distance"
                   }
               ]
           },
           {
               "caption": "TAB 2",
               "elements": [
               ]
           },
           {
               "caption": "TAB 3",
               "default": true,
               "elements": [
                   {
                     "attributes" : {
                        "text" : "Some text"
                     },
                     "type" : "text_label"
                   }
               ]
           }
       ],
       "type": "tabs"
   }

Text edit
----------

**Keyname**

text_edit

**Attributes**

* json string field: The name of the layer's field to which this element saves its value. Can be *null* if there was no field selected.
* json bool last: Whether to keep value for further sessions of data collection.
* json int max_string_count: How much strings (not symbols!) can be entered to this text edit.
* json bool ngid_login: Whether to replace the value of this text edit with NextGIS ID login. Mutually exclusive with "ngw_login".
* json bool ngw_login: Whether to replace the value of this text edit with NextGIS Web login. Mutually exclusive with "ngid_login".
* json bool only_figures: Whether to allow only numbers for user input to this text edit.
* json string text: Initial text which displays in this text edit before user enters it. Can be a void string.

**Example**

.. sourcecode:: json

    {
      "attributes" : {
         "field" : "researcher",
         "last" : true,
         "max_string_count" : 1,
         "ngid_login": false,
         "ngw_login" : false,
         "only_figures" : false,
         "text" : "Researcher"
      },
      "type" : "text_edit"
    }
