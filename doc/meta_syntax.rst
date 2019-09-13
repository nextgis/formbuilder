.. sectionauthor:: Mikhail Gusev <mikhail.gusev@nextgis.ru>

General
=======

This page describes the syntax of "meta.json" file which is a part of NextGIS
Formbuilder project file (with ".ngfp" extension). The file uses the JSON
specification rules and is encoded in UTF-8.

Syntax
======

Example of the "meta.json" file:

.. sourcecode:: json

   {
      "fields" : [
         {
            "datatype" : "INTEGER",
            "display_name" : "Amount of objects",
            "keyname" : "amount"
         },
         {
            "datatype" : "REAL",
            "display_name" : "Size of area",
            "keyname" : "area"
         },
         {
            "datatype" : "STRING",
            "display_name" : "Comments",
            "keyname" : "comments"
         }
      ],
      "geometry_type" : "POINT",
      "ngw_connection" : null,
      "srs" : {
         "id" : "4326"
      },
      "version" : "3.0"
   }

The root of the file has a json-object type and consists of the following pairs.

:<json array fields: The list of the layer fields. Fully corresponds to the NextGIS Web fields description syntax.
:<json string geometry_type: Layer's geometry type. Fully corresponds to the NextGIS Web list of geometry types.
:<json jsonobj ngw_connection: Currently is a null value.
:<json jsonobj srs: Layer's Spatial Reference System. Fully corresponds to the NextGIS Web SRS description syntax.
:<json string version: Format version of the .ngfp file.
