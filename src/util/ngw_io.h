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

#include "util/ngw_types.h"
#include "core/geom_types.h"
#include "core/srs_types.h"
#include "core/field.h"

#include <QString>
#include <QList>

#include <set>

namespace Fb
{
namespace Util
{


/*!
 * @brief An interface with methods for working with NextGIS Web which are required in other
 * libraries of this project.
 * @see Gui::NgwDialog
 */
class NgwIo
{
    public:

     NgwIo ();
     virtual ~NgwIo ();

     virtual void reset () { }
     virtual QString getLastError () { return QString(); }

     virtual bool getRootResources (QList<NgwResourceData> &resources,
                                    QString base_url, QString login, QString password,
                                    const std::set<NgwResourceType> &allowed_types) = 0;
     virtual bool getResources (QList<NgwResourceData> &resources,
                                QString base_url, int resource_id,
                                const std::set<NgwResourceType> &allowed_types) = 0;
     virtual bool getLayerInfo (NgwLayerInfo &layer_info, QString base_url,
                                int resource_id) = 0;
     virtual bool createLayer (int &new_layer_id, const NgwLayerInfo &layer_info, QString base_url,
                               int resource_group_id) = 0;
     virtual bool createForm (int &new_form_id, QString ngfp_path, QString base_url,
                              int layer_id) = 0;
};


}
}
