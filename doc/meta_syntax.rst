.. sectionauthor:: Mikhail Gusev <mikhail.gusev@nextgis.ru>

General
=======

This page describes the syntax of "meta.json" file which is a part of NextGIS
Formbuilder project file (with ".ngfp" extension). The file uses the JSON
specification rules and is encoded in UTF-8.

Syntax
======

.. http:options::

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
          "ngw_connection" : {
             "id" : 40,
             "login" : "admin",
             "password" : "strongpassword",
             "url" : "http://mikhail.nextgis.com"
          },
          "srs" : {
             "id" : "4326"
          },
          "version" : "2.1"
       }

   The root of the file has a json-object type and consists of the following pairs.

   :<json array fields: The list of the layer fields. Fully corresponds to the NextGIS Web fields description syntax.
   :<json string geometry_type: Layer's geometry type. Fully corresponds to the NextGIS Web list of geometry types.
   :<json jsonobj ngw_connection: All required parameters to connect to selected NextGIS Web layer. ``Note:`` if there was no NextGIS Web layer selected for this project - this value will be *null*.
   :<json jsonobj srs: Layer's Spatial Reference System. Fully corresponds to the NextGIS Web SRS description syntax.
   :<json string version: Format version of the Formbuilder project file.
