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

#include <QString>
#include <QList>
#include <QScopedPointer>

#include "ogrsf_frmts.h"

namespace Fb
{
namespace Util
{

struct GDALDatasetDeleter
{
    static inline void cleanup (GDALDataset *dataset)
    {
        GDALClose(dataset);
    }
};

using GDALDatasetPtr = QScopedPointer<GDALDataset, GDALDatasetDeleter>;

class CsvIo
{
    public:

     static void read (QString file_path, QList<QStringList> &csv_contents);
};


}
}
