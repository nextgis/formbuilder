.. sectionauthor:: Mikhail Gusev <mikhail.gusev@nextgis.ru>

General
=======

This page describes the syntax of "form.json" file, which is a part of NextGIS Formbuilder project file (with ".ngfp" extension). The file uses the JSON specification rules and is encoded in UTF-8.

Syntax
======

Example of the "form.json" file which represents a form with two elements for entering person's name:

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

All elements are always arranged vertically on the form in a strict order, so the root of the file has an array type. Each item of the array has an object type and contains two pairs with the names: "attributes" and "type". 

    :<json jsonobj attributes: Contains several pairs where each name can only be one of the reserved attribute's keynames. The set of pairs depends on the type of the element. Note: if the element has no attributes the *null* value is correct.
    :<json string type: Can only be one of the reserved elem's keynames.

Elements
========

Text
----

Keyname 

    **text_label**

Attributes

    :<json string text: Text which displays in this text label. Can be void string.
    
Example

.. sourcecode:: json

    {
      "attributes" : {
         "text" : "Biotope"
      },
      "type" : "text_label"
    }

Space
-----

Keyname 

    **text_label**

Attributes

    No attributes.
    
Example

.. sourcecode:: json

    {
      "attributes" : null,
      "type" : "space"
    }

Text edit
---------

Keyname 

    **text_edit**

Attributes

    :<json string field: The name of the layer's field to which this element saves its value. Note: the value can be *null* if there was no field selected.
    :<json bool last: Whether to keep value for further sessions of data collection.
    :<json int max_string_count: How much strings (not symbols!) can be entered to this text edit. Range: from 1 to 65535.
    :<json bool ngw_login: Whether to replace the value of this text edit with NGW login. 
    :<json bool only_figures: Whether to allow only numbers for user input to this text edit.
    :<json string text: Initial text which displays in this text edit before user enters it. Can be void string.
    
Example

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
    
Combobox
--------

Keyname 

    **combobox**

Attributes

    :<json bool allow_adding_values: Whether to allow user to add own items to this combobox during data collection.
    :<json string field: The name of the layer's field to which this element saves its value. Note: the value can be *null* if there was no field selected.
    :<json bool input_search: Whether to show all corresponding items from this combobox at the time when user types some text. 
    :<json bool last: Whether to keep value for further sessions of data collection.
    :<json int ngw_id: The NGW resource identifier for the NGW lookup table or -1 if this combobox has no such connection.
    :<json array values: The list of items for this combobox. An array of JSON objects where each one has two (or three) pairs of name-values. See below.
    
    :<json string alias: Alias (displayed value) for the item. Can not be void string.
    :<json string name: Name (inner value) for the item. Can not be void string.
    :<json bool default: Whether this item is showed as default one before user enters this combobox. The value is always true.

Example

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
               "alias" : "Jungle tree",
               "name" : "3"
            }
         ]
      },
      "type" : "combobox"
   }
    
    
    
    
    
    