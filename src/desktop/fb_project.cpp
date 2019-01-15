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

#include "fb_project.h"

using namespace Fb;
using namespace Core;


FbProject::FbProject ():
    Project(),
    file_path("")
{

}

FbProject::~FbProject ()
{
}


void FbProject::initialize (GeomType geom_type)
{
    layer = new Layer(tr("Layer"), geom_type, SrsType::Epsg4326);
//    layer->addField("Field", FieldType::String);
    layers.insert(layer); // will take ownership on it

    is_initialized = true;
}


const QList<Field *> &FbProject::layer0_getFields() const
{
    return layer->getFields();
}

void FbProject::layer0_modifyFields (const QList<FieldInfo> &new_fields_info)
{
    layer->modifyFields(new_fields_info);
}

void FbProject::layer0_selectField (int field_index, const Core::Elem *elem, QString field_slot)
{
    layer->selectField(field_index, elem, field_slot);
}

void FbProject::layer0_clearAllBindings ()
{
    layer->clearAllBindings();
}

void FbProject::layer0_updateAllBindings ()
{
    layer->updateAllBindings();
}

void FbProject::layer0_resetBindingsForElem (const Elem *elem)
{
    layer->resetBindingsForElem(elem);
}



