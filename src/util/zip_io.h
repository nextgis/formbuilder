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

#include "ogrsf_frmts.h"

#include <QObject>
#include <QString>

namespace Fb
{
namespace Util
{


struct ZipFileInfo
{
    QString file_path;
    QString file_name_in_zip;
};

class ZipIo
{
    public:

     static void writeFiles (QString zip_path, QList<ZipFileInfo> files);
     static void readFile (QString zip_path, QString file_path, std::string &file_contents);

    protected:

     static bool addFileToZip (const CPLString &file_path, const CPLString &zip_file_name,
                               void *h_zip, GByte **paby_buffer, size_t buffer_size);
};


}
}
