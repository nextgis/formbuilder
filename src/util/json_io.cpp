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

#include "json_io.h"

using namespace Fb;
using namespace Util;


void JsonIo::write (QString file_path, const QJsonDocument &j_doc) // STATIC
{
    QFile file(file_path);
    if (!file.open(QIODevice::WriteOnly))
    {
        throw QObject::tr("Unable to open %1 for writing").arg(file_path);
        return;
    }

    if (file.write(j_doc.toJson()) == -1)
    {
        throw QObject::tr("Unable to write %1").arg(file_path);
        return;
    }
}



