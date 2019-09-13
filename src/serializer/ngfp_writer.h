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

#include "serializer/ngfp_common.h"
#include "core/project.h"
#include "mockup/elemview.h"
#include "mockup/elemviews/ngmform_view.h"

#include <QJsonDocument>


namespace Fb
{
namespace Serializer
{

using namespace Core;
using namespace Mockup;


class NgfpWriter
{
    public:

     static void saveNgfp (QString file_path, const Layer *layer, const NgmFormView *form);

    protected:

     static QJsonDocument metaToJson (const Layer *layer);
     static QJsonDocument formToJson (const NgmFormView *form, const Layer *layer);
     static QJsonArray containerToJson (const Container *container, const Layer *layer);
     static QJsonValue elemViewToJson (const ElemView *elemview, const Layer *layer);
     static QJsonValue attrToJson (const Attr *attr);
     static QJsonValue fieldSlotToJson (const Layer *layer, const Elem *elem, QString field_slot);

     static void modifySpecificElemView (QJsonValue &j_elemview, const ElemView *elemview,
                                         const Layer *layer);
};


}
}



