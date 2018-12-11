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

#include "field.h"
#include "geom_types.h"
#include "srs_types.h"

#include <QObject>
#include <QHash>

namespace Fb
{
namespace Core
{


const int LAYER_MIN_FIELD_COUNT = 0;
const int LAYER_MAX_FIELD_COUNT = 256;
const int LAYER_NAME_MAX_LEN = 256;

// TEMP:
struct NgwConnection
{
    int resource_id;
    QString login;
    QString password;
    QString base_url;
};

struct FieldInfo
{
    QString name;
    QString alias;
    FieldType type;
};

inline bool operator== (const FieldInfo &o1, const FieldInfo &o2)
{
    if (o1.name == o2.name &&
        o1.alias == o2.alias &&
        o1.type == o2.type)
        return true;
    return false;
}

inline uint qHash (const FieldInfo &field_info, uint seed)
{
    return ::qHash(field_info.name, seed) ^
           ::qHash(field_info.alias, seed) ^
           ::qHash(static_cast<uint>(field_info.type), seed);
}


/*!
 * \brief A Layer represents a GIS "layer" with a set of fields. The fields of a layer can be bound
 * to elements in order to collect data on the target device.
 */
class Layer final
{
    friend class Project;

    public:

     explicit Layer (QString name, GeomType geom_type, SrsType srs_type);
     ~Layer ();

     inline QString getName () const { return name; }
     inline void setName (QString new_name) { name = new_name; }
     inline GeomType getGeomType () const { return geom_type; }
     inline void setGeomType (GeomType new_geom_type) { geom_type = new_geom_type; }
     inline SrsType getSrsType () const { return srs_type; }
     inline void setSrsType (SrsType new_srs_type) { srs_type = new_srs_type; }
     inline QString getNgwUrl () const { return ngw_url; }
     inline void setNgwUrl (QString new_ngw_url) { ngw_url = new_ngw_url; }
     inline const NgwConnection &getNgwConnection () const { return ngw_con; }
     inline void setNgwConnection (NgwConnection new_ngw_con) { ngw_con = new_ngw_con; }

     inline const QList<Field*> &getFields () const { return fields; }

     Field *getField (int index);
     Field *getField (QString field_name);
     Field *getField (const Elem *elem, QString slot);

     static bool areNamesEqual (QString field_name1, QString field_name2);
     static bool isNameCorrect (QString name);
     QPair<QString, QString> createUniqueFieldName ();

     bool addField (QString field_name, QString field_alias, FieldType field_type);
     void removeField (QString field_name);

     void unbindElem (const Elem *elem, QString slot);
     void unbindElem (const Elem *elem);
     void clearAllBindings ();
     void updateAllBindings ();
     void resetBindingsForElem (const Elem *elem);

     void selectField (int field_index, const Elem *elem, QString field_slot);

     void modifyFields (const QList<FieldInfo> &new_fields_info);

    private:

     QString name;
     GeomType geom_type;
     SrsType srs_type;

     // TEMP:
     QString ngw_url; // QString ngw_view_url;
     NgwConnection ngw_con; // TODO: should be moved to a specific project (e.g. FBProject)

     // TODO: make a set/map of fields in order to supply uniqueness. The main problem: field names
     // must be unique case-insensitevely.
     QList<Field*> fields;
};


}
}
