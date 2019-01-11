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

#include "coordinates.h"

#include "attr_registrar.h"

using namespace Fb;
using namespace Core;


Coordinates::Coordinates (QString key_name):
    Elem(key_name)
{
    attr_srs        = (Enum*)     this->addAttr(new Enum("srs", {"EPSG:4326"},
                                                         0));
    attr_srs_format = (Enum*)     this->addAttr(new Enum("srs_format", {"dd.dddddd"},
                                                         0));
    attr_is_hidden  = (Boolean*)  this->addAttr(new Boolean("coords_is_hidden", false));

    fslot_lat = this->addFieldSlot("field_lat");
    fslot_lat = this->addFieldSlot("field_lon");
}

Coordinates::~Coordinates ()
{
}



