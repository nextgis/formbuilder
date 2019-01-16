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

#include "core/project.h"
#include "core/layer.h"

#include <QSharedPointer>

using namespace Fb;
using namespace Core;


/*!
 * \brief A project for NextGIS Formbuilder 2.X. It operates only one layer and has an obligatory
 * NgForm elem.
 */
class FbProject: public Project
{
    Q_OBJECT

    public:

     FbProject ();
     virtual ~FbProject ();

     void initialize (GeomType geom_type);

     inline QString getFilePath () const { return file_path; }
     inline void setFilePath (QString new_file_path) { file_path = new_file_path; }

     const Layer *layer0_get () const { return layer; }
     const QList<Field*> &layer0_getFields () const;
     void layer0_modifyFields (const QList<FieldInfo> &new_fields_info);
     void layer0_selectField (int field_index, const Core::Elem *elem, QString field_slot);
     void layer0_clearAllBindings ();
     void layer0_updateAllBindings ();
     void layer0_resetBindingsForElem (const Elem *elem);

    //protected:

     Layer *layer; // will point to the 0 layer of the base class

     QString file_path;

     QString copy_ngw_base_url = ""; // used for downloaded ngw forms to copy features
     int copy_ngw_res_id = -1;
};



